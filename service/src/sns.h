#include "lw.h"
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
