#ifndef GOOGLE_AUTH_H
#define GOOGLE_AUTH_H

/**
 * OAuth2 in C/C++
 * Tested for Google auth, following this guide:
 * https://developers.google.com/identity/protocols/cauthflow/native-app
 */

#ifdef	__cplusplus
#include <ctime>
extern "C" {
#else
#include <time.h>
#endif
#include <parson.h>

#include "cauthflow_google_auth_export.h"

struct GoogleCloudProject {
    const char *projectNumber;
    const char *projectId;
    const char *lifecycleState;
    const char *name;
    time_t google_access_token_expiry;
    const char *google_access_token;
    const char *google_refresh_token;
};

struct StrStr {
    const char *first, *second;
};

extern CAUTHFLOW_GOOGLE_AUTH_EXPORT struct GoogleCloudProject get_google_auth(const char*);
extern CAUTHFLOW_GOOGLE_AUTH_EXPORT struct StrStr auth_flow_user_approval(void);
extern CAUTHFLOW_GOOGLE_AUTH_EXPORT JSON_Value * auth_flow_get_tokens(const char *, const char *);
extern CAUTHFLOW_GOOGLE_AUTH_EXPORT JSON_Value * auth_flow_get_tokens_from_refresh(const char *);
extern CAUTHFLOW_GOOGLE_AUTH_EXPORT const JSON_Object * get_project(const char *);

#ifdef	__cplusplus
} /*extern "C"*/
#endif

#endif /* GOOGLE_AUTH_H */
