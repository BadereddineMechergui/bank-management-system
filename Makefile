CC=gcc
CFLAGS=-Wall -g

C_INCLUDE_PATH=-Iinc
BUILDDIR=build
SRCDIR=src
INCDIR=inc

main: main.o bank_manager.o ascii_art.o employee.o client.o cursor_position.o
	$(CC) $(C_INCLUDE_PATH) $(CFLAGS) $(BUILDDIR)/*.o -o $(BUILDDIR)/main -lsqlite3
main.o: $(SRCDIR)/main.c 
	$(CC) $(C_INCLUDE_PATH) $(CFLAGS) -c $(SRCDIR)/main.c -o $(BUILDDIR)/main.o
bank_manager.o: $(SRCDIR)/bank_manager.c 
	$(CC) $(C_INCLUDE_PATH) $(CFLAGS) -c $(SRCDIR)/bank_manager.c -o $(BUILDDIR)/bank_manager.o
ascii_art.o: $(SRCDIR)/ascii_art.c
	$(CC) $(C_INCLUDE_PATH) $(CFLAGS) -c $(SRCDIR)/ascii_art.c -o $(BUILDDIR)/ascii_art.o
employee.o: $(SRCDIR)/employee.c
	$(CC) $(C_INCLUDE_PATH) $(CFLAGS) -c $(SRCDIR)/employee.c -o $(BUILDDIR)/employee.o
client.o: $(SRCDIR)/client.c
	$(CC) $(C_INCLUDE_PATH) $(CFLAGS) -c $(SRCDIR)/client.c -o $(BUILDDIR)/client.o
cursor_position.o: $(SRCDIR)/cursor_position.c
	$(CC) $(C_INCLUDE_PATH) $(CFLAGS) -c $(SRCDIR)/cursor_position.c -o $(BUILDDIR)/cursor_position.o
clean: 
	rm -rf $(BUILDDIR)/*.o $(BUILDDIR)/main
