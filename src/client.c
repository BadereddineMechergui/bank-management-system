#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "client.h"
#include "cursor_position.h"
#include "ascii_art.h"
#include "bank_manager.h"
#include "employee.h"

void display_client_menu(void) {
    clrscr();
    display_home();
    printf("\n\n\n\n\n\n\n\t\t\t\t\t\t\t\t\xB2\xB2\xB2\xB2\xB2\xB2\xB2 CLIENT MENU \xB2\xB2\xB2\xB2\xB2\xB2\xB2\n");
    printf("\n\n\t\t\t\t1. View Account Information");
    printf("\n\t\t\t\t2. Make a Transaction");
    printf("\n\t\t\t\t3. View Transaction History");
    printf("\n\t\t\t\t4. Update Personal Information");
    printf("\n\t\t\t\t5. View Credit Cards");
    printf("\n\t\t\t\t6. Logout");
    printf("\n\n\t\t\t\tEnter your choice (1-6): ");
}

void view_account_info(sqlite3 *db, const char *national_id) {
    Client client;
    
    clrscr();
    display_home();
    printf("\n\n\n\n\n\n\n\t\t\t\t\t\t\t\t\xB2\xB2\xB2\xB2\xB2\xB2\xB2 ACCOUNT INFORMATION \xB2\xB2\xB2\xB2\xB2\xB2\xB2\n");
    
    if (bank_get_client_by_id(db, national_id, &client) == SQLITE_OK) {
        printf("\n\t\t\t\tAccount Details:");
        printf("\n\t\t\t\t----------------");
        printf("\n\t\t\t\tName: %s %s", client.first_name, client.last_name);
        printf("\n\t\t\t\tNational ID: %s", client.national_id);
        printf("\n\t\t\t\tAccount Type: %s", client.account_type);
        printf("\n\t\t\t\tBalance: $%.2f", client.balance);
        printf("\n\t\t\t\tPhone: %s", client.phone);
    } else {
        printf("\n\t\t\t\tError retrieving account information!");
    }
    
    printf("\n\n\t\t\t\tPress any key to continue...");
    getch();
}

void make_transaction(sqlite3 *db, const char *national_id) {
    Client client;
    char type[20];
    double amount;
    Bank_transaction trans;
    
    clrscr();
    display_home();
    printf("\n\n\n\n\n\n\n\t\t\t\t\t\t\t\t\xB2\xB2\xB2\xB2\xB2\xB2\xB2 MAKE TRANSACTION \xB2\xB2\xB2\xB2\xB2\xB2\xB2\n");
    
    if (bank_get_client_by_id(db, national_id, &client) != SQLITE_OK) {
        printf("\n\t\t\t\tError retrieving account information!");
        printf("\n\n\t\t\t\tPress any key to continue...");
        getch();
        return;
    }
    
    printf("\n\t\t\t\tCurrent Balance: $%.2f", client.balance);
    printf("\n\n\t\t\t\tTransaction type (deposit/withdraw): ");
    scanf("%19s", type);
    while(getchar() != '\n');
    
    printf("\t\t\t\tAmount: $");
    scanf("%lf", &amount);
    while(getchar() != '\n');
    
    if (strcmp(type, "withdraw") == 0 && amount > client.balance) {
        printf("\n\t\t\t\tInsufficient funds!");
        printf("\n\n\t\t\t\tPress any key to continue...");
        getch();
        return;
    }
    
    double change = (strcmp(type, "deposit") == 0) ? amount : -amount;
    if (bank_update_client_balance(db, national_id, change) == SQLITE_OK) {
        strcpy(trans.client_id, national_id);
        trans.employee_id = 1;
        strcpy(trans.type, type);
        trans.amount = amount;
        
        if (bank_record_transaction(db, &trans) == SQLITE_OK) {
            printf("\n\t\t\t\tTransaction completed successfully!");
            printf("\n\t\t\t\tNew balance: $%.2f", client.balance + change);
        } else {
            printf("\n\t\t\t\tFailed to record transaction!");
        }
    } else {
        printf("\n\t\t\t\tTransaction failed!");
    }
    
    printf("\n\n\t\t\t\tPress any key to continue...");
    getch();
}

void view_transaction_history(sqlite3 *db, const char *national_id) {
    int count;
    Bank_transaction *transactions;
    Client client;
    
    clrscr();
    display_home();
    printf("\n\n\n\n\n\n\n\t\t\t\t\t\t\t\t\xB2\xB2\xB2\xB2\xB2\xB2\xB2 TRANSACTION HISTORY \xB2\xB2\xB2\xB2\xB2\xB2\xB2\n");
    
    if (bank_get_client_by_id(db, national_id, &client) == SQLITE_OK) {
        printf("\n\t\t\t\tAccount: %s %s", client.first_name, client.last_name);
        printf("\n\t\t\t\tCurrent Balance: $%.2f", client.balance);
        
        transactions = bank_get_client_transaction_history(db, national_id, &count);
        
        if (transactions != NULL && count > 0) {
            printf("\n\n\t\t\t\tRecent Transactions:");
            printf("\n\t\t\t\t-------------------");
            
            for (int i = 0; i < count; i++) {
                printf("\n\t\t\t\tDate: %s", transactions[i].date);
                printf("\n\t\t\t\tType: %s", transactions[i].type);
                printf("\n\t\t\t\tAmount: $%.2f\n", transactions[i].amount);
            }
            
            free(transactions);
        } else {
            printf("\n\n\t\t\t\tNo transactions found.");
        }
    } else {
        printf("\n\t\t\t\tError retrieving account information!");
    }
    
    printf("\n\n\t\t\t\tPress any key to continue...");
    getch();
}

void update_personal_info(sqlite3 *db, const char *national_id) {
    Client client;
    char choice;
    
    clrscr();
    display_home();
    printf("\n\n\n\n\n\n\n\t\t\t\t\t\t\t\t\xB2\xB2\xB2\xB2\xB2\xB2\xB2 UPDATE INFORMATION \xB2\xB2\xB2\xB2\xB2\xB2\xB2\n");
    
    if (bank_get_client_by_id(db, national_id, &client) != SQLITE_OK) {
        printf("\n\t\t\t\tError retrieving account information!");
        printf("\n\n\t\t\t\tPress any key to continue...");
        getch();
        return;
    }
    
    printf("\n\t\t\t\tCurrent Details:");
    printf("\n\t\t\t\t----------------");
    printf("\n\t\t\t\tName: %s %s", client.first_name, client.last_name);
    printf("\n\t\t\t\tPhone: %s", client.phone);
    
    printf("\n\n\t\t\t\tUpdate information (y/n)? ");
    choice = getchar();
    while(getchar() != '\n');
    
    if (choice == 'y' || choice == 'Y') {
        char input[50];
        
        printf("\n\t\t\t\tEnter new details (press Enter to keep current):");
        
        printf("\n\t\t\t\tFirst Name: ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0;
        if (strlen(input) > 0) {
            strncpy(client.first_name, input, sizeof(client.first_name) - 1);
        }
        
        printf("\t\t\t\tLast Name: ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0;
        if (strlen(input) > 0) {
            strncpy(client.last_name, input, sizeof(client.last_name) - 1);
        }
        
        printf("\t\t\t\tPhone: ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0;
        if (strlen(input) > 0) {
            strncpy(client.phone, input, sizeof(client.phone) - 1);
        }
        
        if (bank_update_client(db, &client) == SQLITE_OK) {
            printf("\n\t\t\t\tInformation updated successfully!");
        } else {
            printf("\n\t\t\t\tFailed to update information!");
        }
    }
    
    printf("\n\n\t\t\t\tPress any key to continue...");
    getch();
}

void view_credit_cards(sqlite3 *db, const char *national_id) {
    Credit_card *cards;
    int card_count;
    Client client;
    char cvv[4];  // Buffer for CVV (3 digits + null terminator)
    
    clrscr();
    display_home();
    printf("\n\n\n\n\n\n\n\t\t\t\t\t\t\t\t\xB2\xB2\xB2\xB2\xB2\xB2\xB2 CREDIT CARDS \xB2\xB2\xB2\xB2\xB2\xB2\xB2\n");
    
    // Get client information for balance
    if (bank_get_client_by_id(db, national_id, &client) != SQLITE_OK) {
        printf("\n\t\t\t\tError retrieving client information!");
        printf("\n\n\t\t\t\tPress any key to continue...");
        getch();
        return;
    }
    
    if (bank_list_client_cards(db, national_id, &cards, &card_count) == SQLITE_OK) {
        if (card_count > 0) {
            for (int i = 0; i < card_count; i++) {
                // Extract only the first 3 characters of CVV
                strncpy(cvv, cards[i].cvv, 3);
                cvv[3] = '\0';
                
                printf("\n\t\t\t\t+----------------------------------+");
                printf("\n\t\t\t\t| %-32s |", cards[i].network);
                printf("\n\t\t\t\t| Card Number: %-19s |", cards[i].card_number);
                printf("\n\t\t\t\t| Exp: %-7s    CVV: %-3s         |", 
                       cards[i].expiry_date, cvv);
                printf("\n\t\t\t\t| Balance: $%-23.2f |", client.balance);
                printf("\n\t\t\t\t+----------------------------------+\n");
            }
            free(cards);
        } else {
            printf("\n\t\t\t\tNo credit cards found.");
        }
    } else {
        printf("\n\t\t\t\tError retrieving credit card information!");
    }
    
    printf("\n\n\t\t\t\tPress any key to continue...");
    getch();
}

void handle_client_login(sqlite3 *db) {
    char national_id[20];
    char pin[5];  
    Client client;
    
    clrscr();
    display_home();
    printf("\n\n\n\n\n\n\n\t\t\t\t\t\t\t\t\xB2\xB2\xB2\xB2\xB2\xB2\xB2 CLIENT LOGIN \xB2\xB2\xB2\xB2\xB2\xB2\xB2\n");
    
    printf("\n\t\t\t\tNational ID: ");
    scanf("%19s", national_id);
    while(getchar() != '\n');
    
    printf("\t\t\t\tPIN: ");
    scanf("%4s", pin);  
    while(getchar() != '\n');
    
    if (bank_verify_client_pin(db, national_id, pin) == SQLITE_OK) {
        handle_client_menu(db, national_id);
    } else {
        printf("\n\t\t\t\tInvalid National ID or PIN!");
        printf("\n\n\t\t\t\tPress any key to continue...");
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
        }
}

void handle_client_menu(sqlite3 *db, const char *national_id) {
    int choice;
    
    do {
        clrscr();
        display_home();
        printf("\n\n\n\n\n\n\n\t\t\t\t\t\t\t\t\xB2\xB2\xB2\xB2\xB2\xB2\xB2 CLIENT MENU \xB2\xB2\xB2\xB2\xB2\xB2\xB2\n");
        printf("\n\n\t\t\t\t1. View Account Information");
        printf("\n\t\t\t\t2. Make a Transaction");
        printf("\n\t\t\t\t3. View Transaction History");
        printf("\n\t\t\t\t4. Update Personal Information");
        printf("\n\t\t\t\t5. View Credit Cards");
        printf("\n\t\t\t\t6. Logout");
        printf("\n\n\t\t\t\tEnter your choice (1-6): ");
        
        scanf("%d", &choice);
        while(getchar() != '\n');
        
        switch (choice) {
            case 1:
                view_account_info(db, national_id);
                break;
            case 2:
                make_transaction(db, national_id);
                break;
            case 3:
                view_transaction_history(db, national_id);
                break;
            case 4:
                update_personal_info(db, national_id);
                break;
            case 5:
                view_credit_cards(db, national_id);
                break;
            case 6:
                printf("\n\n\t\t\t\tLogging out...");
                clrscr();
                display_welcome();
                
                int choice;
                do {
                    gotoxy(65, 31);
                    scanf("%d", &choice);
                } while (choice != 1 && choice != 2);
                
                if (choice == 1) handle_employee_login(db);
                else handle_client_login(db);
                break;
            default:
                printf("\n\n\t\t\t\tInvalid choice! Press any key to continue...");
                getch();
        }
    } while (choice != 6);
}
