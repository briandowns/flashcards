#ifndef _MODELS_H
#define _MODELS_H

#include <stdlib.h>

typedef struct user {
    int id;
    char *first_name;
    char *last_name;
    char *email;
} user_t;

typedef struct deck {
    int id;
    char *description;
    int user_id;
} deck_t;

typedef struct card {
    int id;
    char *front;
    char *back;
    char *label;
    int deck_id;
    int user_id;
} card_t;

user_t* user_new(const char *first_name, const char *last_name, const char *email);
void user_free(user_t *user);
deck_t* deck_new(const char *desc, const uint64_t user_id);
void deck_free(deck_t *deck);
card_t* card_new(const uint64_t deck_id, const uint64_t user_id);
void card_free(card_t *card);

#endif /* _MODELS_H */
