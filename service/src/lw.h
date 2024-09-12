#ifndef LW_H
#define LW_H

/* SCHEMA PRODUCED DATE - TIME : 9/04/2024 - 12:11:52 */
#pragma section lightwave_rp_code_enum
/* Definition LIGHTWAVE-RP-CODE-ENUM created on 09/04/2024 at 12:11 */
enum { lw_rp_success = 0, lw_rp_info = 1, lw_rp_error = 2 };
typedef short lightwave_rp_code_enum_def;
#pragma section lightwave_info_code_enum
/* Definition LIGHTWAVE-INFO-CODE-ENUM created on 09/04/2024 at 12:11 */
enum { lw_info_field_truncated = 100, lw_info_array_truncated = 101 };
typedef short lightwave_info_code_enum_def;
#pragma section lightwave_error_src_enum
/* Definition LIGHTWAVE-ERROR-SRC-ENUM created on 09/04/2024 at 12:11 */
enum {
  lw_error_src_lightwave = 1,
  lw_error_src_http = 2,
  lw_error_src_tcpip = 3,
  lw_error_src_ssl = 4
};
typedef short lightwave_error_src_enum_def;
#pragma section lightwave_error_enum
/* Definition LIGHTWAVE-ERROR-ENUM created on 09/04/2024 at 12:11 */
enum {
  lw_error_invalid_license = 1,
  lw_error_invalid_header = 2,
  lw_error_invalid_rq_code = 3,
  lw_error_invalid_type = 4,
  lw_error_serialization_error = 5,
  lw_error_deserialization_error = 6,
  lw_error_request_timeout = 7,
  lw_error_response_not_defined = 12,
  lw_error_invalid_uri_scheme = 100,
  lw_error_invalid_api_def = 101,
  lw_error_invalid_base_uri = 102,
  lw_error_invalid_hdr_version = 103,
  lw_error_signature_error = 104,
  lw_error_internal_error = 500
};
typedef short lightwave_error_enum_def;
#pragma section lightwave_rq_header
/* Definition LIGHTWAVE-RQ-HEADER created on 09/04/2024 at 12:11 */
#include <tnsint.h>
#pragma fieldalign shared2 __lightwave_rq_header
typedef struct __lightwave_rq_header {
  short rq_code;
  unsigned short rq_version;
  __uint32_t rq_timeout;
  char reserved[24];
} lightwave_rq_header_def;
#define lightwave_rq_header_def_Size 32
#pragma section lightwave_rp_header
/* Definition LIGHTWAVE-RP-HEADER created on 09/04/2024 at 12:11 */
#pragma fieldalign shared2 __lightwave_rp_header
typedef struct __lightwave_rp_header {
  short rp_code;
  short http_status;
  short info_code;
  short info_detail;
  char reserved[24];
} lightwave_rp_header_def;
#define lightwave_rp_header_def_Size 32
#pragma section lightwave_error_rp
/* Definition LIGHTWAVE-ERROR-RP created on 09/04/2024 at 12:11 */
#include <tnsint.h>
#pragma fieldalign shared2 __lightwave_error_rp
typedef struct __lightwave_error_rp {
  lightwave_rp_header_def lightwave_rp_header;
  __int32_t error_source;
  __int32_t error_code;
  __int32_t error_subcode;
  char error_message[4096];
} lightwave_error_rp_def;
#define lightwave_error_rp_def_Size 4140

#endif /* LW_H */