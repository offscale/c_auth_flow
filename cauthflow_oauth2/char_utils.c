#include "char_utils.h"

inline bool is_whitespace(const char ch) {
    return ch == 0x00|| ch == 0x20 || ch == 0x0A || ch == 0x0D || ch == 0x09;
}

inline bool is_sign(const char ch) {
    return ch == '+' || ch == '-';
}

inline bool is_exponent(const char ch) {
    return ch == 'e' || ch == 'E';
}

inline bool is_decimal_point(const char ch) {
    return ch == '.';
}

inline bool is_quote(const char ch) {
    return ch == '"' || ch == '\'';
}

inline bool is_comma(const char ch) {
    return ch == ',';
}

inline bool is_colon(const char ch) {
    return ch == ':';
}

inline bool is_keyword(const char *text) {
    return strcmp(text, "true") == 0 || strcmp(text, "false") == 0 || strcmp(text, "null") == 0;
}

inline bool is_alpha(const char ch) {
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

inline bool is_key(const char ch) {
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch>= '0' && ch <= '9') || ch == '_';
}

inline bool is_digit(const char n) {
    return n >= '0' && n <= '9';
}

inline bool is_domain_character(const char ch) {
    return is_alpha(ch) || is_digit(ch) || ch == '-' || ch == '.';
}


inline bool is_sub_delims(const char ch) {
    return ch == '!' || ch == '$' || ch == '&' || ch == '\'' || ch == '(' || ch == ')' ||
           ch == '*' || ch == '+' || ch == ',' || ch == ';' || ch == '=';
}

inline bool is_hex_digit(const char ch) {
    return ch == 'a' || ch == 'A' ||
           ch == 'b' || ch == 'B' ||
           ch == 'c' || ch == 'C' ||
           ch == 'd' || ch == 'D' ||
           ch == 'e' || ch == 'E' ||
           ch == 'f' || ch == 'F' ||
           is_digit(ch);
}

inline bool is_pct_encoded(const char ch) {
    return ch == '%' || is_hex_digit(ch);
}

inline bool is_unreserved(const char ch) {
    return ch == '-' || ch == '.' || ch == '_' || ch == '~' || is_alpha(ch) || is_digit(ch);
}

inline bool is_pchar(const char ch) {
    return ch == ':' || ch == '@' || is_unreserved(ch) || is_pct_encoded(ch) || is_sub_delims(ch);
}

/* See the RFC 3986. */
inline bool is_fragment(const char ch) {
    return ch == '/' || ch == '?' || is_pchar(ch);
}

inline bool is_valid_path_char(const char ch) {
    return ch == '/' || is_fragment(ch);
}
