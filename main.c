
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mysql/mysql.h>
#include <sqlite3.h>

#define FLASHCARD_DB "flashcards.db"

typedef struct user {
    int id;
    char *first_name;
    char *last_name;
    char *email;
} user_t;

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

typedef struct deck {
    int id;
    char *description;
    int user_id;
} deck_t;

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

typedef struct card {
    int id;
    char *front;
    char *back;
    char *label;
    int deck_id;
    int user_id;
} card_t;

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

int create_table(sqlite3 *db, const char *table) {
    sqlite3_stmt *stmt;

    char *query = {0};
    char *err = {0};
    
    if (strcmp(table, "decks") == 0) {
        query = strdup("CREATE TABLE IF NOT EXISTS decks(id INTEGER PRIMARY KEY AUTOINCREMENT, description TEXT, user_id INTEGER)");
    } else if (strcmp(table, "cards") == 0) {
        query = strdup("CREATE TABLE IF NOT EXISTS cards(id INTEGER PRIMARY KEY AUTOINCREMENT, front TEXT, back TEXT, labels TEXT, deck_id INTEGER, user_id INTEGER)");
    } else if (strcmp(table, "users") == 0) {
        query = strdup("CREATE TABLE IF NOT EXISTS users(id INTEGER PRIMARY KEY AUTOINCREMENT, first_name TEXT, last_name TEXT, email TEXT)");
    }

    int rc = sqlite3_exec(db, query, NULL, 0, &err);
    if(rc != SQLITE_OK){
        fprintf(stderr, "erroring creating table [%s]: %s\n", table, err);
        sqlite3_free(err);

        return 1;
    }

    free(query);

    return 0;
}

int insert_deck(sqlite3 *db, const deck_t *deck) {
    char *err = {0};
    char *query = sqlite3_mprintf("INSERT INTO decks (description, user_id) VALUES (%Q, %d)", deck->description, deck->user_id);
    printf("%s\n", query);
    int rc = sqlite3_exec(db, query, NULL, 0, &err);
    if(rc != SQLITE_OK){
        fprintf(stderr, "create table error: %s\n", err);
        sqlite3_free(err);

        return 1;
    }

    return 0;
}

static int callback(void *data, int argc, char **argv, char **azColName) {
    deck_t *deck = (deck_t*)data;
    printf("row count: %d\n", argc);
    for (int i = 0; i < argc; i++) {
        if (strcmp(azColName[i], "id") == 0) {
            printf("%d\n", atoi(argv[i]));
            deck->id = argv[i] ? atoi(argv[i]) : 0;
        }

        if (strcmp(azColName[i], "description") == 0) {
            if (argv[i]) {
                deck->description = malloc(strlen(argv[i] + 1));
                strcpy(deck->description, argv[i]);
            }
        }

        if (strcmp(azColName[i], "user_id") == 0) {
            deck->user_id = argv[i] ? atoi(argv[i]) : 0;
        }

        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }

    printf("\n");

     
    return 0;
}

deck_t* get_deck_by_id(sqlite3 *db, const int id) {
    deck_t *deck = deck_new("", 0);

    char *err = {0};
    char *query = sqlite3_mprintf("SELECT * FROM decks WHERE id = %d", id);
    printf("%s\n", query);

    int rc = sqlite3_exec(db, query, callback, deck, &err);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "error getting deck [%d]: %s\n", id, err);
        sqlite3_free(err);
        deck_free(deck);

        return NULL;
    }

    return deck;
}

card_t* get_card_by_id(sqlite3 *db, const u_int64_t id) {
    sqlite3_stmt *stmt;

    sqlite3_prepare_v2(db, "SELECT * FROM cards WHERE id = ?", -1, &stmt, NULL);
    sqlite3_bind_int64(stmt, 1, id);

	while (sqlite3_step(stmt) != SQLITE_DONE) {
        printf("%d\n", 0);
		int num_cols = sqlite3_column_count(stmt);
		printf("%d\n", num_cols);
		for (int i = 0; i < num_cols; i++) {
			switch (sqlite3_column_type(stmt, i)) {
			case (SQLITE3_TEXT):
				printf("%s, ", sqlite3_column_text(stmt, i));
				break;
			case (SQLITE_INTEGER):
				printf("%d, ", sqlite3_column_int(stmt, i));
				break;
			case (SQLITE_FLOAT):
				printf("%g, ", sqlite3_column_double(stmt, i));
				break;
			default:
				break;
			}
		}
	}

    return NULL;
}

int main(int argc, char **argv) {
    sqlite3 *db;

    sqlite3_open(FLASHCARD_DB, &db);

	if (db == NULL) {
		printf("failed to open DB\n");
		return 1;
	}

    char *tables[] = {"decks", "cards", "users"};
    for (size_t i = 0; i < sizeof(tables) / sizeof(tables[0]); i++) {
        if (create_table(db, "decks") == 1) {
            return 1;
        }
    }

    deck_t *deck1 = deck_new("Italian Lanuage Verbs", 1);
    if (insert_deck(db, deck1) != 0) {
        return 1;
    }
    deck_free(deck1);

    deck_t *deck2 = get_deck_by_id(db, 1);
    if (deck2 == NULL) {
        printf("error\n");
        return 0;
    }
    deck_free(deck2);

    printf("id: %d\n", deck2->id);
    printf("desc: %s\n", deck2->description);
    printf("user id: %d\n", deck2->user_id);

    sqlite3_close_v2(db);

    return 0;
}