#include "lw.h"
#pragma section send_grid_email_with_t_val
/* Definition SEND-GRID-EMAIL-WITH-T-VAL created on 09/11/2024 at 11:37 */
#pragma fieldalign shared2 __send_grid_email_with_t_val
typedef struct __send_grid_email_with_t_val {
  short rq_post_send;
  /*value is 10*/
} send_grid_email_with_t_val_def;
#define send_grid_email_with_t_val_def_Size 2
#pragma section send_grid_email_with_t_enm
/* Definition SEND-GRID-EMAIL-WITH-T-ENM created on 09/11/2024 at 11:37 */
enum { rq_post_send = 10 };
typedef short send_grid_email_with_t_enm_def;
#pragma section dynamic_template_data_01
/* Definition DYNAMIC-TEMPLATE-DATA-01 created on 09/11/2024 at 11:37 */
#include <tnsint.h>
#pragma fieldalign shared2 __dynamic_template_data_01
typedef struct __dynamic_template_data_01 {
  char user[200];
  char vendor_name[200];
  __int32_t transaction_amount;
  char transaction_date[32];
  char transaction_type[32];
  char cc_last_4[16];
  char transaction_id[32];
  char alert_message[1024];
} dynamic_template_data_01_def;
#define dynamic_template_data_01_def_Size 1540
#pragma section to_type_01
/* Definition TO-TYPE-01 created on 09/11/2024 at 11:37 */
#pragma fieldalign shared2 __to_type_01
typedef struct __to_type_01 {
  char email[32];
} to_type_01_def;
#define to_type_01_def_Size 32
#pragma section personalizations_type_01
/* Definition PERSONALIZATIONS-TYPE-01 created on 09/11/2024 at 11:37 */
#pragma fieldalign shared2 __personalizations_type_01
typedef struct __personalizations_type_01 {
  short to_count;
  to_type_01_def to_rw[4];
  dynamic_template_data_01_def dynamic_template_data;
} personalizations_type_01_def;
#define personalizations_type_01_def_Size 1670
#pragma section from_type_01
/* Definition FROM-TYPE-01 created on 09/11/2024 at 11:37 */
#pragma fieldalign shared2 __from_type_01
typedef struct __from_type_01 {
  char email[32];
} from_type_01_def;
#define from_type_01_def_Size 32
#pragma section post_send_v_3
/* Definition POST-SEND-V-3 created on 09/11/2024 at 11:37 */
#pragma fieldalign shared2 __post_send_v_3
typedef struct __post_send_v_3 {
  short personalizations_count;
  personalizations_type_01_def personalizations[4];
  from_type_01_def from_rw;
  char subject[32];
  char template_id[100];
} post_send_v_3_def;
#define post_send_v_3_def_Size 6846
#pragma section post_send_rq
/* Definition POST-SEND-RQ created on 09/11/2024 at 11:37 */
#pragma fieldalign shared2 __post_send_rq
typedef struct __post_send_rq {
  lightwave_rq_header_def lightwave_rq_header;
  post_send_v_3_def post_send_v_3;
} post_send_rq_def;
#define post_send_rq_def_Size 6878
#pragma section post_send_202_rp
/* Definition POST-SEND-202-RP created on 09/11/2024 at 11:37 */
#pragma fieldalign shared2 __post_send_202_rp
typedef struct __post_send_202_rp {
  lightwave_rp_header_def lightwave_rp_header;
} post_send_202_rp_def;
#define post_send_202_rp_def_Size 32
#pragma section post_send_400_rp
/* Definition POST-SEND-400-RP created on 09/11/2024 at 11:37 */
#pragma fieldalign shared2 __post_send_400_rp
typedef struct __post_send_400_rp {
  lightwave_rp_header_def lightwave_rp_header;
} post_send_400_rp_def;
#define post_send_400_rp_def_Size 32
#pragma section post_send_401_rp
/* Definition POST-SEND-401-RP created on 09/11/2024 at 11:37 */
#pragma fieldalign shared2 __post_send_401_rp
typedef struct __post_send_401_rp {
  lightwave_rp_header_def lightwave_rp_header;
} post_send_401_rp_def;
#define post_send_401_rp_def_Size 32
#pragma section post_send_403_rp
/* Definition POST-SEND-403-RP created on 09/11/2024 at 11:37 */
#pragma fieldalign shared2 __post_send_403_rp
typedef struct __post_send_403_rp {
  lightwave_rp_header_def lightwave_rp_header;
} post_send_403_rp_def;
#define post_send_403_rp_def_Size 32
#pragma section post_send_500_rp
/* Definition POST-SEND-500-RP created on 09/11/2024 at 11:37 */
#pragma fieldalign shared2 __post_send_500_rp
typedef struct __post_send_500_rp {
  lightwave_rp_header_def lightwave_rp_header;
} post_send_500_rp_def;
#define post_send_500_rp_def_Size 32