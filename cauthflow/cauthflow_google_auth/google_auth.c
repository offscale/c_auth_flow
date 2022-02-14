/**
 * OAuth2 in C/C++
 * Tested for Google auth, following this guide:
 * https://developers.google.com/identity/protocols/cauthflow/native-app
 */

#define XSTR(s) STR(s)
#define STR(s) #s

#define DEBUG_SERVER_RESPONSE(name)                                            \
  fprintf(response.status_code == 200 ? stdout : stderr, name ": %ld\t%s\n",   \
          response.status_code, response.body == NULL ? "" : response.body)

#if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
#define NOWINBASEINTERLOCK
#include <intrin.h>
#define strdup _strdup

#endif

#include <curl/curl.h>
#include <parson.h>

#include <json_common.h>
#define RANDOMSTR_IMPLEMENTATION
#include <randomstr.h>
#undef RANDOMSTR_IMPLEMENTATION

#define C89STRINGUTILS_IMPLEMENTATION
#include <c89stringutils_string_extras.h>
#undef C89STRINGUTILS_IMPLEMENTATION

#include <open_browser.h>
#include <tiny_web_server.h>

#include <curl_simple_https.h>

#include "google_auth.h"
#include <cauthflow_configure.h>

#include <stdarg.h>
#include <stdlib.h>

#ifdef bool
#undef bool
#endif
#if __STDC_VERSION__ >= 199901L
#include <stdbool.h>
typedef long long longest;
#else
#define inline
#include <cauthflow_stdbool.h>
typedef long longest;
#endif /* __STDC_VERSION__ >= 199901L */

void no_projects_error();

CURLUcode append_curl_query(CURLU *urlp, const char *fmt, ...) {
  char *_buf;
  CURLUcode rc;

  va_list args;
  va_start(args, fmt);
  vasprintf(&_buf, fmt, args);
  va_end(args);

  rc = curl_url_set(urlp, CURLUPART_QUERY, _buf, CURLU_APPENDQUERY);
  free(_buf);
  return rc;
}

/*
 * Authentication flow (including spinning up of local web server, and setting
 * of project ID)
 * */
extern struct GoogleCloudProject get_google_auth(const char *client_id,
                                                 const char *client_secret,
                                                 const char *refresh_token) {
  JSON_Object *tokens_obj_json;

  if (refresh_token == NULL) {
    const struct StrStr auth_flow_code_resp =
        auth_flow_user_approval(client_id);
    const char *redirect_uri = auth_flow_code_resp.first,
               *code = auth_flow_code_resp.second;

    const JSON_Value *tokens_json =
        auth_flow_get_tokens(client_id, client_secret, code, redirect_uri);
    tokens_obj_json = json_value_get_object(tokens_json);
    refresh_token = json_object_get_string(tokens_obj_json, "refresh_token");
  } else {
    const JSON_Value *tokens_json = auth_flow_get_tokens_from_refresh(
        client_id, client_secret, refresh_token);
    tokens_obj_json = json_value_get_object(tokens_json);
  }

  {
    const char *google_access_token =
        json_object_get_string(tokens_obj_json, "access_token");
    const double google_access_token_expires_in =
        json_object_get_number(tokens_obj_json, "expires_in");
    const time_t google_access_token_expiry =
        time(NULL) + (longest)google_access_token_expires_in;
    struct GoogleCloudProject project;

    const JSON_Object *project_object_json = get_project(google_access_token);
    if (!json_object_has_value(project_object_json, "projectId"))
      no_projects_error();

    project.projectNumber =
        json_object_get_string(project_object_json, "projectNumber");
    project.projectId =
        json_object_get_string(project_object_json, "projectId");
    project.lifecycleState =
        json_object_get_string(project_object_json, "lifecycleState");
    project.name = json_object_get_string(project_object_json, "name");
    project.google_access_token_expiry = google_access_token_expiry;
    project.google_access_token = google_access_token;
    project.google_refresh_token = refresh_token;
    return project;
  }
}

inline struct StrStr auth_flow_user_approval(const char *client_id) {
  const char *redirect_uri =
      "http://" SERVER_HOST ":" PORT_TO_BIND_S EXPECTED_PATH;
  const char *temporary_secret_state = generate_random_string(10);
  const char *scope = /*"openid"
                      "%20"
                      "https://www.googleapis.com/auth/cloud-platform"
                      "%20"
                      "https://www.googleapis.com/auth/cloudplatformprojects"
                      "%20"
                      "https://www.googleapis.com/auth/devstorage.read_write"*/
      "openid"
      "%20"
      "https://www.googleapis.com/auth/userinfo.email"
      "%20"
      "https://www.googleapis.com/auth/cloud-platform"
      "%20"
      "https://www.googleapis.com/auth/accounts.reauth";

  char *url;
  CURLU *urlp = curl_url();
  CURLUcode rc = curl_url_set(urlp, CURLUPART_SCHEME, "https", 0);
  rc = curl_url_set(urlp, CURLUPART_HOST, "accounts.google.com", 0);
  rc = curl_url_set(urlp, CURLUPART_PORT, "443", 0);
  rc = curl_url_set(urlp, CURLUPART_PATH, "/o/oauth2/v2/auth", 0);
  rc = curl_url_set(urlp, CURLUPART_QUERY,
                    "response_type=code&access_type=offline", 0);
  rc = append_curl_query(urlp, "state=%s", temporary_secret_state);

#ifdef CAUTHFLOW_CLIENT_FROM_CONFIG
  rc = curl_url_set(urlp, CURLUPART_QUERY, "client_id=" CLIENT_ID,
                    CURLU_APPENDQUERY);
#else
  {
    char *client_id_str;
    asprintf(&client_id_str, "client_id=%s", client_id);
    rc = curl_url_set(urlp, CURLUPART_QUERY, client_id_str, CURLU_APPENDQUERY);
    free(client_id_str);
  }
#endif /* CAUTHFLOW_CLIENT_FROM_CONFIG */

  rc = append_curl_query(urlp, "redirect_uri=%s", redirect_uri);
  rc = append_curl_query(urlp, "scope=%s", scope);
  rc = curl_url_get(urlp, CURLUPART_URL, &url, 0);
  if (rc == CURLUE_OK) {
    open_browser(url);
    curl_free(url);
  }

  /* we then need to start our web server and block
     until we get the appropriate response */
  {
    const struct AuthenticationResponse oauth_response =
        wait_for_oauth2_redirect();
    struct StrStr str_str;

    if (oauth_response.secret == NULL ||
        strcmp(oauth_response.secret, temporary_secret_state) != 0) {
      fprintf(stderr,
              "cauthflow redirect contained the wrong secret state (%s), "
              "expected: (%s)\n",
              oauth_response.secret == NULL ? "(NULL)" : oauth_response.secret,
              temporary_secret_state);
      exit(EXIT_FAILURE);
    } else {
      printf("\n"
             "struct AuthenticationResponse oauth_response = {\n"
             "  .scope=\"%s\",\n"
             "  .secret=\"%s\",\n"
             "  .code=\"%s\"\n"
             "};\n",
             oauth_response.scope, oauth_response.secret, oauth_response.code);
    }

    str_str.first = redirect_uri;
    str_str.second = oauth_response.code;
    return str_str;
  }
}

inline JSON_Value *auth_flow_get_tokens(const char *client_id,
                                        const char *client_secret,
                                        const char *code,
                                        const char *redirect_uri) {
  CURLU *urlp = curl_url();
  CURLUcode rc = curl_url_set(urlp, CURLUPART_SCHEME, "https", 0);

  rc = curl_url_set(urlp, CURLUPART_HOST, "oauth2.googleapis.com", 0);
  rc = curl_url_set(urlp, CURLUPART_PORT, "443", 0);
  rc = curl_url_set(urlp, CURLUPART_PATH, "/token", 0);
  rc = curl_url_set(urlp, CURLUPART_QUERY, "grant_type=authorization_code", 0);
  rc = append_curl_query(urlp, "code=%s", code);
  rc = append_curl_query(urlp, "redirect_uri=%s", redirect_uri);
#ifdef CAUTHFLOW_CLIENT_FROM_CONFIG
  rc = curl_url_set(urlp, CURLUPART_QUERY, "client_id=" CLIENT_ID,
                    CURLU_APPENDQUERY);
  rc = curl_url_set(urlp, CURLUPART_QUERY, "client_secret=" CLIENT_SECRET,
                    CURLU_APPENDQUERY);
#else
  {
    char *client_id_str;
    asprintf(&client_id_str, "client_id=%s", client_id);
    rc = curl_url_set(urlp, CURLUPART_QUERY, client_id_str, CURLU_APPENDQUERY);
    free(client_id_str);
  }
  {
    char *client_secret_str;
    asprintf(&client_secret_str, "client_secret=%s", client_secret);
    rc = curl_url_set(urlp, CURLUPART_QUERY, client_secret_str,
                      CURLU_APPENDQUERY);
    free(client_secret_str);
  }
#endif /* CAUTHFLOW_CLIENT_FROM_CONFIG */
  if (rc != CURLUE_OK)
    return NULL;
  {
    struct ServerResponse response = https_json_post(urlp, NULL, NULL);
    DEBUG_SERVER_RESPONSE("auth_flow_get_tokens");
    if (response.status_code != 200) {
      fputs("request failed to get token", stderr);
      exit(EXIT_FAILURE);
    }

    return if_error_exit(json_parse_string(response.body), false);
  }
}

inline JSON_Value *
auth_flow_get_tokens_from_refresh(const char *client_id,
                                  const char *client_secret,
                                  const char *refresh_token) {
  CURLU *urlp = curl_url();
  CURLUcode rc = curl_url_set(urlp, CURLUPART_SCHEME, "https", 0);

  rc = curl_url_set(urlp, CURLUPART_HOST, "oauth2.googleapis.com", 0);
  rc = curl_url_set(urlp, CURLUPART_PORT, "443", 0);
  rc = curl_url_set(urlp, CURLUPART_PATH, "/token", 0);
  rc = curl_url_set(urlp, CURLUPART_QUERY, "grant_type=refresh_token", 0);

#ifdef CAUTHFLOW_CLIENT_FROM_CONFIG
  rc = curl_url_set(urlp, CURLUPART_QUERY, "client_id=" CLIENT_ID,
                    CURLU_APPENDQUERY);
  rc = curl_url_set(urlp, CURLUPART_QUERY, "client_secret=" CLIENT_SECRET,
                    CURLU_APPENDQUERY);
#else
  {
    char *client_id_str;
    asprintf(&client_id_str, "client_id=%s", client_id);
    rc = curl_url_set(urlp, CURLUPART_QUERY, client_id_str, CURLU_APPENDQUERY);
    free(client_id_str);
  }
  {
    char *client_secret_str;
    asprintf(&client_secret_str, "client_secret=%s", client_secret);
    rc = curl_url_set(urlp, CURLUPART_QUERY, client_secret_str,
                      CURLU_APPENDQUERY);
    free(client_secret_str);
  }
#endif /* CAUTHFLOW_CLIENT_FROM_CONFIG */

  rc = append_curl_query(urlp, "refresh_token=%s", refresh_token);
  if (rc != CURLUE_OK)
    return NULL;

  {
    struct ServerResponse response = https_json_post(urlp, NULL, NULL);
    DEBUG_SERVER_RESPONSE("auth_flow_get_tokens_from_refresh");
    if (response.status_code != 200) {
      fputs("request failed to get token", stderr);
      exit(EXIT_FAILURE);
    }
#ifdef DEBUG_HTTP
    fputs(response.body, stderr);
#endif

    return if_error_exit(json_parse_string(response.body), false);
  }
}

inline const JSON_Object *get_project(const char *google_access_token) {
  /* https://cloud.google.com/resource-manager/reference/rest/v1/projects/list
   * GET https://cloudresourcemanager.googleapis.com/v1/projects
   * */
  CURLU *urlp = curl_url();
  CURLUcode rc = CURLUE_OK;
  struct curl_slist *headers = NULL;
  char *auth_header;
  struct ServerResponse response = {CURLE_OK, NULL, 100, NULL};

  rc = curl_url_set(urlp, CURLUPART_SCHEME, "https", 0);
  rc = curl_url_set(urlp, CURLUPART_HOST, "cloudresourcemanager.googleapis.com",
                    0);
  rc = curl_url_set(urlp, CURLUPART_PATH, "/v1/projects", 0);
  if (rc != CURLUE_OK)
    return NULL;

  asprintf(&auth_header, "Authorization: Bearer %s", google_access_token);
  headers = curl_slist_append(headers, auth_header);
  response = https_json_get(urlp, NULL, headers);
  free(auth_header);
  DEBUG_SERVER_RESPONSE("get_project");
  if (response.status_code > 299 || response.body == NULL ||
      response.code != CURLE_OK) {
    fprintf(stderr, "request failed to get projects\n\n%s\n", response.body);
    exit(EXIT_FAILURE);
  }
  if_bad_status_exit(&response);

  {
    const JSON_Value *json_value = json_parse_string(response.body);
    const JSON_Array *projects_items_json =
        json_object_get_array(json_value_get_object(json_value), "projects");
    const size_t projects_items_n = json_array_get_count(projects_items_json);
    size_t i;

    if (projects_items_n == 0)
      no_projects_error();

    for (i = 0; i < projects_items_n; i++) {
      const JSON_Object *project_obj =
          json_array_get_object(projects_items_json, i);
      if (strcmp(json_object_get_string(project_obj, "lifecycleState"),
                 "ACTIVE") == 0) {
        const char *projectId =
            json_object_get_string(project_obj, "projectId");
        const size_t projectId_n = strlen(projectId);
        const bool is_quickstart =
            (projectId_n > 11 /* len("quickstart-") */
             && projectId[0] == 'q' && projectId[1] == 'u' &&
             projectId[2] == 'i' && projectId[3] == 'c' &&
             projectId[4] == 'k' && projectId[5] == 's' &&
             projectId[6] == 't' && projectId[7] == 'a' &&
             projectId[8] == 'r' && projectId[9] == 't' &&
             projectId[10] == '-');
        if (!is_quickstart)
          return project_obj;
      }
    }
  }
  return NULL;
}

void no_projects_error() {
  fputs("A project must be created. For details on how and why, see: "
        "https://cloud.google.com/resource-manager/docs/"
        "creating-managing-projects",
        stderr);
  exit(EXIT_FAILURE);
}
