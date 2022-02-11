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
strcat_s (char * dest, rsize_t dmax, const char * src)
{
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

int serve(char **response) {
    int code;
    char pipe_buf[PIPE_BUF+1];


#if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
    /* Initialize Winsock */
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        fprintf(stderr, "WSAStartup failed: %d\n", iResult);
    }
#endif

    int socket_options = SO_DEBUG;
    const int server_socket = (int)socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in svr_addr, cli_addr;
    socklen_t sin_len = sizeof(cli_addr);
    int port = PORT_TO_BIND;

    if (server_socket < 0)
        err(1, "can't open socket");

    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&socket_options, sizeof(int));


    svr_addr.sin_family = AF_INET;
    svr_addr.sin_addr.s_addr = INADDR_ANY;
    svr_addr.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr *)&svr_addr, sizeof(svr_addr)) == -1)
    {
#if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
        closesocket(server_socket);
#else
        close(server_socket);
#endif
        fputs(strerror(code), stderr);
        return code;
    }

    code = listen(server_socket, MSG_BACKLOG);
    if (code == -1) {
        fputs(strerror(errno), stderr);
        return code;
    }

    /*if (*response == NULL)
        *response = malloc(sizeof(char)*PIPE_BUF+1);*/

    while(true) {
        ssize_t bytes, total_bytes=0;
        const int client_fd = accept(server_socket,
                                     (struct sockaddr *)&cli_addr, &sin_len);
        if (client_fd == -1) {
            fputs(strerror(code), stderr);
            return code;
        }
        puts("Running server on http://"SERVER_HOST ":" PORT_TO_BIND_S);

        /* memset(pipe_buf, 0, PIPE_BUF); */
        do {
            bytes = read(client_fd, *response, PIPE_BUF);
            if (bytes == -1) {
                if (total_bytes == 0) {
                    fputs(strerror(errno), stderr);
                    return EXIT_FAILURE;
                }
            } else {
                response = realloc(*response, total_bytes + bytes);
                memcpy(*response + bytes, pipe_buf, bytes);
                total_bytes += bytes;
            }
            printf("read bytes: %ld\n"
                   "buffer: %s\n", bytes, *response);
            fflush(stdout);
        } while(bytes > 0);

        if (strncmp(STOP_ON_STARTSWITH, *response, strlen(STOP_ON_STARTSWITH)) == 0) {
            bytes = write(client_fd, responseOk, sizeof responseOk);
            if (bytes == -1) {
                fputs(strerror(errno), stderr);
                return EXIT_FAILURE;
            }
            printf("wrote bytes: %ld\n", bytes);
#if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
            send(client_fd, responseOk, sizeof(responseOk) - 1, 0 );
            closesocket(client_fd);
#else
            write(client_fd, responseOk, sizeof(responseOk) - 1);
            close(client_fd);
#endif
            return EXIT_SUCCESS;
        }
    }
}

struct AuthenticationResponse wait_for_oauth2_redirect() {
   struct AuthenticationResponse authentication_response = {NULL, NULL, NULL};
   char *response=NULL;
   int code;
   puts("serve()");
   code = serve(&response);
   fprintf(stderr, "serve::code: %d\n"
                   "serve::response: %s\n",
                   code, response);
   puts("fin server()");
   if (code != EXIT_SUCCESS) {
     fputs("server() failed", stderr);
   }
   return authentication_response;

#if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
    /* Initialize Winsock */
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        fprintf(stderr, "WSAStartup failed: %d\n", iResult);
    }
#endif

    int socket_options = SO_DEBUG;
    int client_file_descriptor;

    struct sockaddr_in svr_addr, cli_addr;
    socklen_t sin_len = sizeof(cli_addr);

    int server_socket = (int)socket(AF_INET, SOCK_STREAM, 0);
    int port = PORT_TO_BIND;
    int listen_resp;
    bool ok;
    char *incoming_datastream;
    char buffer[STACK_SIZE];
#define QUERY_N 350
    char query[QUERY_N]; /* roughly 350 chars expected from current output as printed */
    size_t i, j;
    char *_query, *key;
    const size_t initial_size = STACK_SIZE * sizeof(char);

#if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
    int
#else
    unsigned long
#endif
            bytes, total_bytes=0;

    if (server_socket < 0)
        err(1, "can't open socket");

    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&socket_options, sizeof(int));


    svr_addr.sin_family = AF_INET;
    svr_addr.sin_addr.s_addr = INADDR_ANY;
    svr_addr.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr *)&svr_addr, sizeof(svr_addr)) == -1)
    {
#if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
        closesocket(server_socket);
#else
        close(server_socket);
#endif
        err(1, "Can't bind");
    }

    listen_resp = listen(server_socket, MSG_BACKLOG);
    if (listen_resp != 0) {
        exit(EXIT_FAILURE);
    }
    ok = false;
    while (!ok)
    {
        client_file_descriptor = (int)accept(server_socket, (struct sockaddr *)&cli_addr, &sin_len);
        puts("got incoming connection");

        if (client_file_descriptor == -1)
        {
            perror("Can't accept");
            continue;
        }

        /* keep on reading until we have digest everything */
        incoming_datastream = (char *)(malloc(initial_size));


        if (incoming_datastream == NULL) err(ERROR_EOM_OVERFLOW, "OOM");

#if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
            bytes = recv(client_file_descriptor, buffer, STACK_SIZE, 0);
#else
            bytes = read(client_file_descriptor, buffer, STACK_SIZE);
#endif

        total_bytes += bytes;
        incoming_datastream = realloc(incoming_datastream, sizeof(char)*total_bytes + 1);
        if (incoming_datastream && bytes > 0) {
#if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
            strcat_s(incoming_datastream, bytes, buffer);
#else
            incoming_datastream[bytes] = '\0';
            strcat_s(incoming_datastream, bytes, buffer);
            /* strcat_s (char * dest, rsize_t dmax, const char * src)
            strcat(incoming_datastream, buffer);*/
#endif
        }

        if (incoming_datastream != NULL && total_bytes > 0) {
            fputs("before NUL", stderr);
            incoming_datastream[total_bytes] = '\0';
            fputs("after NUL", stderr);
            fputs("zero", stderr);
        }

#if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
        else {
            fputs("Error while reading incoming data stream.", stderr);
            _write(client_file_descriptor, responseErr, sizeof(responseErr) - 1);
            continue;
        }
#else
        else {
            exit(EXIT_FAILURE);
        }
#endif

        fputs("one", stderr);
        /* check if this is a GET method */
        if ( incoming_datastream[2] != 'G' ||
             incoming_datastream[3] != 'E' ||
             incoming_datastream[4] != 'T' ) {

#if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
            send(client_file_descriptor, responseOk, sizeof(responseOk) - 1, 0);
#else
            write(client_file_descriptor, responseOk, sizeof(responseOk) - 1); /*-1:'\0'*/
#endif
            continue;
        }
        fputs("three", stderr);


        for (i=0, j=6; i<QUERY_N && !isspace(incoming_datastream[j]); i++, j++)
            query[i] = incoming_datastream[j];
        query[i + 1] = '\0';
#undef QUERY_N
        j = i + 1;

        _query = (char*)malloc(j * sizeof(char));
        memcpy(_query, query, j);

        for (; (key = keyValuePair(&_query)); ) {
            char *value = key;
            const char *_key = extractKey(&value);
            if (strcmp("code", _key) == 0)
                authentication_response.code = value;
            else if (strcmp(EXPECTED_PATH"?state", _key) == 0)
                authentication_response.secret = value;
        }
        free(_query);

        ok = authentication_response.code != NULL && authentication_response.secret != NULL;
        
        if ( ok ) {
#if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
            send( client_file_descriptor, responseOk, sizeof(responseOk) - 1, 0 );
            closesocket(client_file_descriptor);
#else
            write(client_file_descriptor, responseOk, sizeof(responseOk) - 1); /*-1:'\0'*/
            close(client_file_descriptor);
#endif
            break;
        } else {
#if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
            send( client_file_descriptor, responseErr, sizeof(responseErr) - 1, 0 );
            closesocket(client_file_descriptor);
#else
            write(client_file_descriptor, responseErr, sizeof(responseErr) - 1); /*-1:'\0'*/
            close(client_file_descriptor);
#endif
        }

    }
#if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
    WSACleanup();
#endif
    return authentication_response;
}

#ifdef TEST_TINY_WEB_SERVER
int main()
{
    wait_for_oauth2_redirect();
}
#endif
