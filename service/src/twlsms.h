#include "lw.h"
#pragma section twilio_messaging_send_val
/* Definition TWILIO-MESSAGING-SEND-VAL created on 08/30/2024 at 11:29 */
#pragma fieldalign shared2 __twilio_messaging_send_val
typedef struct __twilio_messaging_send_val {
  short rq_send_sms;
  /*value is 1*/
} twilio_messaging_send_val_def;
#define twilio_messaging_send_val_def_Size 2
#pragma section twilio_messaging_send_enm
/* Definition TWILIO-MESSAGING-SEND-ENM created on 08/30/2024 at 11:29 */
enum { rq_send_sms = 1 };
typedef short twilio_messaging_send_enm_def;
#pragma section subresource_uris_type
/* Definition SUBRESOURCE-URIS-TYPE created on 08/30/2024 at 11:29 */
#pragma fieldalign shared2 __subresource_uris_type
typedef struct __subresource_uris_type {
  short additional_properties_count;
  struct {
    char map_key[16];
    char map_value[16];
  } additional_properties[4];
} subresource_uris_type_def;
#define subresource_uris_type_def_Size 130
#pragma section create_message_response
/* Definition CREATE-MESSAGE-RESPONSE created on 08/30/2024 at 11:29 */
#include <tnsint.h>
#pragma fieldalign shared2 __create_message_response
typedef struct __create_message_response {
  char account_sid[32];
  char api_version[33];
  char body[38];
  char date_created[33];
  char date_sent[32];
  char date_updated[32];
  char direction[32];
  __int32_t error_code;
  char error_message[32];
  char from_rw[32];
  char messaging_service_sid[32];
  char num_media[32];
  char num_segments[32];
  char price[32];
  char price_unit[32];
  char sid[32];
  char status_rw[32];
  subresource_uris_type_def subresource_uris;
  char to_rw[32];
  char uri[32];
} create_message_response_def;
#define create_message_response_def_Size 718
#pragma section send_smsrq
/* Definition SEND-SMSRQ created on 08/30/2024 at 11:29 */
#pragma fieldalign shared2 __send_smsrq
typedef struct __send_smsrq {
  lightwave_rq_header_def lightwave_rq_header;
  char account_sid[34];
  char body[160];
  char to_rw[32];
  char from_rw[32];
} send_smsrq_def;
#define send_smsrq_def_Size 290
#pragma section send_sms_201_rp
/* Definition SEND-SMS-201-RP created on 08/30/2024 at 11:29 */
#pragma fieldalign shared2 __send_sms_201_rp
typedef struct __send_sms_201_rp {
  lightwave_rp_header_def lightwave_rp_header;
  create_message_response_def create_message_response;
} send_sms_201_rp_def;
#define send_sms_201_rp_def_Size 750