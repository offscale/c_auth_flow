#ifndef CAUTHFLOW_SERVER_H
#define CAUTHFLOW_SERVER_H

#ifdef __cplusplus
#include <cstdbool>
extern "C" {
#elif __STDC_VERSION__ >= 199901L
#include <stdbool.h>
#else
#include <cauthflow_stdbool.h>
#endif /* __cplusplus */

#include "cauthflow_export.h"
#include <cauthflow_types_common.h>
#include <string.h>

extern CAUTHFLOW_EXPORT const char responseOk[];
extern CAUTHFLOW_EXPORT const char responseErr[];

/* This is a specialised web server type functionality
   that waits on IBM to call us back.
   This is doing what startHttpServer is doing
   in PHP. */
struct AuthenticationResponse {
  const char *secret, *code, *scope;
};

extern CAUTHFLOW_EXPORT struct StrStr redirect_dance(const char *,
                                                     const char *);

extern CAUTHFLOW_EXPORT struct AuthenticationResponse
wait_for_oauth2_redirect();

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !CAUTHFLOW_SERVER_H */
