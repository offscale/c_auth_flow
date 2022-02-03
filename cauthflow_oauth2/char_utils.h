#ifndef OAUTH2_CHAR_UTILS_H
#define OAUTH2_CHAR_UTILS_H

#include <string.h>
#include "cauthflow_oauth2_export.h"

#if __STDC_VERSION__ >= 199901L
#include <stdbool.h>
#else
#define inline
#include <cauthflow_stdbool.h>
#endif /* __STDC_VERSION__ >= 199901L */

extern CAUTHFLOW_OAUTH2_EXPORT bool is_whitespace(char);

extern CAUTHFLOW_OAUTH2_EXPORT bool is_sign(char);

extern CAUTHFLOW_OAUTH2_EXPORT bool is_exponent(char);

extern CAUTHFLOW_OAUTH2_EXPORT bool is_decimal_point(char);

extern CAUTHFLOW_OAUTH2_EXPORT bool is_quote(char);

extern CAUTHFLOW_OAUTH2_EXPORT bool is_comma(char);

extern CAUTHFLOW_OAUTH2_EXPORT bool is_colon(char);

extern CAUTHFLOW_OAUTH2_EXPORT bool is_keyword(const char*);

extern CAUTHFLOW_OAUTH2_EXPORT bool is_alpha(char);

extern CAUTHFLOW_OAUTH2_EXPORT bool is_key(char);

extern CAUTHFLOW_OAUTH2_EXPORT bool is_digit(char);

extern CAUTHFLOW_OAUTH2_EXPORT bool is_domain_character(char);

extern CAUTHFLOW_OAUTH2_EXPORT bool is_sub_delims(char);

extern CAUTHFLOW_OAUTH2_EXPORT bool is_hex_digit(char);

extern CAUTHFLOW_OAUTH2_EXPORT bool is_pct_encoded(char);

extern CAUTHFLOW_OAUTH2_EXPORT bool is_unreserved(char);

extern CAUTHFLOW_OAUTH2_EXPORT bool is_pchar(char);

// See the RFC 3986.
extern CAUTHFLOW_OAUTH2_EXPORT bool is_fragment(char);

extern CAUTHFLOW_OAUTH2_EXPORT bool is_valid_path_char(char);

#endif /* !OAUTH2_CHAR_UTILS_H */
