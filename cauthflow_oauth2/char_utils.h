#ifndef OAUTH2_CHAR_UTILS_H
#define OAUTH2_CHAR_UTILS_H

#include <string.h>
#include "oauth2_export.h"

#if __STDC_VERSION__ >= 199901L
#include <stdbool.h>
#else
#define inline
#include <cauthflow_stdbool.h>
#endif /* __STDC_VERSION__ >= 199901L */

extern OAUTH2_EXPORT bool is_whitespace(char);

extern OAUTH2_EXPORT bool is_sign(char);

extern OAUTH2_EXPORT bool is_exponent(char);

extern OAUTH2_EXPORT bool is_decimal_point(char);

extern OAUTH2_EXPORT bool is_quote(char);

extern OAUTH2_EXPORT bool is_comma(char);

extern OAUTH2_EXPORT bool is_colon(char);

extern OAUTH2_EXPORT bool is_keyword(const char*);

extern OAUTH2_EXPORT bool is_alpha(char);

extern OAUTH2_EXPORT bool is_key(char);

extern OAUTH2_EXPORT bool is_digit(char);

extern OAUTH2_EXPORT bool is_domain_character(char);

extern OAUTH2_EXPORT bool is_sub_delims(char);

extern OAUTH2_EXPORT bool is_hex_digit(char);

extern OAUTH2_EXPORT bool is_pct_encoded(char);

extern OAUTH2_EXPORT bool is_unreserved(char);

extern OAUTH2_EXPORT bool is_pchar(char);

// See the RFC 3986.
extern OAUTH2_EXPORT bool is_fragment(char);

extern OAUTH2_EXPORT bool is_valid_path_char(char);

#endif /* !OAUTH2_CHAR_UTILS_H */
