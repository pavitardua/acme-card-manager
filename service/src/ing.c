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
#include "lwc.h"

#pragma list

/* Static variables. */
static char pathmon_name[32];
static char* kfk_consumer_serverclass = "KFK-CONSUMERSVR";
static char* kfk_forward_serverclass = "pay-server";

/* add PATMON NAME TO THE CONSUMER_REQ STRUCT BELOW */
typedef struct {
  char topic[64];
  char pathmon[64];
  char server_class[64];
} topic_serverclass_pair;

typedef struct {
  char action[32];
  char group_name[64];
  char instance_name[64];
  int timeout;
  int max_bytes;
  char format[32];
  char auto_offset_reset[32];
  int auto_commit;
  topic_serverclass_pair topics[10];
  int topic_count;
} consumer_request_v2;
static void begin_consumption(consumer_request_v2 req);

static void begin_consumption(consumer_request_v2 req) {
  char response_body[1024];
  short cc;
  short pathsend_error = 0;
  short file_system_error = 0;
  printf("inside function begin_consumption consumer_request group_name %s \n",
         req.group_name);
  /*
   snprintf(consumer_req.topics[1].topic, sizeof(consumer_req.topics[1].topic),
  "payments"); snprintf(consumer_req.topics[1].pathmon,
  sizeof(consumer_req.topics[1].pathmon), "macme");
  snprintf(consumer_req.topics[1].server_class,
  sizeof(consumer_req.topics[1].server_class), kfk_forward_serverclass);
  */

  cc = SERVERCLASS_SENDL_(
      (char*)pathmon_name, (short)strlen(pathmon_name),
      (char*)kfk_consumer_serverclass, (short)strlen(kfk_consumer_serverclass),
      (char*)&req, (char*)&response_body, sizeof(req), sizeof(response_body));

  if (cc) {
    SERVERCLASS_SEND_INFO_(&pathsend_error, &file_system_error);
    printf("Pathsend Error %d and File System Error %d\n", pathsend_error,
           file_system_error);
    printf("Error sending request to server class %s: %d\n",
           kfk_consumer_serverclass, cc);

  } else {
    printf("Request sent to server class %s\n", kfk_consumer_serverclass);
  }
}

int main(int argc, char* argv[], char** envp) {
  consumer_request_v2 consumer_req;
  char* p;

  /* Get our pathmon name. */
  if ((p = getenv("PATHMON-NAME")) == NULL) {
    printf("PARAM PATHMON^NAME is not set.\n");
    return -1;
  }
  strncpy(pathmon_name, p, sizeof(pathmon_name));

  snprintf(consumer_req.action, sizeof(consumer_req.action), "full_flow");
  snprintf(consumer_req.group_name, sizeof(consumer_req.group_name),
           "consumer_grp_2025");
  snprintf(consumer_req.instance_name, sizeof(consumer_req.instance_name),
           "consumer_inst_2025");
  consumer_req.timeout = 5000;
  consumer_req.max_bytes = 1024;
  snprintf(consumer_req.format, sizeof(consumer_req.format), "json");
  snprintf(consumer_req.auto_offset_reset,
           sizeof(consumer_req.auto_offset_reset), "earliest");
  consumer_req.auto_commit = 0;

  consumer_req.topic_count = 1;
  snprintf(consumer_req.topics[0].topic, sizeof(consumer_req.topics[0].topic),
           "payments");
  snprintf(consumer_req.topics[0].pathmon,
           sizeof(consumer_req.topics[0].pathmon), "$MACME");
  snprintf(consumer_req.topics[0].server_class,
           sizeof(consumer_req.topics[0].server_class),
           kfk_forward_serverclass);

  printf("consumer_req group_name %s \n", consumer_req.group_name);
  begin_consumption(consumer_req);

  return 0;
}

/** End of file */