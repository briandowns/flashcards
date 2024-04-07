#ifndef _DATABASE_H
#define _DATABASE_H

#include <mysql/mysql.h>
#include <sqlite3.h>

#include "models.h"

#define FLASHCARD_DB "flashcards.db"

void database_init();
void database_cleanup();
int insert_deck(const deck_t *deck);
deck_t* get_deck_by_id(const int id);
card_t* get_card_by_id(const u_int64_t id);

#endif /* _DATABASE_H */
