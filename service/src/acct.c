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

#include "acme.h"
#include "sns.h"

#pragma list

/* Static variables. */
static short account_filenum;
static char pathmon_name[32];
static char* sns_serverclass = "SNS-SERVER";

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
  publish_with_post_rq_def publish_rq;
  publish_with_post_other_rp_def publish_rp;
  account_def account;
  short rc;

  /* Get the account record. */
  FILE_SETKEY_(account_filenum, rq->account_number, sizeof(rq->account_number),
               0, SETKEY_EXACT);
  rc = FILE_READ64_(account_filenum, (char _ptr64*)&account, sizeof(account));
  
  if (rc == 0 && *account.account_detail.phone_number == '+') {    
   
    /* Send the alert. */
    memset(&publish_rq, 0, sizeof(publish_rq));
    publish_rq.lightwave_rq_header.rq_code = rq_publish_with_post;  
    strncpy(publish_rq.phone_number, account.account_detail.phone_number, sizeof(account.account_detail.phone_number));
    strcpy(publish_rq.message_rw, rq->alert_message);

    SERVERCLASS_SENDL_((char*)pathmon_name, (short)strlen(pathmon_name),
                       (char*)sns_serverclass, (short)strlen(sns_serverclass),
                       (char*)&publish_rq, (char*)&publish_rp,
                       sizeof(publish_rq), sizeof(publish_rp));
  }

  /* Alerts are fire and forget so reply immediately. */
  memset(&rp, 0, sizeof(rp));
  REPLYX((const char*)&rp, sizeof(rp));
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