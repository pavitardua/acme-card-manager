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
#include <strings.h>
#include <stdarg.h>
#include <tal.h>
#include <zsysc>

#include "acme.h"
/* #include "sns.h" */

#pragma list

/* Static variables. */
static short transaction_filenum;
static short card_filenum;
static char pathmon_name[32];
static char* acct_serverclass = "ACCT-SERVER";

/* Static function prototypes. */
static int activate_transaction(const char* type);
static void create_payment(void* request);
static const char* create_transaction_id(void);
static const char* format_numeric(long long value, short scale);
static void get_transaction(void* request);

static void get_transactions(void* request);
static int parse_int(const char* string, size_t len);
static void reply_with_error(rp_code_def rp_code, error_code_def error_code,
                             const char* error_format, ...);
static void reset_database(void);
static void void_payment(void* request);

/* Static functions. */
static int activate_transaction(const char* type) {
  int cc;
  short message_tag;

  LASTRECEIVE(, &message_tag);
  cc = ACTIVATERECEIVETRANSID(message_tag);

  if (!_status_eq(cc)) {
    reply_with_error(RP_CODE_BAD_REQUEST, ERROR_CODE_NO_TRANSACTION,
                     "The payment card was not found.",
                     "The %s payment operation requires a transaction.", type);
    return 1;
  }

  return 0;
}

static void create_payment(void* request) {
  create_payment_rq_def* rq = (create_payment_rq_def*)request;
  transaction_def transaction;
  create_payment_rp_def rp;
  card_def card;
  short rc;
  int card_yyyy;
  int card_mm;
  short date_and_time[8];
  long long exp_timestamp;
  long long now = JULIANTIMESTAMP();
  char* p;
  size_t i;
  const char* response_code;
  rp_code_def reply_code;

  /* Activate a transaction. */
  if (activate_transaction("create") != 0) {
    return;
  }

  /* Get the card and lock it for update. */
  FILE_SETKEY_(card_filenum, rq->payment_detail.card_number,
               sizeof(rq->payment_detail.card_number), 0, SETKEY_EXACT);
  rc = FILE_READUPDATELOCK64_(card_filenum, (char _ptr64*)&card, sizeof(card));

  if (rc != 0) {
    if (rc == 1) {
      reply_with_error(RP_CODE_NOT_FOUND, RP_CODE_NOT_FOUND,
                       "The payment card was not found.");
    } else {
      reply_with_error(RP_CODE_INTERNAL_ERROR, ERROR_CODE_IO_ERROR,
                       "File system error %d occured while attempting to "
                       "locate the payment card.",
                       (int)rc);
    }
    return;
  }

  /* Get card expiration year and month. index 0 - year, 1 - month, 2 - day */
  card_yyyy =
      parse_int(card.card_detail.exp_year, sizeof(card.card_detail.exp_year));
  card_mm =
      parse_int(card.card_detail.exp_month, sizeof(card.card_detail.exp_month));
  memset(date_and_time, 0, sizeof(date_and_time));
  date_and_time[0] = (short)card_yyyy;
  date_and_time[1] = (short)card_mm;

  /* Bump to midnight the next day. */
  if (date_and_time[1] == 12) {
    date_and_time[0]++;
    date_and_time[1] = 1;
  } else {
    date_and_time[1]++;
  }

  date_and_time[2] = 1;
  exp_timestamp = COMPUTETIMESTAMP(date_and_time);

  /* Validate the payment ... assume failure. */
  reply_code = RP_CODE_BAD_REQUEST;
  if (card_yyyy != parse_int(rq->payment_detail.exp_year,
                             sizeof(rq->payment_detail.exp_year)) ||
      card_mm != parse_int(rq->payment_detail.exp_month,
                           sizeof(rq->payment_detail.exp_month))) {
    response_code = RESPONSE_CODE_INV_DATE;
  } else if (exp_timestamp <= now) {
    response_code = RESPONSE_CODE_EXPIRED;
  } else if (card.card_detail.is_locked != 0) {
    response_code = RESPONSE_CODE_RESTRICTED;
  } else if (memcmp(card.card_detail.security_code,
                    rq->payment_detail.security_code,
                    sizeof(card.card_detail.security_code)) != 0) {
    response_code = RESPONSE_CODE_INV_CVV;
  } else if (rq->payment_detail.amount <= 0) {
    response_code = RESPONSE_CODE_INV_AMOUNT;
  } else if (-(card.card_detail.balance) + rq->payment_detail.amount >
             card.card_detail.spending_limit) {
    response_code = RESPONSE_CODE_INS_FUNDS;
  } else {
    response_code = RESPONSE_CODE_APPROVED;
    reply_code = RP_CODE_SUCCESS;

    /* Update the card balance. */
    card.card_detail.balance -= rq->payment_detail.amount;
    rc = FILE_WRITEUPDATE64_(card_filenum, (char _ptr64*)&card, sizeof(card));

    if (rc != 0) {
      reply_with_error(
          RP_CODE_INTERNAL_ERROR, ERROR_CODE_IO_ERROR,
          "File system error %d occured while attempting to update "
          "the card balance.",
          (int)rc);
      return;
    }
  }

  /* Create the payment transaction record and write it. */
  memset(&transaction, 0, sizeof(transaction));
  memcpy(transaction.transaction_id, create_transaction_id(),
         sizeof(transaction.transaction_id));
  transaction.transaction_type = TRANSACTION_TYPE_SALE;
  transaction.timestamp = now;
  memcpy(transaction.payment_response_code, response_code,
         sizeof(transaction.payment_response_code));
  memcpy(&transaction.payment_detail, &rq->payment_detail,
         sizeof(transaction.payment_detail));

  for (i = 0, p = transaction.payment_detail.merchant_name;
       i < sizeof(transaction.payment_detail.merchant_name); i++, p++) {
    *p = (char)toupper((int)*p);
  }

  rc = FILE_WRITE64_(transaction_filenum, (char _ptr64*)&transaction,
                     sizeof(transaction));

  if (rc != 0) {
    reply_with_error(RP_CODE_INTERNAL_ERROR, ERROR_CODE_IO_ERROR,
                     "File system error %d occured while attempting to write "
                     "the transaction record.",
                     (int)rc);
    return;
  }

  /* Initialize and send the reply. */
  memset(&rp, 0, sizeof(rp));
  rp.rp_code = reply_code;
  memcpy(&rp.transaction, &transaction, sizeof(rp.transaction));
  REPLYX((const char*)&rp, sizeof(rp));
  RESUMETRANSACTION(0);

  /**
   * If successful and the alert limit is met, then send an alert. Note that the
   * alert is not part of the transaction and we don't care if it fails.
   */

  if (reply_code == RP_CODE_SUCCESS &&
      rq->payment_detail.amount >= card.card_detail.alert_limit) {
    alert_account_rq_def alert_rq;
    alert_account_rp_def alert_rp;

    memset(&rq, 0, sizeof(rq));
    alert_rq.rq_code = RQ_CODE_ALERT_ACCOUNT;
    memcpy(alert_rq.account_number, card.card_detail.account_number,
           sizeof(alert_rq.account_number));

    /* Build and send the alert message. */
    snprintf(alert_rq.alert_message, sizeof(alert_rq.alert_message),
             "ACME Card account %-.4s:  A purchase in the amount of $%s at "
             "%-.*s has been charged to your account.",
             &card.card_number[12],
             format_numeric(transaction.payment_detail.amount, 2),
             sizeof(transaction.payment_detail.merchant_name),
             transaction.payment_detail.merchant_name);
    memcpy(alert_rq.card_number, card.card_number,
           sizeof(alert_rq.card_number));

    memcpy(alert_rq.name_on_card, card.card_detail.name_on_card,
           sizeof(alert_rq.name_on_card));

    alert_rq.transaction_amount = transaction.payment_detail.amount;
    memcpy(alert_rq.merchant_name, transaction.payment_detail.merchant_name,
           sizeof(alert_rq.merchant_name));
    alert_rq.transaction_type = transaction.transaction_type;
    memcpy(alert_rq.transaction_id, transaction.transaction_id,
           sizeof(alert_rq.transaction_id));

    SERVERCLASS_SENDL_((char*)pathmon_name, (short)strlen(pathmon_name),
                       (char*)acct_serverclass, (short)strlen(acct_serverclass),
                       (char*)&alert_rq, (char*)&alert_rp, sizeof(alert_rq),
                       sizeof(alert_rp));
  }
}

static const char* create_transaction_id(void) {
  static char id[sizeof(transaction_id_def) + 2];
  sprintf(id, "%0*lld", sizeof(transaction_id_def), JULIANTIMESTAMP());
  return id;
}

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

static void get_transaction(void* request) {
  get_transaction_rq_def* rq = (get_transaction_rq_def*)request;
  transaction_def transaction;
  get_transaction_rp_def rp;
  short rc;

  /* Initialize reply. */
  memset(&rp, 0, sizeof(rp));

  /* Position at the transaction record. */
  FILE_SETKEY_(transaction_filenum, rq->transaction_id,
               sizeof(rq->transaction_id), 0, SETKEY_EXACT);
  rc = FILE_READ64_(transaction_filenum, (char _ptr64*)&transaction,
                    sizeof(transaction));

  if (rc != 0) {
    if (rc == 1) {
      reply_with_error(RP_CODE_NOT_FOUND, RP_CODE_NOT_FOUND,
                       "The transaction was not found.");
    } else {
      reply_with_error(RP_CODE_INTERNAL_ERROR, ERROR_CODE_IO_ERROR,
                       "File system error %d occured while attempting to "
                       "locate the transaction.",
                       (int)rc);
    }
    return;
  }

  memcpy(&rp.transaction, &transaction, sizeof(rp.transaction));

  /* Send the reply. */
  REPLYX((const char*)&rp, sizeof(rp));
}

static void get_transactions(void* request) {
  transaction_def transaction;
  get_transactions_rp_def rp;
  size_t max;

  /* Initialize the reply. */
  memset(&rp, 0, sizeof(rp));

  /* Determine max records. */
  max = sizeof(rp.transaction) / sizeof(transaction_def);

  /* We want the latest transactions so position at end of file and read in
   * reverse. */
  FILE_SETKEY_(transaction_filenum, 0, 0, 0, 0, 0xE000);
  while (rp.item_count < max) {

    if (FILE_READ64_(transaction_filenum, (char _ptr64*)&transaction,
                     sizeof(transaction_def)) != 0) {
      break;
    }

    memcpy(&rp.transaction[rp.item_count], &transaction,
           sizeof(transaction_def));
    rp.item_count++;
  }

  /* Send the reply. */
  REPLYX((const char*)&rp, sizeof(rp));
}

static int parse_int(const char* string, size_t len) {
  char tmp[32];

  memcpy(tmp, string, len);
  tmp[len] = 0;
  return atoi(tmp);
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

static void reset_database(void) {
  transaction_def transaction;
  card_def card;

  BEGINTRANSACTION();

  LOCKFILE(card_filenum);
  LOCKFILE(transaction_filenum);

  /* Reset card balances to 0. */
  FILE_SETKEY_(card_filenum, 0, 0);
  while (FILE_READLOCK64_(card_filenum, (char _ptr64*)&card, sizeof(card)) ==
         0) {
    card.card_detail.balance = 0;
    FILE_WRITEUPDATE64_(card_filenum, (char _ptr64*)&card, sizeof(card));
  }

  /* Clear the transaction log. */
  FILE_SETKEY_(transaction_filenum, 0, 0);
  while (FILE_READLOCK64_(transaction_filenum, (char _ptr64*)&transaction,
                          sizeof(transaction_def)) == 0) {
    FILE_WRITEUPDATE64_(transaction_filenum, 0, 0);
  }

  ENDTRANSACTION();

  printf("Database reset completed.\n");
  return;
}

static void void_payment(void* request) {
  void_payment_rq_def* rq = (void_payment_rq_def*)request;
  transaction_def paymentTransaction;
  transaction_def voidTransaction;
  card_def card;
  void_payment_rp_def rp;
  short rc;

  /* Activate a transaction. */
  if (activate_transaction("void") != 0) {
    return;
  }

  /* Read the payment transaction. */
  FILE_SETKEY_(transaction_filenum, rq->transaction_id,
               sizeof(rq->transaction_id), 0, SETKEY_EXACT);
  rc = FILE_READUPDATELOCK64_(transaction_filenum,
                              (char _ptr64*)&paymentTransaction,
                              sizeof(paymentTransaction));

  if (rc != 0) {
    if (rc == 1) {
      reply_with_error(RP_CODE_NOT_FOUND, RP_CODE_NOT_FOUND,
                       "The transaction was not found.");
    } else {
      reply_with_error(RP_CODE_INTERNAL_ERROR, ERROR_CODE_IO_ERROR,
                       "File system error %d occured while attempting to "
                       "locate the transaction.",
                       (int)rc);
    }
    return;
  }

  /* Update the card balance. */
  FILE_SETKEY_(card_filenum, paymentTransaction.payment_detail.card_number,
               sizeof(paymentTransaction.payment_detail.card_number), 0,
               SETKEY_EXACT);
  rc = FILE_READUPDATELOCK64_(card_filenum, (char _ptr64*)&card, sizeof(card));

  if (rc != 0) {
    if (rc == 1) {
      reply_with_error(RP_CODE_NOT_FOUND, RP_CODE_NOT_FOUND,
                       "The payment card was not found.",
                       sizeof(rq->transaction_id), rq->transaction_id);
    } else {
      reply_with_error(RP_CODE_INTERNAL_ERROR, ERROR_CODE_IO_ERROR,
                       "File system error %d occured while attempting to "
                       "locate the payment card.",
                       (int)rc);
    }
    return;
  }

  card.card_detail.balance += paymentTransaction.payment_detail.amount;
  rc = FILE_WRITEUPDATE64_(card_filenum, (char _ptr64*)&card, sizeof(card));

  if (rc != 0) {
    reply_with_error(RP_CODE_INTERNAL_ERROR, ERROR_CODE_IO_ERROR,
                     "File system error %d occured while attempting to "
                     "update the card balance.",
                     (int)rc);
    return;
  }

  /* Create the void transaction and write it. */
  memset(&voidTransaction, 0, sizeof(voidTransaction));
  memcpy(voidTransaction.transaction_id, create_transaction_id(),
         sizeof(voidTransaction.transaction_id));
  memcpy(voidTransaction.related_transaction_id,
         paymentTransaction.transaction_id,
         sizeof(voidTransaction.related_transaction_id));
  voidTransaction.transaction_type = TRANSACTION_TYPE_VOID;
  voidTransaction.timestamp = JULIANTIMESTAMP();
  memcpy(voidTransaction.payment_response_code, RESPONSE_CODE_APPROVED,
         sizeof(voidTransaction.payment_response_code));
  memcpy(&voidTransaction.payment_detail, &paymentTransaction.payment_detail,
         sizeof(voidTransaction.payment_detail));

  rc = FILE_WRITE64_(transaction_filenum, (char _ptr64*)&voidTransaction,
                     sizeof(voidTransaction));

  if (rc != 0) {
    reply_with_error(RP_CODE_INTERNAL_ERROR, ERROR_CODE_IO_ERROR,
                     "File system error %d occured while attempting to write "
                     "the void transaction record.",
                     (int)rc);
    return;
  }

  /* Update the sale transaction with the void transaction id. */
  memcpy(paymentTransaction.related_transaction_id,
         voidTransaction.transaction_id,
         sizeof(paymentTransaction.transaction_id));
  rc = FILE_WRITEUPDATE64_(transaction_filenum,
                           (char _ptr64*)&paymentTransaction,
                           sizeof(paymentTransaction));

  if (rc != 0) {
    reply_with_error(RP_CODE_INTERNAL_ERROR, ERROR_CODE_IO_ERROR,
                     "File system error %d occured while attempting to update "
                     "that payment transaction record.",
                     (int)rc);
    return;
  }

  /* Initialize and send the reply. */
  memset(&rp, 0, sizeof(rp));
  memcpy(&rp.transaction_id, voidTransaction.transaction_id,
         sizeof(rp.transaction_id));
  REPLYX((const char*)&rp, sizeof(rp));
  RESUMETRANSACTION(0);

  /**
   * If the alert limit is met, then send an alert. Note that the alert is not
   * part of the transaction and we don't care if it fails.
   */
  if (paymentTransaction.payment_detail.amount >=
      card.card_detail.alert_limit) {
    alert_account_rq_def alert_rq;
    alert_account_rp_def alert_rp;

    memset(&rq, 0, sizeof(rq));
    alert_rq.rq_code = RQ_CODE_ALERT_ACCOUNT;
    memcpy(alert_rq.account_number, card.card_detail.account_number,
           sizeof(alert_rq.account_number));

    /* Build and send the alert message. */
    snprintf(alert_rq.alert_message, sizeof(alert_rq.alert_message),
             "ACME Card account %-.4s:  A refund in the amount of $%s from "
             "%-.*s has been credited to your account.",
             &card.card_number[12],
             format_numeric(paymentTransaction.payment_detail.amount, 2),
             sizeof(paymentTransaction.payment_detail.merchant_name),
             paymentTransaction.payment_detail.merchant_name);

    memcpy(alert_rq.card_number, card.card_number,
           sizeof(alert_rq.card_number));

    memcpy(alert_rq.name_on_card, card.card_detail.name_on_card,
           sizeof(alert_rq.name_on_card));

    alert_rq.transaction_amount = paymentTransaction.payment_detail.amount;
    memcpy(alert_rq.merchant_name,
           paymentTransaction.payment_detail.merchant_name,
           sizeof(alert_rq.merchant_name));
    alert_rq.transaction_type = voidTransaction.transaction_type;
    memcpy(alert_rq.transaction_id, paymentTransaction.transaction_id,
           sizeof(alert_rq.transaction_id));

    SERVERCLASS_SENDL_((char*)pathmon_name, (short)strlen(pathmon_name),
                       (char*)acct_serverclass, (short)strlen(acct_serverclass),
                       (char*)&alert_rq, (char*)&alert_rp, sizeof(alert_rq),
                       sizeof(alert_rp));
  }
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

  /* Open the transaction file. */
  if ((rc = FILE_OPEN_(TRANSACTION_FILENAME, sizeof(TRANSACTION_FILENAME) - 1,
                       &transaction_filenum, 0, 0, 0, 1)) != 0) {
    printf("Unable to open card file, file system error %d\n", (int)rc);
    return -1;
  }

  /* Open the card file. */
  if ((rc = FILE_OPEN_(CARD_FILENAME, sizeof(CARD_FILENAME) - 1, &card_filenum,
                       0, 0, 0, 1)) != 0) {
    printf("Unable to open card file, file system error %d\n", (int)rc);
    return -1;
  }

  /* Check for reset request. */
  if (argc == 2 && strcasecmp(argv[1], "--reset-database") == 0) {
    reset_database();
    return 0;
  }

  /* Get our pathmon name. */
  if ((p = getenv("PATHMON-NAME")) == NULL) {
    printf("PARAM PATHMON^NAME is not set.\n");
    return -1;
  }
  strncpy(pathmon_name, p, sizeof(pathmon_name));

  /* Open $RECEIVE */
  if ((rc = FILE_OPEN_("$RECEIVE", 8, &filenum, , , 0, 1)) != 0) {
    printf("Unable to open $RECEIVE, file system error %d\n", (int)rc);
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
    case RQ_CODE_CREATE_PAYMENT:
      create_payment(request);
      break;
    case RQ_CODE_VOID_PAYMENT:
      void_payment(request);
      break;
    case RQ_CODE_GET_TRANSACTION:
      get_transaction(request);
      break;
    case RQ_CODE_GET_TRANSACTIONS:
      get_transactions(request);
      break;
    default:
      REPLYX(, , , , 2);
      break;
    }
  }

  return 0;
}

/** End of file */