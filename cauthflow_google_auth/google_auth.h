#ifndef CAUTHFLOW_GOOGLE_AUTH_H
#define CAUTHFLOW_GOOGLE_AUTH_H

/**
 * OAuth2 in C/C++
 * Tested for Google auth, following this guide:
 * https://developers.google.com/identity/protocols/cauthflow/native-app
 */

#ifdef __cplusplus
#include <cstdbool>
#include <ctime>
extern "C" {
#else
#include <time.h>
#if __STDC_VERSION__ >= 199901L
#include <stdbool.h>
#else
#include <cauthflow_stdbool.h>
#endif /* __STDC_VERSION__ >= 199901L */
#endif /* __cplusplus */
#include <parson.h>

#include "cauthflow_google_auth_export.h"
#include <cauthflow_types_common.h>

struct GoogleCloudProject {
  const char *projectNumber;
  const char *projectId;
  const char *lifecycleState;
  const char *name;
  time_t google_access_token_expiry;
  const char *google_access_token;
  const char *google_refresh_token;
};

extern CAUTHFLOW_GOOGLE_AUTH_EXPORT struct GoogleCloudProject
get_google_auth(const char *, const char *, const char *,
                struct StrStr (*)(const char *, const char *));
extern CAUTHFLOW_GOOGLE_AUTH_EXPORT struct StrStr
auth_flow_user_approval(const char *,
                        struct StrStr (*)(const char *, const char *));
extern CAUTHFLOW_GOOGLE_AUTH_EXPORT JSON_Value *
auth_flow_get_tokens(const char *, const char *, const char *, const char *);
extern CAUTHFLOW_GOOGLE_AUTH_EXPORT JSON_Value *
auth_flow_get_tokens_from_refresh(const char *, const char *, const char *);
extern CAUTHFLOW_GOOGLE_AUTH_EXPORT const JSON_Object *
get_project(const char *);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* !CAUTHFLOW_GOOGLE_AUTH_H */
