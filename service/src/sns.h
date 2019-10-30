/* SCHEMA PRODUCED DATE - TIME :10/30/2019 - 12:43:07 */
#pragma section lightwave_rp_code_enum
/* Definition LIGHTWAVE-RP-CODE-ENUM created on 10/30/2019 at 12:43 */
enum
{
   lw_rp_success = 0,
   lw_rp_info = 1,
   lw_rp_error = 2
};
typedef short                           lightwave_rp_code_enum_def;
#pragma section lightwave_info_code_enum
/* Definition LIGHTWAVE-INFO-CODE-ENUM created on 10/30/2019 at 12:43 */
enum
{
   lw_info_field_truncated = 100,
   lw_info_array_truncated = 101
};
typedef short                           lightwave_info_code_enum_def;
#pragma section lightwave_error_src_enum
/* Definition LIGHTWAVE-ERROR-SRC-ENUM created on 10/30/2019 at 12:43 */
enum
{
   lw_error_src_lightwave = 1,
   lw_error_src_http = 2,
   lw_error_src_tcpip = 3,
   lw_error_src_ssl = 4
};
typedef short                           lightwave_error_src_enum_def;
#pragma section lightwave_error_enum
/* Definition LIGHTWAVE-ERROR-ENUM created on 10/30/2019 at 12:43 */
enum
{
   lw_error_invalid_license = 1,
   lw_error_invalid_header = 2,
   lw_error_invalid_rq_code = 3,
   lw_error_invalid_type = 4,
   lw_error_serialization_error = 5,
   lw_error_deserialization_error = 6,
   lw_error_request_timeout = 7,
   lw_error_response_not_defined = 12,
   lw_error_internal_error = 500
};
typedef short                           lightwave_error_enum_def;
#pragma section lightwave_rq_header
/* Definition LIGHTWAVE-RQ-HEADER created on 10/30/2019 at 12:43 */
#include <tnsint.h>
#pragma fieldalign shared2 __lightwave_rq_header
typedef struct __lightwave_rq_header
{
   short                           rq_code;
   unsigned short                  rq_version;
   __uint32_t                      rq_timeout;
   char                            reserved[24];
} lightwave_rq_header_def;
#define lightwave_rq_header_def_Size 32
#pragma section lightwave_rp_header
/* Definition LIGHTWAVE-RP-HEADER created on 10/30/2019 at 12:43 */
#pragma fieldalign shared2 __lightwave_rp_header
typedef struct __lightwave_rp_header
{
   short                           rp_code;
   short                           http_status;
   short                           info_code;
   short                           info_detail;
   char                            reserved[24];
} lightwave_rp_header_def;
#define lightwave_rp_header_def_Size 32
#pragma section lightwave_error_rp
/* Definition LIGHTWAVE-ERROR-RP created on 10/30/2019 at 12:43 */
#include <tnsint.h>
#pragma fieldalign shared2 __lightwave_error_rp
typedef struct __lightwave_error_rp
{
   lightwave_rp_header_def         lightwave_rp_header;
   __int32_t                       error_source;
   __int32_t                       error_code;
   __int32_t                       error_subcode;
   char                            error_message[4096];
} lightwave_error_rp_def;
#define lightwave_error_rp_def_Size 4140
#pragma section aws_simple_notificatio_val
/* Definition AWS-SIMPLE-NOTIFICATIO-VAL created on 10/30/2019 at 12:43 */
#pragma fieldalign shared2 __aws_simple_notificatio_val
typedef struct __aws_simple_notificatio_val
{
   short                           rq_publish_with_post;
   /*value is 2*/
} aws_simple_notificatio_val_def;
#define aws_simple_notificatio_val_def_Size 2
#pragma section aws_simple_notificatio_enm
/* Definition AWS-SIMPLE-NOTIFICATIO-ENM created on 10/30/2019 at 12:43 */
enum
{
   rq_publish_with_post = 2
};
typedef short                           aws_simple_notificatio_enm_def;
#pragma section error_type
/* Definition ERROR-TYPE created on 10/30/2019 at 12:43 */
#pragma fieldalign shared2 __error_type
typedef struct __error_type
{
   char                            type_rw[128];
   char                            code_rw[128];
   char                            message_rw[256];
} error_type_def;
#define error_type_def_Size 512
#pragma section error_response
/* Definition ERROR-RESPONSE created on 10/30/2019 at 12:43 */
#pragma fieldalign shared2 __error_response
typedef struct __error_response
{
   error_type_def                  error_rw;
   char                            request_id[128];
} error_response_def;
#define error_response_def_Size 640
#pragma section publish_result_type
/* Definition PUBLISH-RESULT-TYPE created on 10/30/2019 at 12:43 */
#pragma fieldalign shared2 __publish_result_type
typedef struct __publish_result_type
{
   char                            message_id[128];
} publish_result_type_def;
#define publish_result_type_def_Size 128
#pragma section response_metadata_type
/* Definition RESPONSE-METADATA-TYPE created on 10/30/2019 at 12:43 */
#pragma fieldalign shared2 __response_metadata_type
typedef struct __response_metadata_type
{
   char                            request_id[128];
} response_metadata_type_def;
#define response_metadata_type_def_Size 128
#pragma section publish_response
/* Definition PUBLISH-RESPONSE created on 10/30/2019 at 12:43 */
#pragma fieldalign shared2 __publish_response
typedef struct __publish_response
{
   publish_result_type_def         publish_result;
   response_metadata_type_def      response_metadata;
} publish_response_def;
#define publish_response_def_Size 256
#pragma section publish_with_post_rq
/* Definition PUBLISH-WITH-POST-RQ created on 10/30/2019 at 12:43 */
#pragma fieldalign shared2 __publish_with_post_rq
typedef struct __publish_with_post_rq
{
   lightwave_rq_header_def         lightwave_rq_header;
   char                            access_key_id[256];
   char                            secret_access_key[256];
   char                            session_token[512];
   char                            topic_arn[2048];
   char                            phone_number[32];
   char                            subject[100];
   char                            message_rw[4096];
} publish_with_post_rq_def;
#define publish_with_post_rq_def_Size 7332
#pragma section publish_with_post_200_rp
/* Definition PUBLISH-WITH-POST-200-RP created on 10/30/2019 at 12:43 */
#pragma fieldalign shared2 __publish_with_post_200_rp
typedef struct __publish_with_post_200_rp
{
   lightwave_rp_header_def         lightwave_rp_header;
   publish_response_def            publish_response;
} publish_with_post_200_rp_def;
#define publish_with_post_200_rp_def_Size 288
#pragma section publish_with_post_other_rp
/* Definition PUBLISH-WITH-POST-OTHER-RP created on 10/30/2019 at 12:43 */
#pragma fieldalign shared2 __publish_with_post_other_rp
typedef struct __publish_with_post_other_rp
{
   lightwave_rp_header_def         lightwave_rp_header;
   error_response_def              error_response;
} publish_with_post_other_rp_def;
#define publish_with_post_other_rp_def_Size 672
