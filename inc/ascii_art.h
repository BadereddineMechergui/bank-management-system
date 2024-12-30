#ifndef ASCII_ART_H
#define ASCII_ART_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Date structure for displaying current time
typedef struct {
    char *time;
} date;

// Function prototypes
void print_centered(const char* text);
void print_line(char c, int width);
void display_ascii_art(const char* filename);
int display_menu(void);
date get_current_date(void);
void display_date(date d);
void display_welcome(void);
void display_home(void);
void dalay(int j);

#endif // ASCII_ART_H
