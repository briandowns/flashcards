#include <ctype.h>
#include <strings.h>
#include <time.h>

#include <ulfius.h>
#include <jansson.h>

#include "api.h"
#include "database.h"
#include "http.h"
#include "models.h"

#define HEALTH_STATUS_SICK    "sick"
#define HEALTH_STATUS_HEALTHY "healthy"

#define USER     "bdowns"
#define PASSWORD "asdfqwer"

#define PORT 3000

#define HEALTH_PATH  "/healthz"
#define DECK_PATH "/deck"
#define CARD_PATH "/card"
#define SEARCH_PATH  "/search"

#define PATIENT_NOT_FOUND_ERROR() {                                                 \
    json_t * json_body = json_object();                                             \
    json_object_set_new(json_body, "error", json_string("Patient Does Not Exist")); \
    ulfius_set_json_body_response(response, HTTP_STATUS_OK, json_body);             \
    json_decref(json_body);                                                         \
}                                                                                   \

/**
 * time_spent takes the start time of a route handler
 * and calculates how long it ran for. It then returns 
 * that value to be logged.
 */
#define TIME_SPENT(x) {                         \
    clock_t diff = clock() - x;                 \
    int msec = diff * 1000 / CLOCKS_PER_SEC;    \
    msec % 1000;                                \
}                                               \

/**
 * callback_health_check handles all health check
 * requests to the service
 */
static int callback_health(const struct _u_request *request, struct _u_response *response, void *user_data) {
    json_t * json_body = json_object();
    json_object_set_new(json_body, "status", json_string(HEALTH_STATUS_HEALTHY)); 
    json_object_set_new(json_body, "git_sha", json_string("asdfasdfasdfasdf1234123412341234s"));
    ulfius_set_json_body_response(response, 200, json_body);
    json_decref(json_body);
    return U_CALLBACK_CONTINUE;
}

/**
 * auth_basic is responsible for basic authentication for 
 * configured endpoints
 */
int auth_basic(const struct _u_request *request, struct _u_response *response, void *user_data) {
    if (request->auth_basic_user != NULL && request->auth_basic_password != NULL && 
        0 == o_strcmp(request->auth_basic_user, USER) && 0 == o_strcmp(request->auth_basic_password, PASSWORD)) {
        return U_CALLBACK_CONTINUE;
    } else {
        ulfius_set_string_body_response(response, 401, "error authentication");
        return U_CALLBACK_UNAUTHORIZED;
    } 
}

int callback_patient(const struct _u_request *request, struct _u_response *response, void *user_data) {
    clock_t start = clock();
    return U_CALLBACK_CONTINUE;
}

/**
 * callback_default is used to handled calls that don't have
 * a matching route. Returns an expected 404.
 */
int callback_default(const struct _u_request *request, struct _u_response *response, void *user_data) {
    ulfius_set_string_body_response(response, HTTP_STATUS_NOT_FOUND, "page not found");
    return U_CALLBACK_CONTINUE;
}

int api_add_routes(struct _u_instance *instance) {
    ulfius_add_endpoint_by_val(instance, HTTP_METHOD_GET, HEALTH_PATH, NULL, 0, &callback_health, NULL);
    // ulfius_add_endpoint_by_val(&instance, "HTTP_METHOD_POST", PATIENT_PATH, NULL, 0, &auth_basic, "auth param");
    // ulfius_add_endpoint_by_val(&instance, "HTTP_METHOD_POST", API_PREFIX RELATIONSHIPS_PATH, NULL, 1, &callback_relationships, NULL);

    ulfius_set_default_endpoint(instance, &callback_default, NULL);
    return 0;
}


