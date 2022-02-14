#ifndef CAUTHFLOW_JSON_COMMON_H
#define CAUTHFLOW_JSON_COMMON_H

#include <curl_simple_https.h>
#include <parson.h>
#if __STDC_VERSION__ >= 199901L
#include <stdbool.h>
#else
#define inline
#include <cauthflow_stdbool.h>
#endif /* __STDC_VERSION__ >= 199901L */
#include <cauthflow_utils_export.h>

extern CAUTHFLOW_UTILS_EXPORT JSON_Value *
if_bad_status_exit(const struct ServerResponse *);

extern CAUTHFLOW_UTILS_EXPORT JSON_Value *if_error_exit(const JSON_Value *,
                                                        bool always_throw);

#endif /* CAUTHFLOW_JSON_COMMON_H */
