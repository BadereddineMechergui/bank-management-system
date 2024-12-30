#include "ascii_art.h"
#include "cursor_position.h"
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#endif

#define CONSOLE_WIDTH 80
#define MAX_LINE_LENGTH 1024
#define PROJECT_PATH "c:/Users/mecba/Desktop/cs50-project"

// Function to clear the screen
void clear_screen(void) {
    clrscr();
}

// Function to print centered text
void print_centered(const char* text) {
    int padding = (CONSOLE_WIDTH - strlen(text)) / 2;
    if (padding > 0) {
        printf("%*s", padding, "");
    }
    printf("%s\n", text);
}

// Function to print a line of characters
void print_line(char c, int width) {
    for (int i = 0; i < width; i++) {
        putchar(c);
    }
    putchar('\n');
}

// Function to display ASCII art from a file
void display_ascii_art(const char* filename) {
    char filepath[256];
    snprintf(filepath, sizeof(filepath), "%s/ascii-art/%s", PROJECT_PATH, filename);
    
    FILE* file = fopen(filepath, "r");
    if (file == NULL) {
        printf("Error: Could not open file %s\n", filepath);
        return;
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }

    fclose(file);
}

// Function to get current date and time
date get_current_date(void) {
    date d;
    time_t t = time(0);
    d.time = ctime(&t);
    // Remove newline character
    d.time[strlen(d.time) - 1] = '\0';
    return d;
}

// Function to display date
void display_date(date d) {
    printf("%s", d.time);
}

// Function to display welcome screen
void display_welcome(void) {
    clear_screen();
    display_ascii_art("welcome.txt");
}

// Function to display home screen
void display_home(void) {
    clear_screen();
    display_ascii_art("home.txt");
    date d = get_current_date();
    gotoxy(127,4);
    display_date(d);
}

// Function to display menu and get user choice
int display_menu(void) {
    int choice;
    
    display_home();
    printf("\n");
    print_line('=', CONSOLE_WIDTH);
    print_centered("BANK MANAGEMENT SYSTEM");
    print_line('=', CONSOLE_WIDTH);
    printf("\n");
    
    print_centered("1. Employee Login");
    print_centered("2. Client Login");
    print_centered("3. Register New Client");
    print_centered("4. About");
    print_centered("5. Exit");
    printf("\n");
    
    print_line('-', CONSOLE_WIDTH);
    print_centered("Enter your choice (1-5): ");
    
    scanf("%d", &choice);
    while(getchar() != '\n'); // Clear input buffer
    
    return choice;
}

void dalay(int j)
{   int k =0;
    for(int i = 0;i < j;i++)
        k = i;
}