/* SCHEMA PRODUCED DATE - TIME : 9/11/2024 - 10:17:00 */
#pragma section setkey_approx
/* Constant SETKEY-APPROX created on 09/11/2024 at 10:17 */
#define SETKEY_APPROX 0
#pragma section setkey_generic
/* Constant SETKEY-GENERIC created on 09/11/2024 at 10:17 */
#define SETKEY_GENERIC 1
#pragma section setkey_exact
/* Constant SETKEY-EXACT created on 09/11/2024 at 10:17 */
#define SETKEY_EXACT 2
#pragma section rq_code
/* Definition RQ-CODE created on 09/11/2024 at 10:17 */
typedef short                           rq_code_def;
#pragma section rp_code
/* Definition RP-CODE created on 09/11/2024 at 10:17 */
typedef short                           rp_code_def;
#pragma section error_code
/* Definition ERROR-CODE created on 09/11/2024 at 10:17 */
typedef short                           error_code_def;
#pragma section account_number
/* Definition ACCOUNT-NUMBER created on 09/11/2024 at 10:17 */
typedef char                            account_number_def[4];
#pragma section card_number
/* Definition CARD-NUMBER created on 09/11/2024 at 10:17 */
typedef char                            card_number_def[16];
#pragma section item_count
/* Definition ITEM-COUNT created on 09/11/2024 at 10:17 */
typedef short                           item_count_def;
#pragma section transaction_id
/* Definition TRANSACTION-ID created on 09/11/2024 at 10:17 */
typedef char                            transaction_id_def[20];
#pragma section timestamp
/* Definition TIMESTAMP created on 09/11/2024 at 10:17 */
typedef long long                       timestamp_def;
#pragma section merchant_name
/* Definition MERCHANT-NAME created on 09/11/2024 at 10:17 */
typedef char                            merchant_name_def[20];
#pragma section rp_code_success
/* Constant RP-CODE-SUCCESS created on 09/11/2024 at 10:17 */
#define RP_CODE_SUCCESS 0
#pragma section rp_code_not_found
/* Constant RP-CODE-NOT-FOUND created on 09/11/2024 at 10:17 */
#define RP_CODE_NOT_FOUND 1
#pragma section rp_code_bad_request
/* Constant RP-CODE-BAD-REQUEST created on 09/11/2024 at 10:17 */
#define RP_CODE_BAD_REQUEST 400
#pragma section rp_code_internal_error
/* Constant RP-CODE-INTERNAL-ERROR created on 09/11/2024 at 10:17 */
#define RP_CODE_INTERNAL_ERROR 500
#pragma section transaction_type_sale
/* Constant TRANSACTION-TYPE-SALE created on 09/11/2024 at 10:17 */
#define TRANSACTION_TYPE_SALE 1
#pragma section transaction_type_void
/* Constant TRANSACTION-TYPE-VOID created on 09/11/2024 at 10:17 */
#define TRANSACTION_TYPE_VOID 2
#pragma section transaction_type
/* Definition TRANSACTION-TYPE created on 09/11/2024 at 10:17 */
enum
{
   transaction_type_sale = 1,
   transaction_type_void = 2
};
typedef short                           transaction_type_def;
#pragma section payment_response_code
/* Definition PAYMENT-RESPONSE-CODE created on 09/11/2024 at 10:17 */
typedef char                            payment_response_code_def[2];
#pragma section response_code_approved
/* Constant RESPONSE-CODE-APPROVED created on 09/11/2024 at 10:17 */
#define RESPONSE_CODE_APPROVED "00"
#pragma section response_code_inv_card
/* Constant RESPONSE-CODE-INV-CARD created on 09/11/2024 at 10:17 */
#define RESPONSE_CODE_INV_CARD "14"
#pragma section response_code_ins_funds
/* Constant RESPONSE-CODE-INS-FUNDS created on 09/11/2024 at 10:17 */
#define RESPONSE_CODE_INS_FUNDS "51"
#pragma section response_code_expired
/* Constant RESPONSE-CODE-EXPIRED created on 09/11/2024 at 10:17 */
#define RESPONSE_CODE_EXPIRED "54"
#pragma section response_code_inv_amount
/* Constant RESPONSE-CODE-INV-AMOUNT created on 09/11/2024 at 10:17 */
#define RESPONSE_CODE_INV_AMOUNT "67"
#pragma section response_code_inv_date
/* Constant RESPONSE-CODE-INV-DATE created on 09/11/2024 at 10:17 */
#define RESPONSE_CODE_INV_DATE "80"
#pragma section response_code_inv_cvv
/* Constant RESPONSE-CODE-INV-CVV created on 09/11/2024 at 10:17 */
#define RESPONSE_CODE_INV_CVV "82"
#pragma section response_code_restricted
/* Constant RESPONSE-CODE-RESTRICTED created on 09/11/2024 at 10:17 */
#define RESPONSE_CODE_RESTRICTED "98"
#pragma section error_code_no_transaction
/* Constant ERROR-CODE-NO-TRANSACTION created on 09/11/2024 at 10:17 */
#define ERROR_CODE_NO_TRANSACTION 1
#pragma section error_code_io_error
/* Constant ERROR-CODE-IO-ERROR created on 09/11/2024 at 10:17 */
#define ERROR_CODE_IO_ERROR 2
#pragma section error_code_not_found
/* Constant ERROR-CODE-NOT-FOUND created on 09/11/2024 at 10:17 */
#define ERROR_CODE_NOT_FOUND 3
#pragma section error_detail
/* Definition ERROR-DETAIL created on 09/11/2024 at 10:17 */
#pragma fieldalign shared2 __error_detail
typedef struct __error_detail
{
   error_code_def                  error_code;
   char                            error_message[1024];
} error_detail_def;
#define error_detail_def_Size 1026
#pragma section error_rp
/* Definition ERROR-RP created on 09/11/2024 at 10:17 */
#pragma fieldalign shared2 __error_rp
typedef struct __error_rp
{
   rp_code_def                     rp_code;
   error_detail_def                error_detail;
} error_rp_def;
#define error_rp_def_Size 1028
#pragma section rq_code_create_account
/* Constant RQ-CODE-CREATE-ACCOUNT created on 09/11/2024 at 10:17 */
#define RQ_CODE_CREATE_ACCOUNT 1
#pragma section rq_code_get_account
/* Constant RQ-CODE-GET-ACCOUNT created on 09/11/2024 at 10:17 */
#define RQ_CODE_GET_ACCOUNT 2
#pragma section rq_code_update_account
/* Constant RQ-CODE-UPDATE-ACCOUNT created on 09/11/2024 at 10:17 */
#define RQ_CODE_UPDATE_ACCOUNT 3
#pragma section rq_code_delete_account
/* Constant RQ-CODE-DELETE-ACCOUNT created on 09/11/2024 at 10:17 */
#define RQ_CODE_DELETE_ACCOUNT 4
#pragma section rq_code_get_accounts
/* Constant RQ-CODE-GET-ACCOUNTS created on 09/11/2024 at 10:17 */
#define RQ_CODE_GET_ACCOUNTS 5
#pragma section rq_code_alert_account
/* Constant RQ-CODE-ALERT-ACCOUNT created on 09/11/2024 at 10:17 */
#define RQ_CODE_ALERT_ACCOUNT 6
#pragma section rq_code_enum_account
/* Definition RQ-CODE-ENUM-ACCOUNT created on 09/11/2024 at 10:17 */
enum
{
   rq_code_create_account = 1,
   rq_code_get_account = 2,
   rq_code_update_account = 3,
   rq_code_delete_account = 4,
   rq_code_get_accounts = 5,
   rq_code_alert_account = 6
};
typedef short                           rq_code_enum_account_def;
#pragma section rp_code_enum_account
/* Definition RP-CODE-ENUM-ACCOUNT created on 09/11/2024 at 10:17 */
enum
{
   rp_code_acct_success = 0
};
typedef short                           rp_code_enum_account_def;
#pragma section account_detail
/* Definition ACCOUNT-DETAIL created on 09/11/2024 at 10:17 */
#pragma fieldalign shared2 __account_detail
typedef struct __account_detail
{
   char                            first_name[32];
   char                            last_name[32];
   char                            address1[80];
   char                            address2[80];
   char                            city[32];
   char                            state[32];
   char                            postal_code[16];
   char                            email_address[32];
   char                            phone_number[20];
} account_detail_def;
#define account_detail_def_Size 356
#pragma section account
/* Definition ACCOUNT created on 09/11/2024 at 10:17 */
#pragma fieldalign shared2 __account
typedef struct __account
{
   account_number_def              account_number;
   account_detail_def              account_detail;
} account_def;
#define account_def_Size 360
#pragma section get_accounts_rq
/* Definition GET-ACCOUNTS-RQ created on 09/11/2024 at 10:17 */
#pragma fieldalign shared2 __get_accounts_rq
typedef struct __get_accounts_rq
{
   rq_code_def                     rq_code;
   /*value is 5*/
   item_count_def                  item_count;
} get_accounts_rq_def;
#define get_accounts_rq_def_Size 4
#pragma section get_accounts_rp
/* Definition GET-ACCOUNTS-RP created on 09/11/2024 at 10:17 */
#pragma fieldalign shared2 __get_accounts_rp
typedef struct __get_accounts_rp
{
   rp_code_def                     rp_code;
   item_count_def                  item_count;
   account_def                     account[50];
} get_accounts_rp_def;
#define get_accounts_rp_def_Size 18004
#pragma section create_account_rq
/* Definition CREATE-ACCOUNT-RQ created on 09/11/2024 at 10:17 */
#pragma fieldalign shared2 __create_account_rq
typedef struct __create_account_rq
{
   rq_code_def                     rq_code;
   /*value is 1*/
   account_detail_def              account_detail;
} create_account_rq_def;
#define create_account_rq_def_Size 358
#pragma section create_account_rp
/* Definition CREATE-ACCOUNT-RP created on 09/11/2024 at 10:17 */
#pragma fieldalign shared2 __create_account_rp
typedef struct __create_account_rp
{
   rp_code_enum_account_def        rp_code;
   account_def                     account;
} create_account_rp_def;
#define create_account_rp_def_Size 362
#pragma section get_account_rq
/* Definition GET-ACCOUNT-RQ created on 09/11/2024 at 10:17 */
#pragma fieldalign shared2 __get_account_rq
typedef struct __get_account_rq
{
   rq_code_def                     rq_code;
   /*value is 2*/
   account_number_def              account_number;
} get_account_rq_def;
#define get_account_rq_def_Size 6
#pragma section get_account_rp
/* Definition GET-ACCOUNT-RP created on 09/11/2024 at 10:17 */
#pragma fieldalign shared2 __get_account_rp
typedef struct __get_account_rp
{
   rp_code_enum_account_def        rp_code;
   account_def                     account;
} get_account_rp_def;
#define get_account_rp_def_Size 362
#pragma section update_account_rq
/* Definition UPDATE-ACCOUNT-RQ created on 09/11/2024 at 10:17 */
#pragma fieldalign shared2 __update_account_rq
typedef struct __update_account_rq
{
   rq_code_def                     rq_code;
   /*value is 3*/
   account_number_def              account_number;
   account_detail_def              account_detail;
} update_account_rq_def;
#define update_account_rq_def_Size 362
#pragma section update_account_rp
/* Definition UPDATE-ACCOUNT-RP created on 09/11/2024 at 10:17 */
#pragma fieldalign shared2 __update_account_rp
typedef struct __update_account_rp
{
   rp_code_enum_account_def        rp_code;
   account_def                     account;
} update_account_rp_def;
#define update_account_rp_def_Size 362
#pragma section delete_account_rq
/* Definition DELETE-ACCOUNT-RQ created on 09/11/2024 at 10:17 */
#pragma fieldalign shared2 __delete_account_rq
typedef struct __delete_account_rq
{
   rq_code_def                     rq_code;
   /*value is 4*/
   account_number_def              account_number;
} delete_account_rq_def;
#define delete_account_rq_def_Size 6
#pragma section delete_account_rp
/* Definition DELETE-ACCOUNT-RP created on 09/11/2024 at 10:17 */
#pragma fieldalign shared2 __delete_account_rp
typedef struct __delete_account_rp
{
   rp_code_enum_account_def        rp_code;
} delete_account_rp_def;
#define delete_account_rp_def_Size 2
#pragma section name_on_card
/* Definition NAME-ON-CARD created on 09/11/2024 at 10:17 */
typedef char name_on_card_def[64];
#pragma section alert_account_rq
/* Definition ALERT-ACCOUNT-RQ created on 09/11/2024 at 10:17 */
#include <tnsint.h>
#pragma fieldalign shared2 __alert_account_rq
typedef struct __alert_account_rq
{
   rq_code_def                     rq_code;
   /*value is 6*/
   account_number_def              account_number;
   char                            alert_message[1024];
   __int32_t transaction_amount;
   transaction_type_def transaction_type;
   merchant_name_def merchant_name;
   card_number_def card_number;
   name_on_card_def name_on_card;
   transaction_id_def transaction_id;
} alert_account_rq_def;
#define alert_account_rq_def_Size 1156
#pragma section alert_account_rp
/* Definition ALERT-ACCOUNT-RP created on 09/11/2024 at 10:17 */
#pragma fieldalign shared2 __alert_account_rp
typedef struct __alert_account_rp
{
   rp_code_enum_account_def        rp_code;
} alert_account_rp_def;
#define alert_account_rp_def_Size 2
#pragma section account_filename
/* Constant ACCOUNT-FILENAME created on 09/11/2024 at 10:17 */
#define ACCOUNT_FILENAME "ACCTFILE"
#pragma section account_record
/* Record ACCOUNT-RECORD created on 09/11/2024 at 10:17 */
typedef account_def                     account_record_def;
#pragma section rq_code_create_card
/* Constant RQ-CODE-CREATE-CARD created on 09/11/2024 at 10:17 */
#define RQ_CODE_CREATE_CARD 1
#pragma section rq_code_get_card
/* Constant RQ-CODE-GET-CARD created on 09/11/2024 at 10:17 */
#define RQ_CODE_GET_CARD 2
#pragma section rq_code_update_card
/* Constant RQ-CODE-UPDATE-CARD created on 09/11/2024 at 10:17 */
#define RQ_CODE_UPDATE_CARD 3
#pragma section rq_code_delete_card
/* Constant RQ-CODE-DELETE-CARD created on 09/11/2024 at 10:17 */
#define RQ_CODE_DELETE_CARD 4
#pragma section rq_code_get_cards
/* Constant RQ-CODE-GET-CARDS created on 09/11/2024 at 10:17 */
#define RQ_CODE_GET_CARDS 5
#pragma section rq_code_enum_card
/* Definition RQ-CODE-ENUM-CARD created on 09/11/2024 at 10:17 */
enum
{
   rq_code_create_card = 1,
   rq_code_get_card = 2,
   rq_code_update_card = 3,
   rq_code_delete_card = 4,
   rq_code_get_cards = 5
};
typedef short                           rq_code_enum_card_def;
#pragma section rp_code_enum_card
/* Definition RP-CODE-ENUM-CARD created on 09/11/2024 at 10:17 */
enum
{
   rp_code_card_success = 0
};
typedef short rp_code_enum_card_def;
#pragma section card_detail
/* Definition CARD-DETAIL created on 09/11/2024 at 10:17 */
#include <tnsint.h>
#pragma fieldalign shared2 __card_detail
typedef struct __card_detail
{
   account_number_def              account_number;
   name_on_card_def                name_on_card;
   char                            exp_month[2];
   char                            exp_year[4];
   char                            security_code[4];
/* @LightWaveAttribute(scale="2") */
   __int32_t                       balance;
/* @LightWaveAttribute(scale="2") */
   __int32_t                       spending_limit;
/* @LightWaveAttribute(scale="2") */
   __int32_t                       alert_limit;
/* @LightWaveAttribute(type="boolean") */
   short                           is_locked;
} card_detail_def;
#define card_detail_def_Size 92
#pragma section card
/* Definition CARD created on 09/11/2024 at 10:17 */
#pragma fieldalign shared2 __card
typedef struct __card
{
   card_number_def                 card_number;
   card_detail_def                 card_detail;
} card_def;
#define card_def_Size 108
#pragma section get_cards_rq
/* Definition GET-CARDS-RQ created on 09/11/2024 at 10:17 */
#pragma fieldalign shared2 __get_cards_rq
typedef struct __get_cards_rq
{
   rq_code_def                     rq_code;
   /*value is 5*/
   account_number_def              account_number;
   item_count_def                  item_count;
} get_cards_rq_def;
#define get_cards_rq_def_Size 8
#pragma section get_cards_rp
/* Definition GET-CARDS-RP created on 09/11/2024 at 10:17 */
#pragma fieldalign shared2 __get_cards_rp
typedef struct __get_cards_rp
{
   rp_code_def                     rp_code;
   item_count_def                  item_count;
   card_def                        card[50];
} get_cards_rp_def;
#define get_cards_rp_def_Size 5404
#pragma section create_card_rq
/* Definition CREATE-CARD-RQ created on 09/11/2024 at 10:17 */
#pragma fieldalign shared2 __create_card_rq
typedef struct __create_card_rq
{
   rq_code_def                     rq_code;
   /*value is 1*/
   card_detail_def                 card_detail;
} create_card_rq_def;
#define create_card_rq_def_Size 94
#pragma section create_card_rp
/* Definition CREATE-CARD-RP created on 09/11/2024 at 10:17 */
#pragma fieldalign shared2 __create_card_rp
typedef struct __create_card_rp
{
   rp_code_enum_card_def           rp_code;
   card_def                        card;
} create_card_rp_def;
#define create_card_rp_def_Size 110
#pragma section get_card_rq
/* Definition GET-CARD-RQ created on 09/11/2024 at 10:17 */
#pragma fieldalign shared2 __get_card_rq
typedef struct __get_card_rq
{
   rq_code_def                     rq_code;
   /*value is 2*/
   card_number_def                 card_number;
} get_card_rq_def;
#define get_card_rq_def_Size 18
#pragma section get_card_rp
/* Definition GET-CARD-RP created on 09/11/2024 at 10:17 */
#pragma fieldalign shared2 __get_card_rp
typedef struct __get_card_rp
{
   rp_code_enum_card_def           rp_code;
   card_def                        card;
} get_card_rp_def;
#define get_card_rp_def_Size 110
#pragma section update_card_rq
/* Definition UPDATE-CARD-RQ created on 09/11/2024 at 10:17 */
#pragma fieldalign shared2 __update_card_rq
typedef struct __update_card_rq
{
   rq_code_def                     rq_code;
   /*value is 3*/
   card_number_def                 card_number;
   card_detail_def                 card_detail;
} update_card_rq_def;
#define update_card_rq_def_Size 110
#pragma section update_card_rp
/* Definition UPDATE-CARD-RP created on 09/11/2024 at 10:17 */
#pragma fieldalign shared2 __update_card_rp
typedef struct __update_card_rp
{
   rp_code_enum_card_def           rp_code;
   card_def                        card;
} update_card_rp_def;
#define update_card_rp_def_Size 110
#pragma section delete_card_rq
/* Definition DELETE-CARD-RQ created on 09/11/2024 at 10:17 */
#pragma fieldalign shared2 __delete_card_rq
typedef struct __delete_card_rq
{
   rq_code_def                     rq_code;
   /*value is 4*/
   card_number_def                 card_number;
} delete_card_rq_def;
#define delete_card_rq_def_Size 18
#pragma section delete_card_rp
/* Definition DELETE-CARD-RP created on 09/11/2024 at 10:17 */
#pragma fieldalign shared2 __delete_card_rp
typedef struct __delete_card_rp
{
   rp_code_enum_card_def           rp_code;
} delete_card_rp_def;
#define delete_card_rp_def_Size 2
#pragma section card_filename
/* Constant CARD-FILENAME created on 09/11/2024 at 10:17 */
#define CARD_FILENAME "CARDFILE"
#pragma section card_altkey_account_number
/* Constant CARD-ALTKEY-ACCOUNT-NUMBER created on 09/11/2024 at 10:17 */
#define CARD_ALTKEY_ACCOUNT_NUMBER 16718
#pragma section card_record
/* Record CARD-RECORD created on 09/11/2024 at 10:17 */
typedef card_def                        card_record_def;
#pragma section rq_code_create_payment
/* Constant RQ-CODE-CREATE-PAYMENT created on 09/11/2024 at 10:17 */
#define RQ_CODE_CREATE_PAYMENT 1
#pragma section rq_code_void_payment
/* Constant RQ-CODE-VOID-PAYMENT created on 09/11/2024 at 10:17 */
#define RQ_CODE_VOID_PAYMENT 2
#pragma section rq_code_get_transaction
/* Constant RQ-CODE-GET-TRANSACTION created on 09/11/2024 at 10:17 */
#define RQ_CODE_GET_TRANSACTION 3
#pragma section rq_code_get_transactions
/* Constant RQ-CODE-GET-TRANSACTIONS created on 09/11/2024 at 10:17 */
#define RQ_CODE_GET_TRANSACTIONS 4
#pragma section rq_code_enum_payment
/* Definition RQ-CODE-ENUM-PAYMENT created on 09/11/2024 at 10:17 */
enum
{
   rq_code_create_payment = 1,
   rq_code_void_payment = 2,
   rq_code_get_transaction = 3,
   rq_code_get_transactions = 4
};
typedef short                           rq_code_enum_payment_def;
#pragma section rp_code_enum_payment
/* Definition RP-CODE-ENUM-PAYMENT created on 09/11/2024 at 10:17 */
enum
{
   rp_code_payment_success = 0
};
typedef short                           rp_code_enum_payment_def;
#pragma section payment_detail
/* Definition PAYMENT-DETAIL created on 09/11/2024 at 10:17 */
#include <tnsint.h>
#pragma fieldalign shared2 __payment_detail
typedef struct __payment_detail
{
/* @LightWaveAttribute(scale="2") */
   __int32_t                       amount;
   merchant_name_def               merchant_name;
   card_number_def                 card_number;
   name_on_card_def                name_on_card;
   char                            exp_month[2];
   char                            exp_year[4];
   char                            security_code[4];
} payment_detail_def;
#define payment_detail_def_Size 114
#pragma section transaction
/* Definition TRANSACTION created on 09/11/2024 at 10:17 */
#pragma fieldalign shared2 __transaction
typedef struct __transaction
{
   transaction_id_def              transaction_id;
   transaction_id_def              related_transaction_id;
   payment_response_code_def       payment_response_code;
   transaction_type_def            transaction_type;
/* @LightWaveAttribute(type="timestamp") */
   long long                       timestamp;
   payment_detail_def              payment_detail;
} transaction_def;
#define transaction_def_Size 166
#pragma section create_payment_rq
/* Definition CREATE-PAYMENT-RQ created on 09/11/2024 at 10:17 */
#pragma fieldalign shared2 __create_payment_rq
typedef struct __create_payment_rq
{
   rq_code_def                     rq_code;
   /*value is 1*/
   payment_detail_def              payment_detail;
} create_payment_rq_def;
#define create_payment_rq_def_Size 116
#pragma section create_payment_rp
/* Definition CREATE-PAYMENT-RP created on 09/11/2024 at 10:17 */
#pragma fieldalign shared2 __create_payment_rp
typedef struct __create_payment_rp
{
   rp_code_def                     rp_code;
   transaction_def                 transaction;
} create_payment_rp_def;
#define create_payment_rp_def_Size 168
#pragma section void_payment_rq
/* Definition VOID-PAYMENT-RQ created on 09/11/2024 at 10:17 */
#pragma fieldalign shared2 __void_payment_rq
typedef struct __void_payment_rq
{
   rq_code_def                     rq_code;
   /*value is 2*/
   transaction_id_def              transaction_id;
} void_payment_rq_def;
#define void_payment_rq_def_Size 22
#pragma section void_payment_rp
/* Definition VOID-PAYMENT-RP created on 09/11/2024 at 10:17 */
#pragma fieldalign shared2 __void_payment_rp
typedef struct __void_payment_rp
{
   rp_code_def                     rp_code;
   transaction_id_def              transaction_id;
} void_payment_rp_def;
#define void_payment_rp_def_Size 22
#pragma section get_transaction_rq
/* Definition GET-TRANSACTION-RQ created on 09/11/2024 at 10:17 */
#pragma fieldalign shared2 __get_transaction_rq
typedef struct __get_transaction_rq
{
   rq_code_def                     rq_code;
   /*value is 3*/
   transaction_id_def              transaction_id;
} get_transaction_rq_def;
#define get_transaction_rq_def_Size 22
#pragma section get_transaction_rp
/* Definition GET-TRANSACTION-RP created on 09/11/2024 at 10:17 */
#pragma fieldalign shared2 __get_transaction_rp
typedef struct __get_transaction_rp
{
   rp_code_def                     rp_code;
   transaction_def                 transaction;
} get_transaction_rp_def;
#define get_transaction_rp_def_Size 168
#pragma section get_transactions_rq
/* Definition GET-TRANSACTIONS-RQ created on 09/11/2024 at 10:17 */
#pragma fieldalign shared2 __get_transactions_rq
typedef struct __get_transactions_rq
{
   rq_code_def                     rq_code;
   /*value is 4*/
   item_count_def                  item_count;
} get_transactions_rq_def;
#define get_transactions_rq_def_Size 4
#pragma section get_transactions_rp
/* Definition GET-TRANSACTIONS-RP created on 09/11/2024 at 10:17 */
#pragma fieldalign shared2 __get_transactions_rp
typedef struct __get_transactions_rp
{
   rp_code_def                     rp_code;
   item_count_def                  item_count;
   transaction_def                 transaction[50];
} get_transactions_rp_def;
#define get_transactions_rp_def_Size 8304
#pragma section transaction_filename
/* Constant TRANSACTION-FILENAME created on 09/11/2024 at 10:17 */
#define TRANSACTION_FILENAME "TRANFILE"
#pragma section transaction_record
/* Record TRANSACTION-RECORD created on 09/11/2024 at 10:17 */
typedef transaction_def transaction_record_def;