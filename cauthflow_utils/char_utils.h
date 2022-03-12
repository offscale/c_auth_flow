#ifndef CAUTHFLOW_CHAR_UTILS_H
#define CAUTHFLOW_CHAR_UTILS_H

#include "cauthflow_utils_export.h"

#ifdef __cplusplus
#include <cstdbool>
#include <cstring>
extern "C" {
#else
#include <string.h>

#if __STDC_VERSION__ >= 199901L
#include <stdbool.h>
#else
#define inline
#include <cauthflow_stdbool.h>
#endif /* __STDC_VERSION__ >= 199901L */
#endif /* __cplusplus */

extern CAUTHFLOW_UTILS_EXPORT bool is_whitespace(char);

extern CAUTHFLOW_UTILS_EXPORT bool is_sign(char);

extern CAUTHFLOW_UTILS_EXPORT bool is_exponent(char);

extern CAUTHFLOW_UTILS_EXPORT bool is_decimal_point(char);

extern CAUTHFLOW_UTILS_EXPORT bool is_quote(char);

extern CAUTHFLOW_UTILS_EXPORT bool is_comma(char);

extern CAUTHFLOW_UTILS_EXPORT bool is_colon(char);

extern CAUTHFLOW_UTILS_EXPORT bool is_keyword(const char *);

extern CAUTHFLOW_UTILS_EXPORT bool is_alpha(char);

extern CAUTHFLOW_UTILS_EXPORT bool is_key(char);

extern CAUTHFLOW_UTILS_EXPORT bool is_digit(char);

extern CAUTHFLOW_UTILS_EXPORT bool is_domain_character(char);

extern CAUTHFLOW_UTILS_EXPORT bool is_sub_delims(char);

extern CAUTHFLOW_UTILS_EXPORT bool is_hex_digit(char);

extern CAUTHFLOW_UTILS_EXPORT bool is_pct_encoded(char);

extern CAUTHFLOW_UTILS_EXPORT bool is_unreserved(char);

extern CAUTHFLOW_UTILS_EXPORT bool is_pchar(char);

/* See the RFC 3986. */
extern CAUTHFLOW_UTILS_EXPORT bool is_fragment(char);

extern CAUTHFLOW_UTILS_EXPORT bool is_valid_path_char(char);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !CAUTHFLOW_CHAR_UTILS_H */
