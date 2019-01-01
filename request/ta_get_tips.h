#ifndef REQUEST_TA_GET_TIPS_H_
#define REQUEST_TA_GET_TIPS_H_

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  int opt;
} ta_get_tips_req_t;

ta_get_tips_req_t* ta_get_tips_req_new();
void ta_get_tips_req_free(ta_get_tips_req_t* req);

#ifdef __cplusplus
}
#endif

#endif  // REQUEST_TA_GET_TIPS_H_
