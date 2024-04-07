
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <mysql/mysql.h>
#include <sqlite3.h>
#include <ulfius.h>

#include "api.h"
#include "database.h"
#include "models.h"

#define STR1(x) #x
#define STR(x) STR1(x)

#define PORT 8080

int control = 0;

int callback_get_test (const struct _u_request * request, struct _u_response * response, void * user_data) {
    (void)(request);
    (void)(user_data);
    ulfius_set_string_body_response(response, 200, "Hello World!");
    return U_CALLBACK_CONTINUE;
}

/**
 * sig_handler captures ctrl-c
 */
static void sig_handler(int dummy) {
    control = 1;
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
    signal(SIGINT, sig_handler);

    database_init();

    struct _u_instance instance;

    y_init_logs("simple_example", Y_LOG_MODE_CONSOLE, Y_LOG_LEVEL_DEBUG, NULL, "Starting simple_example");

    if (ulfius_init_instance(&instance, PORT, NULL, NULL) != U_OK) {
        y_log_message(Y_LOG_LEVEL_ERROR, "Error ulfius_init_instance, abort");
        return(1);
    }

    if (api_add_routes(&instance) != 0) {
        perror("unable to add routes");
        return EXIT_FAILURE;
    }

    // Start the framework
    if (ulfius_start_framework(&instance) == U_OK) {
        printf("Start framework on port %d\n", instance.port);

        // Wait for the user to press <enter> on the console to quit the application
        getchar();
    } else {
        fprintf(stderr, "error starting framework\n");
    }

    ulfius_stop_framework(&instance);
    ulfius_clean_instance(&instance);

    deck_t *deck1 = deck_new("Italian Lanuage Verbs", 1);
    if (insert_deck(deck1) != 0) {
        return 1;
    }
    deck_free(deck1);

    deck_t *deck2 = get_deck_by_id(2);
    if (deck2 == NULL) {
        printf("error\n");
        return 0;
    }
    
    printf("id: %d\n", deck2->id);
    printf("desc: %s\n", deck2->description);
    printf("user id: %d\n", deck2->user_id);

    deck_free(deck2);

    return 0;
}