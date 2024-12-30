#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include <sqlite3.h>
#include "bank_manager.h"

// Function prototypes
void handle_employee_menu(sqlite3 *db);
void display_employee_menu(void);
void handle_employee_login(sqlite3 *db);

// Employee operations
void view_client_information(sqlite3 *db);
void perform_transaction(sqlite3 *db);
void view_all_transactions(sqlite3 *db);
void add_new_client(sqlite3 *db);
void modify_client(sqlite3 *db);

#endif // EMPLOYEE_H
