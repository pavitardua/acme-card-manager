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

#pragma list

/* Static variables. */
static short card_filenum;
static char pathmon_name[32];
static char* acct_serverclass = "ACCT-SERVER";
static int sms_enabled = 0;

/* Static function prototypes. */
static int activate_transaction(const char* type);
static void create_card(void* request);
static void delete_card(void* request);
static void get_card(void* request);
static void get_cards(void* request);
static void reply_with_error(rp_code_def rp_code, error_code_def error_code,
                             const char* error_format, ...);
static void update_card(void* request);

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

static void create_card(void* request) {
  create_card_rq_def* rq = (create_card_rq_def*)request;
  card_def card;
  card_number_def card_number;
  create_card_rp_def rp;
  short rc;

  /* Activate a transaction. */
  if (activate_transaction("create") != 0) {
    return;
  }

  /* Determine the next card number by reading the last record and adding 1.
   */
  FILE_SETKEY_(card_filenum, 0, 0, 0, 0, 0xE000);
  rc = FILE_READLOCK64_(card_filenum, (char _ptr64*)&card, sizeof(card));

  if (rc == 0) {
    char tmp[sizeof(card_number) + 2];
    unsigned int next;

    memset(tmp, 0, sizeof(tmp));
    memcpy(tmp, &card.card_number[sizeof(card_number) - 4], 4);
    next = atoi(tmp);
    next += 1;
    sprintf(tmp, "%-12.12s%04u", card.card_number, next);
    memcpy(card_number, tmp, sizeof(card_number));
  } else if (rc == 1) {
    memcpy(card_number, "4111111111111111", sizeof(card_number));
  } else {
    reply_with_error(RP_CODE_INTERNAL_ERROR, ERROR_CODE_IO_ERROR,
                     "File system error %d occured while attempting to "
                     "determine the next card number.",
                     (int)rc);
    return;
  }

  /* Initialize the card record and write it. */
  memset(&card, 0, sizeof(card));
  memcpy(card.card_number, card_number, sizeof(card.card_number));
  memcpy(&card.card_detail, &rq->card_detail, sizeof(card.card_detail));

  rc = FILE_WRITE64_(card_filenum, (char _ptr64*)&card, sizeof(card));

  if (rc != 0) {
    reply_with_error(RP_CODE_INTERNAL_ERROR, ERROR_CODE_IO_ERROR,
                     "File system error %d occured while attempting to "
                     "write the card record.",
                     (int)rc);
    return;
  }

  /* Initialize and send the reply. */
  memset(&rp, 0, sizeof(rp));
  memcpy(&rp.card, &card, sizeof(rp.card));
  REPLYX((const char*)&rp, sizeof(rp));
  RESUMETRANSACTION(0);
}

static void delete_card(void* request) {
  delete_card_rq_def* rq = (delete_card_rq_def*)request;
  card_def card;
  delete_card_rp_def rp;
  short rc;

  /* Activate a transaction. */
  if (activate_transaction("delete") != 0) {
    return;
  }

  /* Initialize reply. */
  memset(&rp, 0, sizeof(rp));

  /* Position at the card record. */
  FILE_SETKEY_(card_filenum, rq->card_number, sizeof(rq->card_number), 0,
               SETKEY_EXACT);
  rc = FILE_READUPDATELOCK64_(card_filenum, (char _ptr64*)&card, sizeof(card));

  if (rc != 0) {
    if (rc == 11) {
      reply_with_error(RP_CODE_NOT_FOUND, RP_CODE_NOT_FOUND,
                       "The card was not found.");
    } else {
      reply_with_error(RP_CODE_INTERNAL_ERROR, ERROR_CODE_IO_ERROR,
                       "File system error %d occured while attempting to "
                       "locate the card record for deletion.",
                       (int)rc);
    }
    return;
  }

  rc = FILE_WRITEUPDATE64_(card_filenum, 0, 0);

  if (rc != 0) {
    reply_with_error(RP_CODE_INTERNAL_ERROR, ERROR_CODE_IO_ERROR,
                     "File system error %d occured while attempting to "
                     "delete the card record.",
                     (int)rc);
    return;
  }

  /* Send the reply. */
  memset(&rp, 0, sizeof(rp));
  REPLYX((const char*)&rp, sizeof(rp));
  RESUMETRANSACTION(0);
}

static void get_card(void* request) {
  get_card_rq_def* rq = (get_card_rq_def*)request;
  card_def card;
  get_card_rp_def rp;
  short rc;

  /* Initialize reply. */
  memset(&rp, 0, sizeof(rp));

  /* Position at the card record. */
  FILE_SETKEY_(card_filenum, rq->card_number, sizeof(rq->card_number), 0,
               SETKEY_EXACT);
  rc = FILE_READ64_(card_filenum, (char _ptr64*)&card, sizeof(card));

  if (rc != 0) {
    if (rc == 1) {
      reply_with_error(RP_CODE_NOT_FOUND, RP_CODE_NOT_FOUND,
                       "The card was not found.");
    } else {
      reply_with_error(RP_CODE_INTERNAL_ERROR, ERROR_CODE_IO_ERROR,
                       "File system error %d occured while attempting to "
                       "locate the card record.",
                       (int)rc);
    }
    return;
  }

  memcpy(&rp.card, &card, sizeof(rp.card));

  /* Send the reply. */
  REPLYX((const char*)&rp, sizeof(rp));
}

static void get_cards(void* request) {
  get_cards_rq_def* rq = (get_cards_rq_def*)request;
  card_def card;
  get_cards_rp_def rp;
  size_t max;

  /* Initialize the reply. */
  memset(&rp, 0, sizeof(rp));

  /* Determine max records. */
  max = sizeof(rp.card) / sizeof(card_def);

  /* If there's no account number then position at start of file, otherwise,
  position at the account number. */
  if (*rq->account_number == 0) {
    FILE_SETKEY_(card_filenum, 0, 0);
  } else {
    FILE_SETKEY_(card_filenum, rq->account_number, sizeof(rq->account_number),
                 (short)CARD_ALTKEY_ACCOUNT_NUMBER, SETKEY_EXACT);
  }

  while (rp.item_count < max) {

    if (FILE_READ64_(card_filenum, (char _ptr64*)&card, sizeof(card_def)) !=
        0) {
      break;
    }

    memcpy(&rp.card[rp.item_count], &card, sizeof(card_def));
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

static void update_card(void* request) {
  update_card_rq_def* rq = (update_card_rq_def*)request;
  card_def card;
  update_card_rp_def rp;
  short rc;
  int wasLocked;

  /* Activate a transaction. */
  if (activate_transaction("update") != 0) {
    return;
  }

  /* Initialize reply. */
  memset(&rp, 0, sizeof(rp));

  /* Position at the card record. */
  FILE_SETKEY_(card_filenum, rq->card_number, sizeof(rq->card_number), 0,
               SETKEY_EXACT);
  rc = FILE_READUPDATELOCK64_(card_filenum, (char _ptr64*)&card, sizeof(card));

  if (rc != 0) {
    if (rc == 11) {
      reply_with_error(RP_CODE_NOT_FOUND, RP_CODE_NOT_FOUND,
                       "The card was not found.");
    } else {
      reply_with_error(RP_CODE_INTERNAL_ERROR, ERROR_CODE_IO_ERROR,
                       "File system error %d occured while attempting to "
                       "locate the card record for update.",
                       (int)rc);
    }
    return;
  }

  wasLocked = card.card_detail.is_locked;
  memcpy(&card.card_detail, &rq->card_detail, sizeof(card.card_detail));
  rc = FILE_WRITEUPDATE64_(card_filenum, (char _ptr64*)&card, sizeof(card));

  if (rc != 0) {
    reply_with_error(RP_CODE_INTERNAL_ERROR, ERROR_CODE_IO_ERROR,
                     "File system error %d occured while attempting to "
                     "update the card record.",
                     (int)rc);
    return;
  }

  /* Send the reply. */
  memcpy(&rp.card, &card, sizeof(rp.card));
  REPLYX((const char*)&rp, sizeof(rp));
  RESUMETRANSACTION(0);

  /**
   * If the lock state has changed send an alert. Note that the alert is not part of the transaction
   * and we don't care if it fails.
   */
  if (sms_enabled != 0 && wasLocked != card.card_detail.is_locked) {
    alert_account_rq_def alert_rq;
    alert_account_rp_def alert_rp;

    memset(&rq, 0, sizeof(rq));
    alert_rq.rq_code = RQ_CODE_ALERT_ACCOUNT;
    memcpy(alert_rq.account_number, card.card_detail.account_number,
           sizeof(alert_rq.account_number));

    /* Build and send the alert message. */
    snprintf(alert_rq.alert_message, sizeof(alert_rq.alert_message),
             "Your ACME Card account %-.4s has been %s.", &card.card_number[12],
             (card.card_detail.is_locked ? "blocked" : "un-blocked"));
    strcpy(alert_rq.name_on_card, card.card_detail.name_on_card);
    strcpy(alert_rq.transaction_id, "");
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

  /* Get our pathmon name. */
  if ((p = getenv("PATHMON-NAME")) == NULL) {
    printf("PARAM PATHMON^NAME is not set.\n");
    return -1;
  }
  strncpy(pathmon_name, p, sizeof(pathmon_name));

  /* Check for SMS enabled. Default to no. */
  if ((p = getenv("ENABLE-SMS")) == NULL) {
    sms_enabled = 0;
  } else {
    sms_enabled = atoi(p);
  }

  /* Open $RECEIVE */
  if ((rc = FILE_OPEN_("$RECEIVE", 8, &filenum, , , 0, 1)) != 0) {
    printf("Unable to open $RECEIVE, file system error %d\n", (int)rc);
    return -1;
  }

  /* Open the card file. */
  if ((rc = FILE_OPEN_(CARD_FILENAME, sizeof(CARD_FILENAME) - 1, &card_filenum,
                       0, 0, 0, 1)) != 0) {
    printf("Unable to open card file, file system error %d\n", (int)rc);
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
    case RQ_CODE_CREATE_CARD:
      create_card(request);
      break;
    case RQ_CODE_DELETE_CARD:
      delete_card(request);
      break;
    case RQ_CODE_GET_CARD:
      get_card(request);
      break;
    case RQ_CODE_GET_CARDS:
      get_cards(request);
      break;
    case RQ_CODE_UPDATE_CARD:
      update_card(request);
      break;

    default:
      REPLYX(, , , , 2);
      break;
    }
  }

  return 0;
}

/** End of file */