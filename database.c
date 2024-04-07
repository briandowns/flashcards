#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sqlite3.h>

#include "database.h"
#include "models.h"

static sqlite3 *db;

int create_table(const char *table) {
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

void database_init() {
    sqlite3_open(FLASHCARD_DB, &db);
	if (db == NULL) {
		printf("failed to open DB\n");
	}

    char *tables[] = {"decks", "cards", "users"};
    for (size_t i = 0; i < sizeof(tables) / sizeof(tables[0]); i++) {
        if (create_table("decks") == 1) {
        }
    }
}

void database_cleanup() {
    sqlite3_close_v2(db);
}

int insert_deck(const deck_t *deck) {
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
    deck->id = atoi(argv[0]);
    deck->description = realloc(deck->description, strlen(argv[1] + 1));
    strcpy(deck->description, argv[1]);
    deck->user_id = atoi(argv[2]);

    //printf("id: %d, desc: %s, user id: %d\n", deck->id, deck->description, deck->user_id);
    return 0;
}

deck_t* get_deck_by_id(const int id) {
    sqlite3_stmt *stmt;

    deck_t *deck = deck_new("", 0);

    char *err = {0};
    char *query = sqlite3_mprintf("SELECT * FROM decks WHERE id = %d", id);
    printf("%s\n", query);

    sqlite3_prepare_v2(db, query, -1, &stmt, NULL);

    // while (sqlite3_step(stmt) != SQLITE_DONE) {
	// 	int num_cols = sqlite3_column_count(stmt);

		// for (int i = 0; i < num_cols; i++) {
			// switch (sqlite3_column_type(stmt, i)) {
			// case (SQLITE3_TEXT):
			// 	printf("%s, ", sqlite3_column_text(stmt, i));
			// 	break;
			// case (SQLITE_INTEGER):
			// 	printf("%d, ", sqlite3_column_int(stmt, i));
			// 	break;
			// case (SQLITE_FLOAT):
			// 	printf("%g, ", sqlite3_column_double(stmt, i));
			// 	break;
			// default:
			// 	break;
			// }
		// }

        // for (int i = 0; i < num_cols; i++) {
        //     if (i == 0) {
        //         deck->id = sqlite3_column_int(stmt, i);
        //         printf("%d, ", sqlite3_column_int(stmt, i));
        //     }
        //     if (i == 1) {
        //         deck->description = malloc(strlen((char*)sqlite3_column_text(stmt, i)) + 1);
        //         strcpy(deck->description, (char*)sqlite3_column_text(stmt, i));
        //         printf("%s, ", sqlite3_column_text(stmt, i));
        //     }
        //     if (i == 2) {
        //         printf("%d, ", sqlite3_column_int(stmt, i));
        //         deck->user_id = sqlite3_column_int(stmt, i);
        //     }
        // }
        // printf("\n");
	// }

    int rc = sqlite3_exec(db, query, callback, deck, &err);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "error getting deck [%d]: %s\n", id, err);
        sqlite3_free(err);
        deck_free(deck);

        return NULL;
    }
    sqlite3_close_v2(db);

    printf("XXX - id: %d, desc: %s, user id: %d\n", deck->id, deck->description, deck->user_id);

    return deck;
}

card_t* get_card_by_id(const u_int64_t id) {
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
