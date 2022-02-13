/* Reference: https://rosettacode.org/wiki/Hello_world/Web_server#C */
#include <stdio.h>
#include <stdlib.h>

#include <cauthflow_configure.h>
#include "tiny_web_server.h"

#if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
#define NOWINBASEINTERLOCK
#include <intrin.h>
#include <ws2tcpip.h>
#include <winsock2.h>
#include <io.h>

void err(int code, const char *message) {
    fputs(message, stderr);
    exit(code == EXIT_SUCCESS? EXIT_FAILURE : code);
}

char* strsep(char** stringp, const char* delim)
{
    char* start = *stringp;
    char* p;

    p = (start != NULL) ? strpbrk(start, delim) : NULL;

    if (p == NULL)
    {
        *stringp = NULL;
    }
    else
    {
        *p = '\0';
        *stringp = p + 1;
    }

    return start;
}

#else

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <err.h>
#include <ctype.h>
#include <netdb.h>

#define ERROR_EOM_OVERFLOW EXIT_FAILURE
#endif

#include "macros.h"

#ifndef SOCK_NONBLOCK

#include <fcntl.h>
#include <errno.h>
#include <sys/syslimits.h>

# define SOCK_NONBLOCK O_NONBLOCK
#endif /* ! SOCK_NONBLOCK */

const char responseOk[] = "HTTP/1.0 200 OK\r\n"
                          "Content-Type: text/plain\r\n"
                          "\r\n"
                          "Ok. You may close this tab and return to the shell.\r\n";
const char responseErr[] = "HTTP/1.0 400 Bad Request\r\n"
                           "Content-Type: text/plain\r\n"
                           "\r\n"
                           "Bad Request\r\n";
const static char STOP_ON_STARTSWITH[] = "GET " EXPECTED_PATH;

/**
* Percent-decodes a string in-place.
*/
static void percentDecode(char *s) {
    /* TODO */
    puts(s);
}

/**
 * Returns a pointer to the beginning of the a key-value pair, writing
 * a NUL delimiter to the input.  Advances input to the next key-value pair.
 */
char *keyValuePair(char **input) {
    return strsep(input, "&");
}

/**
 * Splits keyValue into two strings, and performs percent-decoding on both.
 * Returns a pointer to the key, and advances keyValue to point to the value.
 */
char *extractKey(char **keyValue) {
    char *key = strsep(keyValue, "=");
    percentDecode(key);
    percentDecode(*keyValue);
    return key;
}

/*std::map<std::string, std::string>
split_querystring(std::string const & querystring ) 
{
    std::map<std::string, std::string> result;
    size_t lastpos = 0;
    std::string key;
    std::string value;
    for(size_t ii=0; ii < querystring.size(); ii++ ) {
        if ( querystring[ii] == '=' ) {
            key = querystring.substr(lastpos, ii-lastpos);
            lastpos = ii+1;
        } else if ( querystring[ii] == '&' ) {
            value = querystring.substr(lastpos, ii-lastpos);
            result.insert(std::make_pair(key,value));
            lastpos = ii+1;
            key.clear();
            value.clear();
        }
    }
    if ( !key.empty() ) {
        value = querystring.substr(lastpos);
        result.insert(std::make_pair(key,value));
    }
    return result;
}*/

void append(char *s, char c) {
    size_t len = strlen(s);
    s[len] = c;
    s[len + 1] = '\0';
}

errno_t
strcat_s(char *dest, rsize_t dmax, const char *src) {
    rsize_t orig_dmax;
    char *orig_dest;
    const char *overlap_bumper;

    if (dest == NULL || src == NULL || dmax == 0 || dmax > 1000000) return -1;

    /* hold base of dest in case src was not copied */
    orig_dmax = dmax;
    orig_dest = dest;

    if (dest < src) {
        overlap_bumper = src;

        /* Find the end of dest */
        while (*dest != '\0') {

            if (dest == overlap_bumper) return -1;

            dest++;
            dmax--;
            if (dmax == 0) return -1;
        }

        while (dmax > 0) {
            if (dest == overlap_bumper) return -1;

            *dest = *src;
            if (*dest == '\0') {
#ifdef SAFECLIB_STR_NULL_SLACK
                /* null slack to clear any data */
                 while (dmax) { *dest = '\0'; dmax--; dest++; }
#endif
                return 0;
            }

            dmax--;
            dest++;
            src++;
        }

    } else {
        overlap_bumper = dest;

        /* Find the end of dest */
        while (*dest != '\0') {

            /*
             * NOTE: no need to check for overlap here since src comes first
             * in memory and we're not incrementing src here.
             */
            dest++;
            dmax--;
            if (dmax == 0) return -1;
        }

        while (dmax > 0) {
            if (src == overlap_bumper) return -1;

            *dest = *src;
            if (*dest == '\0') {
#ifdef SAFECLIB_STR_NULL_SLACK
                /* null slack to clear any data */
                 while (dmax) { *dest = '\0'; dmax--; dest++; }
#endif
                return 0;
            }

            dmax--;
            dest++;
            src++;
        }
    }

    /*
     * the entire src was not copied, so null the string
     */
    fputs("strcat_s: not enough "
          "space for src",
          stderr);

    return -1;
}

int write_and_close_socket(int client_fd, const char responseMessage[], size_t messageSize) {
    ssize_t wrote;
    int closed_code;
#if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
    wrote = send(client_fd, responseMessage, messageSize - 1, 0 );
    closed_code = closesocket(client_fd);
#else
    wrote = write(client_fd, responseMessage, messageSize - 1);
    closed_code = close(client_fd);
#endif
    return wrote == -1 || closed_code == -1 ? -1 : 0;
}

int serve(char **response) {
#define STD_ERROR_HANDLER(code)                                  \
            if((code) == -1) {                                   \
                const int _c = fputs(strerror(errno), stderr);   \
                if (_c == EOF) return _c;                        \
                return code == EXIT_SUCCESS? EXIT_FAILURE: code; \
            } else

    struct addrinfo hint = {0}, *p, *info = NULL;
    int code, sockfd;
    size_t current_size = PIPE_BUF;
    struct sockaddr_storage client_addr;
    socklen_t addr_size = sizeof client_addr;
    char pipe_buf[PIPE_BUF + 1];

#if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
    /* Initialize Winsock */
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        const int _code = fprintf(stderr, "WSAStartup failed: %d\n", iResult);
        if (_code == EOF) return _code;
        return code == EXIT_SUCCESS? EXIT_FAILURE: code;
    }
#endif /* defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__) */

    hint.ai_family = AF_INET;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_flags = AI_PASSIVE | SOCK_NONBLOCK;
    code = getaddrinfo(NULL, PORT_TO_BIND_S, &hint, &info);
    if (code != 0) {
        fputs(gai_strerror(code), stderr);
        return code == EXIT_SUCCESS ? EXIT_FAILURE : code;
    }

    for (p = info; p; p = p->ai_next) {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd == -1) {
            continue;
        }
        code = bind(sockfd, p->ai_addr, p->ai_addrlen);
        if (code == -1) {
            const int _code = fputs(strerror(code), stderr);
            if (_code == EOF) return _code;
#if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
                STD_ERROR_HANDLER(closesocket(sockfd));
#else
            STD_ERROR_HANDLER(close(sockfd));
#endif
            continue;
        }

        break;
    }

    STD_ERROR_HANDLER(listen(sockfd, MSG_BACKLOG));

    /*if (*response == NULL) *response = malloc(sizeof(char)*PIPE_BUF+1);*/

    while (true) {
#if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
        int
#else
        ssize_t
#endif
                bytes, total_bytes = 0;
        const int client_fd = accept(sockfd, (struct sockaddr *) &client_addr, &addr_size);
        if (client_fd == -1) {
            const int _code = fputs(strerror(code), stderr);
            if (_code == EOF) return _code;
            continue;
        }
        puts("Running server on http://"SERVER_HOST ":" PORT_TO_BIND_S);

        /* memset(pipe_buf, 0, PIPE_BUF); */
        do {
            bytes = read(client_fd, pipe_buf, PIPE_BUF);
            printf("bytes: %ld\n", bytes);
            if (bytes == -1) {
                const int _code = fputs(strerror(errno), stderr);
                if (_code == EOF) return _code;
            } else if (bytes > 0) {
                const size_t new_size = total_bytes + bytes + 1;
                printf("b4 current_size: %ld\n"
                       "new_size: %ld\n"
                       "b4 total_bytes: %ld\n",
                       current_size, new_size, total_bytes);
                if (new_size > current_size + 1) {
                    printf("b4 sizeof response / sizeof *response[0]: %ld\n", sizeof response / sizeof *response[0]);
                    *response = realloc(*response, new_size + 1);
                    printf("l8 sizeof response / sizeof *response[0]: %ld\n", sizeof *response);
                    if (*response == NULL) {
                        const int _code = fputs("OOM", stderr);
                        if (_code == EOF) return _code;
                        return EXIT_FAILURE;
                    }
                }
                memcpy(*response + total_bytes, pipe_buf, bytes);
                total_bytes += bytes;
                current_size = total_bytes;
                printf("l8 current_size: %ld\n"
                       "l8 total_bytes: %ld\n", current_size, total_bytes);
            }
            if (bytes < PIPE_BUF)
                break;
            /*printf("read bytes: %ld\n"
                   "buffer: %s\n", bytes, *response);
            fflush(stdout);*/
        } while (bytes > 0);

        if (*response != NULL) {
            (*response)[total_bytes] = '\0';
            if (strncmp(STOP_ON_STARTSWITH, *response, strlen(STOP_ON_STARTSWITH)) == 0) {
                STD_ERROR_HANDLER(write_and_close_socket(client_fd, responseOk, sizeof(responseOk) - 1));
#if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
                Sleep(1000*
#else
                sleep(
#endif
                        50);
#if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
                WSACleanup();
#endif /* defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__) */
                return EXIT_SUCCESS;
            }
        }
        STD_ERROR_HANDLER(write_and_close_socket(client_fd, responseErr, sizeof(responseErr) - 1));
    }

#undef STD_ERROR_HANDLER
}

void
query_into_auth_response(struct AuthenticationResponse *authentication_response, const char *var, const char *val);

struct AuthenticationResponse wait_for_oauth2_redirect() {
    struct AuthenticationResponse authentication_response = {NULL, NULL, NULL};
    char *response = calloc(PIPE_BUF + 1, sizeof(char));
    if (response == NULL) {
        const int _code = fputs("OOM", stderr);
        if (_code == EOF) exit(_code);
        exit(EAI_MEMORY);
    } else {
        const int code = serve(&response);
        if (code != EXIT_SUCCESS) {
            fputs("serve() failed", stderr);
            exit(code);
        } else {
            /* querystring parsing */
            char *query = strdup(response), *tokens = query, *p;
            fputs("serve() succeeded", stderr);

            while ((p = strsep(&tokens, "&\n"))) {
                char *var = strtok(p, "="),
                        *val = NULL;
                if (var && (val = strtok(NULL, "="))) {
                    size_t i;
                    bool found_space = false;
                    for (i = 0; val[i] != '\0'; i++)
                        if (isspace(val[i])) {
                            found_space = true;
                            break;
                        }
                    if (found_space) {
                        val[i] = '\0';
                        query_into_auth_response(&authentication_response, var, val);
                        break;
                    } else query_into_auth_response(&authentication_response, var, val);;
                } else
                    fputs("<empty field>\n", stderr);
            }

            free(query);
            free(response);
        }
    }
    return authentication_response;
}

void
query_into_auth_response(struct AuthenticationResponse *authentication_response, const char *var, const char *val) {
    if (var[0] == 'G' && var[1] == 'E' && var[2] == 'T' && var[3] == ' ')
        (*authentication_response).secret = strdup(val);
    else if (strcmp(var, "code") == 0) (*authentication_response).code = strdup(val);
    else if (strcmp(var, "scope") == 0) (*authentication_response).scope = strdup(val);
}

#ifdef TEST_TINY_WEB_SERVER
int main()
{
    wait_for_oauth2_redirect();
}
#endif
