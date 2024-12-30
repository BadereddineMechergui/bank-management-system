#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bank_manager.h"

int bank_init_database(sqlite3 **db, const char *db_path) {
    int rc = sqlite3_open(db_path, db);
    if (rc) {
        fprintf(stderr, "Cannot open database\n");
        return rc;
    }
    
    char *sql_file_path = "c:/Users/mecba/Desktop/cs50-project/schema.sql";
    char *err_msg = NULL;
    char *sql = NULL;
    
    FILE *file = fopen(sql_file_path, "r");
    if (file == NULL) {
        fprintf(stderr, "Cannot open SQL file\n");
        return -1;
    }
    
    fseek(file, 0, SEEK_END);
    long fsize = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    sql = malloc(fsize + 1);
    fread(sql, fsize, 1, file);
    fclose(file);
    sql[fsize] = 0;
    
    // Execute SQL without printing
    rc = sqlite3_exec(*db, sql, 0, 0, &err_msg);
    
    free(sql);
    if (rc != SQLITE_OK) {
        sqlite3_free(err_msg);
        return rc;
    }
    
    return SQLITE_OK;
}

int bank_add_employee(sqlite3 *db, Employee *emp) {
    char *sql = "INSERT INTO Employee (first_name, last_name, username, password) VALUES (?, ?, ?, ?);";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL prepare error: %s\n", sqlite3_errmsg(db));
        return rc;
    }
    
    sqlite3_bind_text(stmt, 1, emp->first_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, emp->last_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, emp->username, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, emp->password, -1, SQLITE_STATIC);
    
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_DONE) {
        emp->id = sqlite3_last_insert_rowid(db);
    }
    
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? SQLITE_OK : rc;
}

int bank_get_employee_by_id(sqlite3 *db, int id, Employee *emp) {
    char *sql = "SELECT * FROM Employee WHERE id = ?;";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    
    if (rc != SQLITE_OK) {
        return rc;
    }
    
    sqlite3_bind_int(stmt, 1, id);
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        emp->id = sqlite3_column_int(stmt, 0);
        strcpy(emp->first_name, (const char*)sqlite3_column_text(stmt, 1));
        strcpy(emp->last_name, (const char*)sqlite3_column_text(stmt, 2));
        strcpy(emp->username, (const char*)sqlite3_column_text(stmt, 3));
        strcpy(emp->password, (const char*)sqlite3_column_text(stmt, 4));
        sqlite3_finalize(stmt);
        return SQLITE_OK;
    }
    
    sqlite3_finalize(stmt);
    return SQLITE_ERROR;
}

int bank_register_new_client(sqlite3 *db, Client *client) {
    char *sql = "INSERT INTO Client (national_id, first_name, last_name, account_type, balance, pin, phone) "
                "VALUES (?, ?, ?, ?, ?, ?, ?);";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL prepare error: %s\n", sqlite3_errmsg(db));
        return rc;
    }
    
    sqlite3_bind_text(stmt, 1, client->national_id, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, client->first_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, client->last_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, client->account_type, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 5, client->balance);
    sqlite3_bind_text(stmt, 6, client->pin, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 7, client->phone, -1, SQLITE_STATIC);
    
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "SQL execution error: %s\n", sqlite3_errmsg(db));
    }
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? SQLITE_OK : rc;
}

int bank_update_client_balance(sqlite3 *db, const char *national_id, double amount) {
    char *sql = "UPDATE Client SET balance = balance + ? WHERE national_id = ?;";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    
    if (rc != SQLITE_OK) {
        return rc;
    }
    
    sqlite3_bind_double(stmt, 1, amount);
    sqlite3_bind_text(stmt, 2, national_id, -1, SQLITE_STATIC);
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? SQLITE_OK : rc;
}

int bank_record_transaction(sqlite3 *db, Bank_transaction *trans) {
    char *sql = "INSERT INTO Bank_transaction (employee_id, client_id, type, amount) VALUES (?, ?, ?, ?);";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL prepare error: %s\n", sqlite3_errmsg(db));
        return rc;
    }
    
    sqlite3_bind_int(stmt, 1, trans->employee_id);
    sqlite3_bind_text(stmt, 2, trans->client_id, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, trans->type, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 4, trans->amount);
    
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "SQL execution error: %s\n", sqlite3_errmsg(db));
    }
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? SQLITE_OK : rc;
}

Bank_transaction* bank_get_client_transaction_history(sqlite3 *db, const char *national_id, int *count) {
    char *sql = "SELECT * FROM Bank_transaction WHERE client_id = ? ORDER BY date DESC;";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL prepare error: %s\n", sqlite3_errmsg(db));
        *count = 0;
        return NULL;
    }
    
    sqlite3_bind_text(stmt, 1, national_id, -1, SQLITE_STATIC);
    
    // Count the number of transactions
    *count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        (*count)++;
    }
    sqlite3_reset(stmt);
    
    // Allocate memory for transactions
    Bank_transaction *transactions = malloc(sizeof(Bank_transaction) * (*count));
    if (transactions == NULL) {
        *count = 0;
        sqlite3_finalize(stmt);
        return NULL;
    }
    
    // Fetch transactions
    int i = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        transactions[i].id = sqlite3_column_int(stmt, 0);
        transactions[i].employee_id = sqlite3_column_int(stmt, 1);
        strcpy(transactions[i].client_id, (const char*)sqlite3_column_text(stmt, 2));
        strcpy(transactions[i].date, (const char*)sqlite3_column_text(stmt, 3));
        strcpy(transactions[i].type, (const char*)sqlite3_column_text(stmt, 4));
        transactions[i].amount = sqlite3_column_double(stmt, 5);
        i++;
    }
    
    sqlite3_finalize(stmt);
    return transactions;
}

int bank_verify_employee_credentials(sqlite3 *db, const char *username, const char *password) {
    char *sql = "SELECT COUNT(*) FROM Employee WHERE username = ? AND password = ?;";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL prepare error: %s\n", sqlite3_errmsg(db));
        return rc;
    }
    
    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password, -1, SQLITE_STATIC);
    
    rc = sqlite3_step(stmt);
    int result = SQLITE_ERROR;
    
    if (rc == SQLITE_ROW) {
        int count = sqlite3_column_int(stmt, 0);
        result = (count > 0) ? SQLITE_OK : SQLITE_ERROR;
    }
    
    sqlite3_finalize(stmt);
    return result;
}

// Employee operations
int bank_update_employee(sqlite3 *db, Employee *emp) {
    const char *sql = "UPDATE Employee SET first_name = ?, last_name = ?, username = ?, password = ? WHERE id = ?";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    
    if (rc == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, emp->first_name, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, emp->last_name, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, emp->username, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, emp->password, -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 5, emp->id);
        rc = sqlite3_step(stmt);
    }
    
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? SQLITE_OK : rc;
}

int bank_delete_employee(sqlite3 *db, int id) {
    const char *sql = "DELETE FROM Employee WHERE id = ?";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    
    if (rc == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id);
        rc = sqlite3_step(stmt);
    }
    
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? SQLITE_OK : rc;
}

int bank_list_employees(sqlite3 *db, Employee **employees, int *count) {
    const char *sql = "SELECT * FROM Employee";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    
    if (rc == SQLITE_OK) {
        // Count rows first
        *count = 0;
        while (sqlite3_step(stmt) == SQLITE_ROW) (*count)++;
        sqlite3_reset(stmt);
        
        // Allocate memory
        *employees = malloc(sizeof(Employee) * (*count));
        if (*employees == NULL) return SQLITE_NOMEM;
        
        // Fetch data
        int i = 0;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Employee *emp = &(*employees)[i++];
            emp->id = sqlite3_column_int(stmt, 0);
            strncpy(emp->first_name, (const char*)sqlite3_column_text(stmt, 1), sizeof(emp->first_name)-1);
            strncpy(emp->last_name, (const char*)sqlite3_column_text(stmt, 2), sizeof(emp->last_name)-1);
            strncpy(emp->username, (const char*)sqlite3_column_text(stmt, 3), sizeof(emp->username)-1);
            strncpy(emp->password, (const char*)sqlite3_column_text(stmt, 4), sizeof(emp->password)-1);
        }
    }
    
    sqlite3_finalize(stmt);
    return rc;
}

// Client operations
int bank_get_client_by_id(sqlite3 *db, const char *national_id, Client *client) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT * FROM Client WHERE national_id = ?;";
    int rc;

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        return rc;
    }

    sqlite3_bind_text(stmt, 1, national_id, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        strcpy(client->national_id, (const char *)sqlite3_column_text(stmt, 0));
        strcpy(client->first_name, (const char *)sqlite3_column_text(stmt, 1));
        strcpy(client->last_name, (const char *)sqlite3_column_text(stmt, 2));
        strcpy(client->account_type, (const char *)sqlite3_column_text(stmt, 3));
        client->balance = sqlite3_column_double(stmt, 4);
        
        // Fix PIN retrieval - only copy the first 4 characters
        const char *pin = (const char *)sqlite3_column_text(stmt, 5);
        strncpy(client->pin, pin, 4);
        client->pin[4] = '\0';  // Ensure null termination
        
        strcpy(client->phone, (const char *)sqlite3_column_text(stmt, 6));
        rc = SQLITE_OK;
    } else {
        rc = SQLITE_ERROR;
    }

    sqlite3_finalize(stmt);
    return rc;
}

int bank_update_client(sqlite3 *db, Client *client) {
    const char *sql = "UPDATE Client SET first_name = ?, last_name = ?, account_type = ?, balance = ?, pin = ?, phone = ? WHERE national_id = ?";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    
    if (rc == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, client->first_name, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, client->last_name, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, client->account_type, -1, SQLITE_STATIC);
        sqlite3_bind_double(stmt, 4, client->balance);
        sqlite3_bind_text(stmt, 5, client->pin, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 6, client->phone, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 7, client->national_id, -1, SQLITE_STATIC);
        rc = sqlite3_step(stmt);
    }
    
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? SQLITE_OK : rc;
}

int bank_delete_client(sqlite3 *db, const char *national_id) {
    const char *sql = "DELETE FROM Client WHERE national_id = ?";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    
    if (rc == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, national_id, -1, SQLITE_STATIC);
        rc = sqlite3_step(stmt);
    }
    
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? SQLITE_OK : rc;
}

int bank_list_clients(sqlite3 *db, Client **clients, int *count) {
    const char *sql = "SELECT * FROM Client";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    
    if (rc == SQLITE_OK) {
        // Count rows first
        *count = 0;
        while (sqlite3_step(stmt) == SQLITE_ROW) (*count)++;
        sqlite3_reset(stmt);
        
        // Allocate memory
        *clients = malloc(sizeof(Client) * (*count));
        if (*clients == NULL) return SQLITE_NOMEM;
        
        // Fetch data
        int i = 0;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Client *client = &(*clients)[i++];
            strncpy(client->national_id, (const char*)sqlite3_column_text(stmt, 0), sizeof(client->national_id)-1);
            strncpy(client->first_name, (const char*)sqlite3_column_text(stmt, 1), sizeof(client->first_name)-1);
            strncpy(client->last_name, (const char*)sqlite3_column_text(stmt, 2), sizeof(client->last_name)-1);
            strncpy(client->account_type, (const char*)sqlite3_column_text(stmt, 3), sizeof(client->account_type)-1);
            client->balance = sqlite3_column_double(stmt, 4);
            strncpy(client->pin, (const char*)sqlite3_column_text(stmt, 5), sizeof(client->pin)-1);
            strncpy(client->phone, (const char*)sqlite3_column_text(stmt, 6), sizeof(client->phone)-1);
        }
    }
    
    sqlite3_finalize(stmt);
    return rc;
}

int bank_verify_client_pin(sqlite3 *db, const char *national_id, const char *pin) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT pin FROM Client WHERE national_id = ?";
    char stored_pin[5];
    int rc;

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        return rc;
    }

    sqlite3_bind_text(stmt, 1, national_id, -1, SQLITE_STATIC);
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *db_pin = (const char *)sqlite3_column_text(stmt, 0);
        strncpy(stored_pin, db_pin, 4);
        stored_pin[4] = '\0';
        
        rc = (strcmp(stored_pin, pin) == 0) ? SQLITE_OK : SQLITE_ERROR;
    } else {
        rc = SQLITE_ERROR;
    }

    sqlite3_finalize(stmt);
    return rc;
}

// Credit card operations
int bank_add_credit_card(sqlite3 *db, const Credit_card *card) {
    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO Credit_card (card_number, expiry_date, holder_id, network, cvv) VALUES (?, ?, ?, ?, ?);";
    int rc;

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        return rc;
    }

    sqlite3_bind_text(stmt, 1, card->card_number, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, card->expiry_date, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, card->holder_id, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, card->network, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, card->cvv, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return (rc == SQLITE_DONE) ? SQLITE_OK : rc;
}

int bank_get_credit_card(sqlite3 *db, const char *card_number, Credit_card *card) {
    const char *sql = "SELECT * FROM Credit_card WHERE card_number = ?";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    
    if (rc == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, card_number, -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            strncpy(card->card_number, (const char*)sqlite3_column_text(stmt, 0), sizeof(card->card_number)-1);
            strncpy(card->expiry_date, (const char*)sqlite3_column_text(stmt, 1), sizeof(card->expiry_date)-1);
            strncpy(card->holder_id, (const char*)sqlite3_column_text(stmt, 2), sizeof(card->holder_id)-1);
            strncpy(card->network, (const char*)sqlite3_column_text(stmt, 3), sizeof(card->network)-1);
            strncpy(card->cvv, (const char*)sqlite3_column_text(stmt, 4), sizeof(card->cvv)-1);
            rc = SQLITE_OK;
        } else {
            rc = SQLITE_ERROR;
        }
    }
    
    sqlite3_finalize(stmt);
    return rc;
}

int bank_update_credit_card(sqlite3 *db, const Credit_card *card) {
    sqlite3_stmt *stmt;
    const char *sql = "UPDATE Credit_card SET expiry_date = ?, network = ?, cvv = ? WHERE card_number = ?;";
    int rc;

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        return rc;
    }

    sqlite3_bind_text(stmt, 1, card->expiry_date, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, card->network, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, card->cvv, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, card->card_number, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return (rc == SQLITE_DONE) ? SQLITE_OK : rc;
}

int bank_delete_credit_card(sqlite3 *db, const char *card_number) {
    const char *sql = "DELETE FROM Credit_card WHERE card_number = ?";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    
    if (rc == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, card_number, -1, SQLITE_STATIC);
        rc = sqlite3_step(stmt);
    }
    
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? SQLITE_OK : rc;
}

int bank_list_client_cards(sqlite3 *db, const char *national_id, Credit_card **cards, int *count) {
    const char *sql = "SELECT * FROM Credit_card WHERE holder_id = ?";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    
    if (rc == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, national_id, -1, SQLITE_STATIC);
        
        // Count rows first
        *count = 0;
        while (sqlite3_step(stmt) == SQLITE_ROW) (*count)++;
        sqlite3_reset(stmt);
        sqlite3_bind_text(stmt, 1, national_id, -1, SQLITE_STATIC);
        
        // Allocate memory
        *cards = malloc(sizeof(Credit_card) * (*count));
        if (*cards == NULL) return SQLITE_NOMEM;
        
        // Fetch data
        int i = 0;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Credit_card *card = &(*cards)[i++];
            strncpy(card->card_number, (const char*)sqlite3_column_text(stmt, 0), sizeof(card->card_number)-1);
            strncpy(card->expiry_date, (const char*)sqlite3_column_text(stmt, 1), sizeof(card->expiry_date)-1);
            strncpy(card->holder_id, (const char*)sqlite3_column_text(stmt, 2), sizeof(card->holder_id)-1);
            strncpy(card->network, (const char*)sqlite3_column_text(stmt, 3), sizeof(card->network)-1);
            strncpy(card->cvv, (const char*)sqlite3_column_text(stmt, 4), sizeof(card->cvv)-1);
        }
    }
    
    sqlite3_finalize(stmt);
    return rc;
}

// Additional Transaction operations
Bank_transaction* bank_get_all_transactions(sqlite3 *db, int *count) {
    const char *sql = "SELECT * FROM Bank_transaction ORDER BY date DESC";
    sqlite3_stmt *stmt;
    Bank_transaction *transactions = NULL;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        // Count rows first
        *count = 0;
        while (sqlite3_step(stmt) == SQLITE_ROW) (*count)++;
        sqlite3_reset(stmt);
        
        // Allocate memory
        transactions = malloc(sizeof(Bank_transaction) * (*count));
        if (transactions != NULL) {
            int i = 0;
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                Bank_transaction *trans = &transactions[i++];
                trans->id = sqlite3_column_int(stmt, 0);
                trans->employee_id = sqlite3_column_int(stmt, 1);
                strncpy(trans->client_id, (const char*)sqlite3_column_text(stmt, 2), sizeof(trans->client_id)-1);
                strncpy(trans->date, (const char*)sqlite3_column_text(stmt, 3), sizeof(trans->date)-1);
                strncpy(trans->type, (const char*)sqlite3_column_text(stmt, 4), sizeof(trans->type)-1);
                trans->amount = sqlite3_column_double(stmt, 5);
            }
        }
    }
    
    sqlite3_finalize(stmt);
    return transactions;
}

int bank_get_transaction_by_id(sqlite3 *db, int id, Bank_transaction *trans) {
    const char *sql = "SELECT * FROM Bank_transaction WHERE id = ?";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    
    if (rc == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            trans->id = sqlite3_column_int(stmt, 0);
            trans->employee_id = sqlite3_column_int(stmt, 1);
            strncpy(trans->client_id, (const char*)sqlite3_column_text(stmt, 2), sizeof(trans->client_id)-1);
            strncpy(trans->date, (const char*)sqlite3_column_text(stmt, 3), sizeof(trans->date)-1);
            strncpy(trans->type, (const char*)sqlite3_column_text(stmt, 4), sizeof(trans->type)-1);
            trans->amount = sqlite3_column_double(stmt, 5);
            rc = SQLITE_OK;
        } else {
            rc = SQLITE_ERROR;
        }
    }
    
    sqlite3_finalize(stmt);
    return rc;
}

int bank_get_transactions_by_date_range(sqlite3 *db, const char *start_date, const char *end_date, Bank_transaction **trans, int *count) {
    const char *sql = "SELECT * FROM Bank_transaction WHERE date BETWEEN ? AND ? ORDER BY date DESC";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    
    if (rc == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, start_date, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, end_date, -1, SQLITE_STATIC);
        
        // Count rows first
        *count = 0;
        while (sqlite3_step(stmt) == SQLITE_ROW) (*count)++;
        sqlite3_reset(stmt);
        sqlite3_bind_text(stmt, 1, start_date, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, end_date, -1, SQLITE_STATIC);
        
        // Allocate memory
        *trans = malloc(sizeof(Bank_transaction) * (*count));
        if (*trans == NULL) return SQLITE_NOMEM;
        
        // Fetch data
        int i = 0;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Bank_transaction *t = &(*trans)[i++];
            t->id = sqlite3_column_int(stmt, 0);
            t->employee_id = sqlite3_column_int(stmt, 1);
            strncpy(t->client_id, (const char*)sqlite3_column_text(stmt, 2), sizeof(t->client_id)-1);
            strncpy(t->date, (const char*)sqlite3_column_text(stmt, 3), sizeof(t->date)-1);
            strncpy(t->type, (const char*)sqlite3_column_text(stmt, 4), sizeof(t->type)-1);
            t->amount = sqlite3_column_double(stmt, 5);
        }
    }
    
    sqlite3_finalize(stmt);
    return rc;
}

int bank_get_transactions_by_type(sqlite3 *db, const char *type, Bank_transaction **trans, int *count) {
    const char *sql = "SELECT * FROM Bank_transaction WHERE type = ? ORDER BY date DESC";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    
    if (rc == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, type, -1, SQLITE_STATIC);
        
        // Count rows first
        *count = 0;
        while (sqlite3_step(stmt) == SQLITE_ROW) (*count)++;
        sqlite3_reset(stmt);
        sqlite3_bind_text(stmt, 1, type, -1, SQLITE_STATIC);
        
        // Allocate memory
        *trans = malloc(sizeof(Bank_transaction) * (*count));
        if (*trans == NULL) return SQLITE_NOMEM;
        
        // Fetch data
        int i = 0;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Bank_transaction *t = &(*trans)[i++];
            t->id = sqlite3_column_int(stmt, 0);
            t->employee_id = sqlite3_column_int(stmt, 1);
            strncpy(t->client_id, (const char*)sqlite3_column_text(stmt, 2), sizeof(t->client_id)-1);
            strncpy(t->date, (const char*)sqlite3_column_text(stmt, 3), sizeof(t->date)-1);
            strncpy(t->type, (const char*)sqlite3_column_text(stmt, 4), sizeof(t->type)-1);
            t->amount = sqlite3_column_double(stmt, 5);
        }
    }
    
    sqlite3_finalize(stmt);
    return rc;
}

int bank_add_client(sqlite3 *db, Client *client, const char *pin) {
    const char *query = "INSERT INTO Client (national_id, first_name, last_name, account_type, balance, pin, phone) "
                       "VALUES (?, ?, ?, ?, ?, ?, ?)";
    sqlite3_stmt *stmt;
    int rc;

    rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        return rc;
    }

    sqlite3_bind_text(stmt, 1, client->national_id, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, client->first_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, client->last_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, client->account_type, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 5, client->balance);
    sqlite3_bind_text(stmt, 6, pin, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 7, client->phone, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return (rc == SQLITE_DONE) ? SQLITE_OK : rc;
}
