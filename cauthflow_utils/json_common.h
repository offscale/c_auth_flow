#ifndef PP_JSON_COMMON_H
#define PP_JSON_COMMON_H

#include <parson.h>
#include <curl_simple_https.h>
#if __STDC_VERSION__ >= 199901L
#include <stdbool.h>
#else
#define inline
#include <cauthflow_stdbool.h>
#endif /* __STDC_VERSION__ >= 199901L */
#include "cauthflow_utils_export.h"

extern CAUTHFLOW_UTILS_EXPORT
JSON_Value* if_bad_status_exit(const struct ServerResponse *);

extern CAUTHFLOW_UTILS_EXPORT
JSON_Value* if_error_exit(const JSON_Value *, bool always_throw);

#endif /* PP_JSON_COMMON_H */
