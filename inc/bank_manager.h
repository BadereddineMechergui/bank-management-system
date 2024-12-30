#ifndef BANK_MANAGER_H
#define BANK_MANAGER_H

#include <sqlite3.h>
#include <time.h>

// Structures for our tables
typedef struct {
    int id;
    char first_name[50];
    char last_name[50];
    char username[50];
    char password[100];
} Employee;

typedef struct {
    char card_number[20];
    char expiry_date[11];
    char holder_id[20];
    char network[20];
    char cvv[4];
} Credit_card;

typedef struct {
    char national_id[20];
    char first_name[50];
    char last_name[50];
    char account_type[20];
    double balance;
    char pin[5];
    char phone[15];
} Client;

typedef struct {
    int id;
    int employee_id;
    char client_id[20];
    char date[20];
    char type[20];
    double amount;
} Bank_transaction;

// Database initialization
int bank_init_database(sqlite3 **db, const char *db_path);

// Employee operations
int bank_add_employee(sqlite3 *db, Employee *emp);
int bank_get_employee_by_id(sqlite3 *db, int id, Employee *emp);
int bank_verify_employee_credentials(sqlite3 *db, const char *username, const char *password);
int bank_update_employee(sqlite3 *db, Employee *emp);
int bank_delete_employee(sqlite3 *db, int id);
int bank_list_employees(sqlite3 *db, Employee **employees, int *count);

// Client operations
int bank_add_client(sqlite3 *db, Client *client, const char *pin);
int bank_get_client_by_id(sqlite3 *db, const char *national_id, Client *client);
int bank_update_client(sqlite3 *db, Client *client);
int bank_delete_client(sqlite3 *db, const char *national_id);
int bank_list_clients(sqlite3 *db, Client **clients, int *count);
int bank_update_client_balance(sqlite3 *db, const char *national_id, double amount);
int bank_verify_client_pin(sqlite3 *db, const char *national_id, const char *pin);

// Credit card operations
int bank_add_credit_card(sqlite3 *db, const Credit_card *card);
int bank_get_credit_card(sqlite3 *db, const char *card_number, Credit_card *card);
int bank_update_credit_card(sqlite3 *db, const Credit_card *card);
int bank_delete_credit_card(sqlite3 *db, const char *card_number);
int bank_list_client_cards(sqlite3 *db, const char *national_id, Credit_card **cards, int *count);

// Transaction operations
int bank_record_transaction(sqlite3 *db, Bank_transaction *trans);
Bank_transaction* bank_get_client_transaction_history(sqlite3 *db, const char *national_id, int *count);
Bank_transaction* bank_get_all_transactions(sqlite3 *db, int *count);
int bank_get_transaction_by_id(sqlite3 *db, int id, Bank_transaction *trans);
int bank_get_transactions_by_date_range(sqlite3 *db, const char *start_date, const char *end_date, Bank_transaction **trans, int *count);
int bank_get_transactions_by_type(sqlite3 *db, const char *type, Bank_transaction **trans, int *count);

#endif // BANK_MANAGER_H
