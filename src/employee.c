#include <stdio.h>
#include <string.h>
#include "employee.h"
#include "cursor_position.h"
#include "ascii_art.h"
#include "client.h"

void display_employee_menu(void) {
    clrscr();
    display_home();
    printf("\n\n\n\n\n\n\n\t\t\t\t\t\t\t\t\xB2\xB2\xB2\xB2\xB2\xB2\xB2 EMPLOYEE MENU \xB2\xB2\xB2\xB2\xB2\xB2\xB2\n"); 
    printf("\n\n\t\t\t\t1. View Client Information");
    printf("\n\n\t\t\t\t2. Perform Transaction");
    printf("\n\n\t\t\t\t3. View Transaction History");
    printf("\n\n\t\t\t\t4. Add New Client");
    printf("\n\n\t\t\t\t5. Modify Client");
    printf("\n\n\t\t\t\t6. Transfer Between Clients");
    printf("\n\n\t\t\t\t7. Logout");
    printf("\n");
    // print_line('-', 80);
    printf("\n\n\t\t\t\tEnter your choice (1-7): ");
}

void handle_employee_login(sqlite3 *db) {
    char username[50];
    char password[100];
    
    clrscr();
    display_home();
    printf("\n\n\n\n\n\n\n\t\t\t\t\t\t\t\t\xB2\xB2\xB2\xB2\xB2\xB2\xB2 EMPLOYEE LOGIN \xB2\xB2\xB2\xB2\xB2\xB2\xB2\n"); 

    
    printf("\n\n\n\n\t\t\t\tEnter username: ");
    scanf("%49s", username);
    while(getchar() != '\n');
    
    printf("\n\n\t\t\t\tEnter password: ");
    scanf("%99s", password);
    while(getchar() != '\n');
    
    if (bank_verify_employee_credentials(db, username, password) == SQLITE_OK) {
        printf("Loading data");
        for(int i = 0;i <= 6;i++)
        {
            dalay(100000000);
            printf(". ");
        }
        printf("\n");
        printf("Appuyez sur une touche pour continuer ...");
        getch();
        handle_employee_menu(db);
    } else {
        printf("Invalid credentials!");

        //recursive fuction:  call itself when the input data  is invalid   
        char select;
        printf("do you want to try again? (y/n): ");
        scanf("%c", &select);
        if (select == 'y' || select == 'Y')
        {
            handle_employee_login(db);
        }

        else if(select == 'n' || select == 'N')
        {
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
}

void view_client_information(sqlite3 *db) {
    Client client;
    Credit_card *cards;
    int card_count;
    char national_id[20];
    Bank_transaction *transactions;
    int trans_count;
    
    clrscr();
    display_home();
    printf("\n\n\n\n\n\n\n\t\t\t\t\t\t\t\t\xB2\xB2\xB2\xB2\xB2\xB2\xB2 CLIENT INFORMATION \xB2\xB2\xB2\xB2\xB2\xB2\xB2\n");
    
    printf("\n\t\t\t\tEnter client National ID: ");
    scanf("%19s", national_id);
    while(getchar() != '\n');
    
    if (bank_get_client_by_id(db, national_id, &client) == SQLITE_OK) {
        printf("\n\t\t\t\tClient Details:");
        printf("\n\t\t\t\t--------------");
        printf("\n\t\t\t\tName: %s %s", client.first_name, client.last_name);
        printf("\n\t\t\t\tAccount Type: %s", client.account_type);
        printf("\n\t\t\t\tBalance: $%.2f", client.balance);
        printf("\n\t\t\t\tPhone: %s", client.phone);
        printf("\n\t\t\t\tPIN: %s", client.pin);
        
        // Show credit cards
        if (bank_list_client_cards(db, national_id, &cards, &card_count) == SQLITE_OK) {
            if (card_count > 0) {
                printf("\n\n\t\t\t\tCredit Cards:");
                printf("\n\t\t\t\t------------");
                for (int i = 0; i < card_count; i++) {
                    printf("\n\t\t\t\tCard Number: %s", cards[i].card_number);
                    printf("\n\t\t\t\tExpiry Date: %s", cards[i].expiry_date);
                    printf("\n\t\t\t\tNetwork: %s", cards[i].network);
                    printf("\n\t\t\t\tCVV: %s\n", cards[i].cvv);
                }
                free(cards);
            }
        }
        
        // Show transaction history
        transactions = bank_get_client_transaction_history(db, national_id, &trans_count);
        if (transactions != NULL && trans_count > 0) {
            printf("\n\t\t\t\tTransaction History:");
            printf("\n\t\t\t\t-------------------");
            
            for (int i = 0; i < trans_count; i++) {
                printf("\n\t\t\t\tDate: %s", transactions[i].date);
                printf("\n\t\t\t\tType: %s", transactions[i].type);
                printf("\n\t\t\t\tAmount: $%.2f\n", transactions[i].amount);
            }
            free(transactions);
        } else {
            printf("\n\n\t\t\t\tNo transaction history found.");
        }
    } else {
        printf("\n\t\t\t\tClient not found!");
    }
    
    printf("\n\n\t\t\t\tPress any key to continue...");
    getch();
}

void perform_transaction(sqlite3 *db) {
    char national_id[20];
    char type[20];
    double amount;
    Client client;
    Bank_transaction trans;
    
    clrscr();
    display_home();
    printf("\n\n\n\n\n\n\n\t\t\t\t\t\t\t\t\xB2\xB2\xB2\xB2\xB2\xB2\xB2 PERFORM TRANSACTION \xB2\xB2\xB2\xB2\xB2\xB2\xB2\n");
    
    printf("\n\n\t\t\t\tEnter client National ID: ");
    scanf("%19s", national_id);
    while(getchar() != '\n');
    
    // First verify client exists and get current balance
    if (bank_get_client_by_id(db, national_id, &client) != SQLITE_OK) {
        printf("\n\t\t\t\tClient not found!");
        printf("\n\n\t\t\t\tPress any key to continue...");
        getch();
        return;
    }
    
    printf("\n\t\t\t\tTransaction type (deposit/withdraw): ");
    scanf("%19s", type);
    while(getchar() != '\n');
    
    printf("\n\t\t\t\tAmount: $");
    scanf("%lf", &amount);
    while(getchar() != '\n');
    
    // Validate withdrawal amount
    if (strcmp(type, "withdraw") == 0 && amount > client.balance) {
        printf("\n\t\t\t\tInsufficient funds! Current balance: $%.2f", client.balance);
        printf("\n\n\t\t\t\tPress any key to continue...");
        getch();
        return;
    }
    
    // Update balance
    double change = (strcmp(type, "deposit") == 0) ? amount : -amount;
    if (bank_update_client_balance(db, national_id, change) == SQLITE_OK) {
        // Record transaction
        strcpy(trans.client_id, national_id);
        trans.employee_id = 1; // Using default employee ID
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

void view_all_transactions(sqlite3 *db) {
    int count;
    Bank_transaction *transactions;
    
    clrscr();
    display_home();
    printf("\n\n\n\n\n\n\n\t\t\t\t\t\t\t\t\xB2\xB2\xB2\xB2\xB2\xB2\xB2 ALL TRANSACTIONS \xB2\xB2\xB2\xB2\xB2\xB2\xB2\n");
    
    transactions = bank_get_all_transactions(db, &count);
    
    if (transactions != NULL && count > 0) {
        printf("\n\t\t\t\tRecent Transactions:");
        printf("\n\t\t\t\t-------------------");
        
        for (int i = 0; i < count; i++) {
            printf("\n\t\t\t\tClient ID: %s", transactions[i].client_id);
            printf("\n\t\t\t\tDate: %s", transactions[i].date);
            printf("\n\t\t\t\tType: %s", transactions[i].type);
            printf("\n\t\t\t\tAmount: $%.2f\n", transactions[i].amount);
        }
        
        free(transactions);
    } else {
        printf("\n\t\t\t\tNo transactions found.");
    }
    
    printf("\n\n\t\t\t\tPress any key to continue...");
    getch();
}

void add_new_client(sqlite3 *db) {
    Client client;
    Credit_card card;
    char pin[7];
    
    clrscr();
    display_home();
    printf("\n\n\n\n\n\n\n\t\t\t\t\t\t\t\t\xB2\xB2\xB2\xB2\xB2\xB2\xB2 NEW CLIENT REGISTRATION \xB2\xB2\xB2\xB2\xB2\xB2\xB2\n");
    
    // Get client details
    printf("\n\t\t\t\tEnter Client Details:");
    printf("\n\t\t\t\t--------------------");
    printf("\n\t\t\t\tNational ID: ");
    scanf("%19s", client.national_id);
    while(getchar() != '\n');
    
    printf("\t\t\t\tFirst Name: ");
    scanf("%49s", client.first_name);
    while(getchar() != '\n');
    
    printf("\t\t\t\tLast Name: ");
    scanf("%49s", client.last_name);
    while(getchar() != '\n');
    
    printf("\t\t\t\tPhone: ");
    scanf("%14s", client.phone);
    while(getchar() != '\n');
    
    printf("\t\t\t\tAccount Type (savings/checking): ");
    scanf("%19s", client.account_type);
    while(getchar() != '\n');
    
    printf("\t\t\t\tInitial Balance: $");
    scanf("%lf", &client.balance);
    while(getchar() != '\n');
    
    printf("\t\t\t\tPIN (6 digits): ");
    scanf("%6s", pin);
    while(getchar() != '\n');
    
    // Add client to database
    if (bank_add_client(db, &client, pin) == SQLITE_OK) {
        printf("\n\t\t\t\tClient added successfully!");
        
        // Get credit card details
        printf("\n\n\t\t\t\tEnter Credit Card Details:");
        printf("\n\t\t\t\t------------------------");
        
        strcpy(card.holder_id, client.national_id);
        
        printf("\n\t\t\t\tCard Number (16 digits): ");
        scanf("%16s", card.card_number);
        while(getchar() != '\n');
        
        printf("\t\t\t\tExpiry Date (MM/YY): ");
        scanf("%5s", card.expiry_date);
        while(getchar() != '\n');
        
        printf("\t\t\t\tCVV (3 digits): ");
        scanf("%3s", card.cvv);
        while(getchar() != '\n');
        
        printf("\t\t\t\tNetwork (visa/mastercard): ");
        scanf("%19s", card.network);
        while(getchar() != '\n');
        
        // Add credit card to database
        if (bank_add_credit_card(db, &card) == SQLITE_OK) {
            printf("\n\t\t\t\tCredit card added successfully!");
        } else {
            printf("\n\t\t\t\tFailed to add credit card!");
        }
    } else {
        printf("\n\t\t\t\tFailed to add client! National ID might already exist.");
    }
    
    printf("\n\n\t\t\t\tPress any key to continue...");
    getch();
}

void modify_client(sqlite3 *db) {
    Client client;
    char national_id[20];
    char choice;
    
    clrscr();
    display_home();
    printf("\n\n\n\n\n\n\n\t\t\t\t\t\t\t\t\xB2\xB2\xB2\xB2\xB2\xB2\xB2 MODIFY CLIENT \xB2\xB2\xB2\xB2\xB2\xB2\xB2\n");
    
    printf("\n\t\t\t\tEnter Client National ID: ");
    scanf("%19s", national_id);
    while(getchar() != '\n');
    
    if (bank_get_client_by_id(db, national_id, &client) != SQLITE_OK) {
        printf("\n\t\t\t\tClient not found!");
        printf("\n\n\t\t\t\tPress any key to continue...");
        getch();
        return;
    }
    
    printf("\n\t\t\t\tCurrent Client Details:");
    printf("\n\t\t\t\t----------------------");
    printf("\n\t\t\t\tName: %s %s", client.first_name, client.last_name);
    printf("\n\t\t\t\tPhone: %s", client.phone);
    printf("\n\t\t\t\tAccount Type: %s", client.account_type);
    printf("\n\t\t\t\tBalance: $%.2f", client.balance);
    
    printf("\n\n\t\t\t\tModify (y/n)? ");
    choice = getchar();
    while(getchar() != '\n');
    
    if (choice == 'y' || choice == 'Y') {
        printf("\n\t\t\t\tEnter new details (press Enter to keep current value):");
        
        printf("\n\t\t\t\tFirst Name: ");
        char input[50];
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
        
        printf("\t\t\t\tAccount Type (savings/checking): ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0;
        if (strlen(input) > 0) {
            strncpy(client.account_type, input, sizeof(client.account_type) - 1);
        }
        
        if (bank_update_client(db, &client) == SQLITE_OK) {
            printf("\n\t\t\t\tClient information updated successfully!");
        } else {
            printf("\n\t\t\t\tFailed to update client information!");
        }
    }
    
    printf("\n\n\t\t\t\tPress any key to continue...");
    getch();
}

void transfer_between_clients(sqlite3 *db) {
    char from_id[20], to_id[20];
    Client from_client, to_client;
    double amount;
    Bank_transaction trans;
    
    clrscr();
    display_home();
    printf("\n\n\n\n\n\n\n\t\t\t\t\t\t\t\t\xB2\xB2\xB2\xB2\xB2\xB2\xB2 CLIENT TRANSFER \xB2\xB2\xB2\xB2\xB2\xB2\xB2\n");
    
    // Get sender details
    printf("\n\t\t\t\tEnter Sender's National ID: ");
    scanf("%19s", from_id);
    while(getchar() != '\n');
    
    if (bank_get_client_by_id(db, from_id, &from_client) != SQLITE_OK) {
        printf("\n\t\t\t\tSender not found!");
        printf("\n\n\t\t\t\tPress any key to continue...");
        getch();
        return;
    }
    
    // Get receiver details
    printf("\n\t\t\t\tEnter Receiver's National ID: ");
    scanf("%19s", to_id);
    while(getchar() != '\n');
    
    if (bank_get_client_by_id(db, to_id, &to_client) != SQLITE_OK) {
        printf("\n\t\t\t\tReceiver not found!");
        printf("\n\n\t\t\t\tPress any key to continue...");
        getch();
        return;
    }
    
    // Show sender's current balance
    printf("\n\t\t\t\tSender: %s %s", from_client.first_name, from_client.last_name);
    printf("\n\t\t\t\tCurrent Balance: $%.2f", from_client.balance);
    
    // Get transfer amount
    printf("\n\n\t\t\t\tEnter Transfer Amount: $");
    scanf("%lf", &amount);
    while(getchar() != '\n');
    
    // Validate amount
    if (amount <= 0) {
        printf("\n\t\t\t\tInvalid amount! Amount must be positive.");
        printf("\n\n\t\t\t\tPress any key to continue...");
        getch();
        return;
    }
    
    if (amount > from_client.balance) {
        printf("\n\t\t\t\tInsufficient funds!");
        printf("\n\n\t\t\t\tPress any key to continue...");
        getch();
        return;
    }
    
    // Perform transfer
    if (bank_update_client_balance(db, from_id, -amount) == SQLITE_OK) {
        if (bank_update_client_balance(db, to_id, amount) == SQLITE_OK) {
            // Record withdrawal transaction
            strcpy(trans.client_id, from_id);
            trans.employee_id = 1;
            strcpy(trans.type, "transfer_out");
            trans.amount = amount;
            bank_record_transaction(db, &trans);
            
            // Record deposit transaction
            strcpy(trans.client_id, to_id);
            strcpy(trans.type, "transfer_in");
            bank_record_transaction(db, &trans);
            
            printf("\n\t\t\t\tTransfer completed successfully!");
            printf("\n\t\t\t\tNew balance for %s %s: $%.2f", 
                   from_client.first_name, from_client.last_name, 
                   from_client.balance - amount);
        } else {
            // Rollback the withdrawal if deposit fails
            bank_update_client_balance(db, from_id, amount);
            printf("\n\t\t\t\tTransfer failed! Error updating receiver's balance.");
        }
    } else {
        printf("\n\t\t\t\tTransfer failed! Error updating sender's balance.");
    }
    
    printf("\n\n\t\t\t\tPress any key to continue...");
    getch();
}

void handle_employee_menu(sqlite3 *db) {
    int choice;
    
    do {
        clrscr();
        display_home();
        printf("\n\n\n\n\n\n\n\t\t\t\t\t\t\t\t\xB2\xB2\xB2\xB2\xB2\xB2\xB2 EMPLOYEE MENU \xB2\xB2\xB2\xB2\xB2\xB2\xB2\n");
        printf("\n\n\t\t\t\t1. View Client Information");
        printf("\n\t\t\t\t2. Perform Transaction");
        printf("\n\t\t\t\t3. View Transaction History");
        printf("\n\t\t\t\t4. Add New Client");
        printf("\n\t\t\t\t5. Modify Client");
        printf("\n\t\t\t\t6. Transfer Between Clients");
        printf("\n\t\t\t\t7. Logout");
        printf("\n\n\t\t\t\tEnter your choice (1-7): ");
        
        scanf("%d", &choice);
        while(getchar() != '\n');
        
        switch (choice) {
            case 1:
                view_client_information(db);
                break;
            case 2:
                perform_transaction(db);
                break;
            case 3:
                view_all_transactions(db);
                break;
            case 4:
                add_new_client(db);
                break;
            case 5:
                modify_client(db);
                break;
            case 6:
                transfer_between_clients(db);
                break;
            case 7:
                printf("\n\n\t\t\t\tLogging out...");
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
                break;
            default:
                printf("\n\n\t\t\t\tInvalid choice! Press any key to continue...");
                getch();
        }
    } while (choice != 7);
}
