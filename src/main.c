#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "ascii_art.h"
#include "bank_manager.h"
#include "client.h"
#include "cursor_position.h"
#include "employee.h"

int main() {
    sqlite3 *db;
    int rc;

    // Initialize database
    rc = bank_init_database(&db, "bank.db");
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database\n");
        return 1;
    }

    // Display welcome screen
    getch();
    clrscr();
    display_welcome();
    
    // Wait for input
    int choice;
    do{

        gotoxy(65, 31);
        scanf("%d",&choice);
    
    } while (choice != 1 && choice != 2);

    if(choice == 1) handle_employee_login(db);
    else handle_client_login(db);
    
    // Clean up and exit
    sqlite3_close(db);
    return 0;
}