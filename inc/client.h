#ifndef CLIENT_H
#define CLIENT_H

#include <sqlite3.h>

// Function prototypes
void display_client_menu(void);
void handle_client_login(sqlite3 *db);
void handle_client_menu(sqlite3 *db, const char *national_id);
void view_account_info(sqlite3 *db, const char *national_id);
void make_transaction(sqlite3 *db, const char *national_id);
void view_transaction_history(sqlite3 *db, const char *national_id);
void update_personal_info(sqlite3 *db, const char *national_id);
void view_credit_cards(sqlite3 *db, const char *national_id);

#endif // CLIENT_H
