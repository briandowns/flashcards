#include <stdlib.h>
#include <string.h>

#include "models.h"

user_t* user_new(const char *first_name, const char *last_name, const char *email) {
    user_t *user = malloc(sizeof(user_t));
    strcpy(user->first_name, first_name);
    strcpy(user->last_name, last_name);
    strcpy(user->email, email);

    return user;
}

void user_free(user_t *user) {
    if (user != NULL) {
        if (user->first_name != NULL) {
            free(user->first_name);
        }
        if (user->last_name != NULL) {
            free(user->last_name);
        }
        if (user->email != NULL) {
            free(user->email);
        }
        free(user);
    }
}

deck_t* deck_new(const char *desc, const uint64_t user_id) {
    deck_t *deck = malloc(sizeof(deck_t));
    deck->description = malloc(strlen(desc));
    strcpy(deck->description, desc);
    deck->user_id = user_id;

    return deck;
}

void deck_free(deck_t *deck) {
    if (deck != NULL) {
        if (deck->description != NULL) {
            free(deck->description);
        }

        free(deck);
    }
}

card_t* card_new(const uint64_t deck_id, const uint64_t user_id) {
    card_t *card = malloc(sizeof(card_t));
    card->deck_id = deck_id;
    card->user_id = user_id;

    return card;
}

void card_free(card_t *card) {
    if (card != NULL) {
        if (card->front != NULL) {
            free(card->front);
        }

        if (card->back != NULL) {
            free(card->back);
        }

        if (card->label != NULL) {
            free(card->label);
        }

        free(card);
    }
}
