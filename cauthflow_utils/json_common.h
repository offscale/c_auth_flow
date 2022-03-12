#ifndef CAUTHFLOW_JSON_COMMON_H
#define CAUTHFLOW_JSON_COMMON_H

#include <curl_simple_https.h>
#include <parson.h>

#ifdef __cplusplus
#include <cstdbool>
#include <cstring>
extern "C" {
#else
#if __STDC_VERSION__ >= 199901L
#include <stdbool.h>
#else
#define inline
#include <cauthflow_stdbool.h>
#endif /* __STDC_VERSION__ >= 199901L */
#endif /* __cplusplus */
#include <cauthflow_utils_export.h>

extern CAUTHFLOW_UTILS_EXPORT JSON_Value *
if_bad_status_exit(const struct ServerResponse *);

extern CAUTHFLOW_UTILS_EXPORT JSON_Value *if_error_exit(const JSON_Value *,
                                                        bool always_throw);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CAUTHFLOW_JSON_COMMON_H */
