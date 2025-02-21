/**
 * Copyright (c) 2019 NuWave Technologies, Inc. All rights reserved.
 */

#pragma nolist

#include <cextdecs>
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <tal.h>
#include <zsysc>
#include <time.h>


#include "acme.h"
#include "lw.h"
#include "sendg.h"
#include "twlsms.h"


#pragma list

/* Static variables. */
static short account_filenum;
static char pathmon_name[32];
static char* sg_serverclass = "SG-SERVER";

static int sms_enabled = 0;
static int email_enabled = 0;

/* Static function prototypes. */
static int activate_transaction(const char* type);
static void alert_account(void* request);
static void create_account(void* request);
static void delete_account(void* request);
static void get_account(void* request);
static void get_accounts(void* request);
static void reply_with_error(rp_code_def rp_code, error_code_def error_code,
                             const char* error_format, ...);
static void update_account(void* request);
static void populate_sendgrid_email_req_and_send(
    account_def account, alert_account_rq_def* alert_account_request);
static void
populate_twilio_sms_req_and_send(account_def account,
                                 alert_account_rq_def* alert_account_request);

static const char* format_numeric(long long value, short scale);
/*
static char* extract_value(const char* str, const char* key);

*/
static void get_current_date(char* buffer);

static const char* format_numeric(long long value, short scale) {

  short rc;
  char iformat[256];
  short count = 0;
  char eformat[32];
  static char buffer[64];
  char* result = buffer;
  short length = 0;
  char* p;

  /* Build the format. */
  memset(eformat, 0, sizeof(eformat));
  p = eformat;
  p += sprintf(eformat, "M'ZZZZZZZZ9.");
  memset(p, '9', scale);
  strcat(p, "'");

  rc = FORMATCONVERTX(iformat, (short)sizeof(iformat), eformat,
                      (short)strlen(eformat), NULL, &count, 1);

  if (rc <= 0) {
    snprintf(buffer, sizeof(buffer), "FORMATCONVERTX=%d", rc);
  } else {
    struct {
      long long* dataptr;
      short datatype;
      short databytes;
      short occurs;
    } list[1];

    list[0].dataptr = &value;
    list[0].datatype = (short)(((scale & 0XFF) << 8) | 6);
    list[0].databytes = sizeof(value);
    list[0].occurs = 1;

    rc = FORMATDATAX(buffer, (short)sizeof(buffer), 1, &length, (short*)iformat,
                     (short*)list, 1, 0);

    if (rc != 0) {
      snprintf(buffer, sizeof(buffer), "FORMATDATAX=%d\n", rc);
    } else {
      buffer[length] = 0;
    }
  }

  /* Trim leading spaces. */
  while (*result == ' ') {
    result++;
  }

  return result;
}
/* Static functions. */
static int activate_transaction(const char* type) {
  int cc;
  short message_tag;

  LASTRECEIVE(, &message_tag);
  cc = ACTIVATERECEIVETRANSID(message_tag);

  if (!_status_eq(cc)) {
    reply_with_error(RP_CODE_INTERNAL_ERROR, ERROR_CODE_IO_ERROR,
                     "The %s card operation requires a transaction.", type);
    return 1;
  }

  return 0;
}

static void alert_account(void* request) {
  alert_account_rq_def* rq = (alert_account_rq_def*)request;
  alert_account_rp_def rp;
  account_def account;
  short rc;

  /* Get the account record. */
  FILE_SETKEY_(account_filenum, rq->account_number, sizeof(rq->account_number),
               0, SETKEY_EXACT);
  rc = FILE_READ64_(account_filenum, (char _ptr64*)&account, sizeof(account));

  if (sms_enabled != 0 && rc == 0) {
    populate_twilio_sms_req_and_send(account, rq);
  }

  if (email_enabled && rc == 0) {
    populate_sendgrid_email_req_and_send(account, rq);
  }

  /* Alerts are fire and forget so reply immediately. */
  memset(&rp, 0, sizeof(rp));
  REPLYX((const char*)&rp, sizeof(rp));
}

static void
populate_twilio_sms_req_and_send(account_def account,
                                 alert_account_rq_def* alert_account_request) {
  static char* twilio_sms_serverclass = "TWL-SERVER";
  short rc;
  char* p;
  send_smsrq_def send_sms_req;
  send_sms_201_rp_def send_sms_rep;
  /* Reset request*/
  memset(&send_sms_req, 0, sizeof(send_sms_req));
  /* Get twilio account sid - env variable should be in CAPS*/
  if ((p = getenv("TWILIO-ACCOUNT-SID")) == NULL) {
    printf("ENV PARAM twilio-account-sid is not set \n");
  } else {
    strncpy(send_sms_req.account_sid, p, sizeof(send_sms_req.account_sid));
  }

  /* Get twilio phone no to send the SMS from*/
  if ((p = getenv("TWILIO-PHONE-NO")) == NULL) {
    printf("ENV PARAM twilio-phone-no is not set \n");
  } else {
    strncpy(send_sms_req.from_rw, p, sizeof(send_sms_req.from_rw));
  }

  /*Set header with request code*/
  send_sms_req.lightwave_rq_header.rq_code = rq_send_sms;

  /* Read the phone number from account details*/
  strcpy(send_sms_req.to_rw, account.account_detail.phone_number);

  /* Set SMS Body*/
  strcpy(send_sms_req.body, alert_account_request->alert_message);

  rc = SERVERCLASS_SENDL_((char*)pathmon_name, (short)strlen(pathmon_name),
                          (char*)twilio_sms_serverclass,
                          (short)strlen(twilio_sms_serverclass),
                          (char*)&send_sms_req, (char*)&send_sms_rep,
                          sizeof(send_sms_req), sizeof(send_sms_rep));
  if (rc != 0) {
    reply_with_error(RP_CODE_INTERNAL_ERROR, ERROR_CODE_IO_ERROR,
                     "File system error %d occured while attempting to "
                     "send a text message via twilio.",
                     (int)rc);
    return;
  }
}

static void populate_sendgrid_email_req_and_send(
    account_def account, alert_account_rq_def* alert_account_request) {
  short rc;
  post_send_rq_def sg_request;
  post_send_202_rp_def sg_rp;
  char today_date[11];

  memset(&sg_request, 0, sizeof(sg_request));

  sg_request.lightwave_rq_header.rq_code = rq_post_send;

  sg_request.post_send_v_3.personalizations_count = 1;
  sg_request.post_send_v_3.personalizations[0].to_count = 1;
  strcpy(sg_request.post_send_v_3.personalizations[0].to_rw[0].email,
         account.account_detail.email_address);
  strcpy(sg_request.post_send_v_3.from_rw.email,
         getenv("SG-NUWAVETECH-FROM-EMAIL"));
  strcpy(sg_request.post_send_v_3.subject, "ACME Transaction alert");
  strcpy(
      sg_request.post_send_v_3.personalizations[0].dynamic_template_data.user,
      alert_account_request->name_on_card);

  strcpy(sg_request.post_send_v_3.personalizations[0]
             .dynamic_template_data.vendor_name,
         alert_account_request->merchant_name);
  sg_request.post_send_v_3.personalizations[0]
      .dynamic_template_data.transaction_amount =
      atoi(format_numeric(alert_account_request->transaction_amount, 2));
  strcpy(sg_request.post_send_v_3.personalizations[0]
             .dynamic_template_data.transaction_id,
         alert_account_request->transaction_id);
  snprintf(sg_request.post_send_v_3.personalizations[0]
               .dynamic_template_data.cc_last_4,
           sizeof(sg_request.post_send_v_3.personalizations[0]
                      .dynamic_template_data.cc_last_4),
           "%-.4s", alert_account_request->card_number);
  strcpy(sg_request.post_send_v_3.personalizations[0]
             .dynamic_template_data.alert_message,
         alert_account_request->alert_message);

  get_current_date(today_date);
  strcpy(sg_request.post_send_v_3.personalizations[0]
             .dynamic_template_data.transaction_date,
         today_date);

  if (strlen(sg_request.post_send_v_3.personalizations[0]
                 .dynamic_template_data.transaction_id) > 0) {
    strcpy(sg_request.post_send_v_3.template_id,
           getenv("SG-TRANSACTION-TEMPLATE-ID"));
  } else {
    strcpy(sg_request.post_send_v_3.template_id,
           getenv("SG-CARD-STATUS-TEMPLATE-ID"));
  }
  if (alert_account_request->transaction_type == 1) {
    strcpy(sg_request.post_send_v_3.personalizations[0]
               .dynamic_template_data.transaction_type,
           "sale");
  } else {
    strcpy(sg_request.post_send_v_3.personalizations[0]
               .dynamic_template_data.transaction_type,
           "void");
  }

  rc = SERVERCLASS_SENDL_((char*)pathmon_name, (short)strlen(pathmon_name),
                          (char*)sg_serverclass, (short)strlen(sg_serverclass),
                          (char*)&sg_request, (char*)&sg_rp, sizeof(sg_request),
                          sizeof(sg_rp));
  if (rc != 0) {
    reply_with_error(RP_CODE_INTERNAL_ERROR, ERROR_CODE_IO_ERROR,
                     "File system error %d occured while attempting to "
                     "send an email via sendgrid.",
                     (int)rc);
    return;
  }
}

static void get_current_date(char* buffer) {
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  sprintf(buffer, "%02d/%02d/%04d", tm.tm_mon + 1, tm.tm_mday,
          tm.tm_year + 1900);
}

static void create_account(void* request) {
  create_account_rq_def* rq = (create_account_rq_def*)request;
  account_def account;
  account_number_def account_number;
  create_account_rp_def rp;
  short rc;

  /* Activate a transaction. */
  if (activate_transaction("create") != 0) {
    return;
  }

  /* Determine the next account number by reading the last record and adding 1.
   */
  FILE_SETKEY_(account_filenum, 0, 0, 0, 0, 0xE000);
  rc = FILE_READLOCK64_(account_filenum, (char _ptr64*)&account,
                        sizeof(account));

  if (rc == 0) {
    char tmp[sizeof(account_number) + 2];
    unsigned int next;

    memset(tmp, 0, sizeof(tmp));
    memcpy(tmp, account.account_number, sizeof(account_number));
    next = atoi(tmp);
    next += 1;
    sprintf(tmp, "%0*u", sizeof(account_number), next);
    memcpy(account_number, tmp, sizeof(account_number));
  } else if (rc == 1) {
    char tmp[sizeof(account_number) + 2];
    sprintf(tmp, "%0*u", sizeof(account_number), 1);
    *tmp = '9';
    memcpy(account_number, tmp, sizeof(account_number));
  } else {
    reply_with_error(RP_CODE_INTERNAL_ERROR, ERROR_CODE_IO_ERROR,
                     "File system error %d occured while attempting to "
                     "determine the next account number.",
                     (int)rc);
    return;
  }

  /* Initialize the account record and write it. */
  memset(&account, 0, sizeof(account));
  memcpy(account.account_number, account_number,
         sizeof(account.account_number));
  memcpy(&account.account_detail, &rq->account_detail,
         sizeof(account.account_detail));

  rc = FILE_WRITE64_(account_filenum, (char _ptr64*)&account, sizeof(account));

  if (rc != 0) {
    reply_with_error(RP_CODE_INTERNAL_ERROR, ERROR_CODE_IO_ERROR,
                     "File system error %d occured while attempting to "
                     "write the account record.",
                     (int)rc);
    return;
  }

  /* Initialize and send the reply. */
  memset(&rp, 0, sizeof(rp));
  memcpy(&rp.account, &account, sizeof(rp.account));
  REPLYX((const char*)&rp, sizeof(rp));
  RESUMETRANSACTION(0);
}

static void delete_account(void* request) {
  delete_account_rq_def* rq = (delete_account_rq_def*)request;
  account_def account;
  delete_account_rp_def rp;
  short rc;

  /* Activate a transaction. */
  if (activate_transaction("delete") != 0) {
    return;
  }

  /* Initialize reply. */
  memset(&rp, 0, sizeof(rp));

  /* Position at the account record. */
  FILE_SETKEY_(account_filenum, rq->account_number, sizeof(rq->account_number),
               0, SETKEY_EXACT);
  rc = FILE_READUPDATELOCK64_(account_filenum, (char _ptr64*)&account,
                              sizeof(account));

  if (rc != 0) {
    if (rc == 11) {
      reply_with_error(RP_CODE_NOT_FOUND, RP_CODE_NOT_FOUND,
                       "The account was not found.");
    } else {
      reply_with_error(RP_CODE_INTERNAL_ERROR, ERROR_CODE_IO_ERROR,
                       "File system error %d occured while attempting to "
                       "locate the account record for deletion.",
                       (int)rc);
    }
    return;
  }

  rc = FILE_WRITEUPDATE64_(account_filenum, 0, 0);

  if (rc != 0) {
    reply_with_error(RP_CODE_INTERNAL_ERROR, ERROR_CODE_IO_ERROR,
                     "File system error %d occured while attempting to "
                     "delete the account record.",
                     (int)rc);
    return;
  }

  /* Send the reply. */
  REPLYX((const char*)&rp, sizeof(rp));
  RESUMETRANSACTION(0);
}

static void get_account(void* request) {
  get_account_rq_def* rq = (get_account_rq_def*)request;
  account_def account;
  get_account_rp_def rp;
  short rc;

  /* Initialize reply. */
  memset(&rp, 0, sizeof(rp));

  /* Position at the account record. */
  FILE_SETKEY_(account_filenum, rq->account_number, sizeof(rq->account_number),
               0, SETKEY_EXACT);
  rc = FILE_READ64_(account_filenum, (char _ptr64*)&account, sizeof(account));

  if (rc != 0) {
    if (rc == 1) {
      rp.rp_code = RP_CODE_NOT_FOUND;
      REPLYX((const char*)&rp, sizeof(rp));
      return;
    } else {
      reply_with_error(RP_CODE_INTERNAL_ERROR, ERROR_CODE_IO_ERROR,
                       "File system error %d occured while attempting to "
                       "locate the account record.",
                       (int)rc);
      return;
    }
  }

  memcpy(&rp.account, &account, sizeof(rp.account));

  /* Send the reply. */
  REPLYX((const char*)&rp, sizeof(rp));
}

static void get_accounts(void* request) {
  account_def account;
  get_accounts_rp_def rp;
  size_t max;

  /* Initialize the reply. */
  memset(&rp, 0, sizeof(rp));

  /* Determine max records. */
  max = sizeof(rp.account) / sizeof(account_def);

  /* Position at start of file and read sequentially. */
  FILE_SETKEY_(account_filenum, 0, 0);
  while (rp.item_count < max) {

    if (FILE_READ64_(account_filenum, (char _ptr64*)&account,
                     sizeof(account_def)) != 0) {
      break;
    }

    memcpy(&rp.account[rp.item_count], &account, sizeof(account_def));
    rp.item_count++;
  }

  /* Send the reply. */
  REPLYX((const char*)&rp, sizeof(rp));
}

static void reply_with_error(rp_code_def rp_code, error_code_def error_code,
                             const char* error_format, ...) {
  error_rp_def rp;
  va_list list;

  va_start(list, error_format);

  memset(&rp, 0, sizeof(rp));
  rp.rp_code = rp_code;
  rp.error_detail.error_code = error_code;
  vsnprintf(rp.error_detail.error_message,
            sizeof(rp.error_detail.error_message), error_format, list);
  va_end(list);

  REPLYX((const char*)&rp, sizeof(rp));
  RESUMETRANSACTION(0);
  return;
}

static void update_account(void* request) {
  update_account_rq_def* rq = (update_account_rq_def*)request;
  account_def account;
  update_account_rp_def rp;
  short rc;

  /* Activate a transaction. */
  if (activate_transaction("update") != 0) {
    return;
  }

  /* Initialize reply. */
  memset(&rp, 0, sizeof(rp));

  /* Position at the account record. */
  FILE_SETKEY_(account_filenum, rq->account_number, sizeof(rq->account_number),
               0, SETKEY_EXACT);
  rc = FILE_READUPDATELOCK64_(account_filenum, (char _ptr64*)&account,
                              sizeof(account));

  if (rc != 0) {
    if (rc == 11) {
      reply_with_error(RP_CODE_NOT_FOUND, RP_CODE_NOT_FOUND,
                       "The account was not found.");
    } else {
      reply_with_error(RP_CODE_INTERNAL_ERROR, ERROR_CODE_IO_ERROR,
                       "File system error %d occured while attempting to "
                       "locate the account record for update.",
                       (int)rc);
    }
    return;
  }

  memcpy(&account.account_detail, &rq->account_detail,
         sizeof(account.account_detail));
  rc = FILE_WRITEUPDATE64_(account_filenum, (char _ptr64*)&account,
                           sizeof(account));

  if (rc != 0) {
    reply_with_error(RP_CODE_INTERNAL_ERROR, ERROR_CODE_IO_ERROR,
                     "File system error %d occured while attempting to "
                     "update the account record.",
                     (int)rc);
    return;
  }

  /* Send the reply. */
  memcpy(&rp.account, &account, sizeof(rp.account));
  REPLYX((const char*)&rp, sizeof(rp));
  RESUMETRANSACTION(0);
}

int main(int argc, char* argv[], char** envp) {

  int open_count;
  int cc;
  short rc;
  short filenum;
  unsigned short count_read;
  int done;
  char request[57344];
  short* rqCode;
  char* p;
  char* q;

  /* Check for SMS enabled. Default to no. */
  if ((p = getenv("ENABLE-SMS")) == NULL) {
    sms_enabled = 0;
  } else {
    sms_enabled = atoi(p);
  }

  if ((q = getenv("ENABLE-EMAIL")) == NULL) {
    email_enabled = 0;
  } else {
    email_enabled = atoi(q);
  }

  /* Get our pathmon name. */
  if ((p = getenv("PATHMON-NAME")) == NULL) {
    printf("PARAM PATHMON-NAME is not set.\n");
    return -1;
  }
  strncpy(pathmon_name, p, sizeof(pathmon_name));

  /* Open $RECEIVE */
  if ((rc = FILE_OPEN_("$RECEIVE", 8, &filenum, , , 0, 1)) != 0) {
    printf("Unable to open $RECEIVE, file system error %d\n", (int)rc);
    return -1;
  }

  /* Open the account file. */
  if ((rc = FILE_OPEN_(ACCOUNT_FILENAME, sizeof(ACCOUNT_FILENAME) - 1,
                       &account_filenum, 0, 0, 0, 1)) != 0) {
    printf("Unable to open account file, file system error %d\n", (int)rc);
    return -1;
  }

  /* Process requests messages. */
  for (done = 0, open_count = 0; !done;) {

    cc = (short)READUPDATEX(filenum, (char*)request, sizeof(request),
                            &count_read);

    if (!_status_eq(cc)) {
      FILE_GETINFO_(filenum, &rc);
      if (rc != 6) {
        printf("READX error %d\n", (int)rc);
        return -1;
      }
    }

    /* Can't process the request if there's no request code. */
    if (count_read < 2) {
      REPLYX(, , , , 2);
      continue;
    }

    /* Dispatch the request. */
    rqCode = (short*)request;
    switch (*rqCode) {
    case ZSYS_VAL_SMSG_OPEN:
      open_count++;
      REPLYX(NULL, 0);
      break;
    case ZSYS_VAL_SMSG_CLOSE:
      if (--open_count <= 0)
        done = 1;
      REPLYX(NULL, 0);
      break;
    case RQ_CODE_ALERT_ACCOUNT:
      alert_account(request);
      break;
    case RQ_CODE_CREATE_ACCOUNT:
      create_account(request);
      break;
    case RQ_CODE_DELETE_ACCOUNT:
      delete_account(request);
      break;
    case RQ_CODE_GET_ACCOUNT:
      get_account(request);
      break;
    case RQ_CODE_GET_ACCOUNTS:
      get_accounts(request);
      break;
    case RQ_CODE_UPDATE_ACCOUNT:
      update_account(request);
      break;
    default:
      REPLYX(, , , , 2);
      break;
    }
  }

  return 0;
}

/** End of file */
