# Bank Management System
#### Video Demo:  <URL HERE>
## Description
A robust banking system implemented in C that provides comprehensive banking operations management. The system uses SQLite3 for data persistence and features a secure authentication system for employees and bank managers.

## Features
- Employee Management
- Client Account Management
- Transaction Processing
- Secure Authentication
- Database Integration
- Account Balance Tracking

## Technologies Used
- Language: C
- Database: SQLite3
- Build System: Make

## Project Structure
```
cs50-project/
├── src/                  # Source files (.c)
│   ├── main.c           # Main program entry
│   ├── bank_manager.c   # Bank manager operations
│   ├── client.c         # Client operations
│   ├── employee.c       # Employee operations
│   ├── cursor_position.c# UI cursor handling
│   └── ascii_art.c      # ASCII art rendering
├── inc/                 # Header files (.h)
│   ├── bank_manager.h   # Bank manager declarations
│   ├── client.h         # Client declarations
│   ├── employee.h       # Employee declarations
│   ├── cursor_position.h# UI cursor declarations
│   └── ascii_art.h      # ASCII art declarations
├── ascii-art/           # ASCII art text files
│   ├── home.txt        # Home screen art
│   └── welcome.txt     # Welcome screen art
├── schema.sql          # Database schema
├── Makefile           # Build configuration
└── README.md          # Project documentation
```

## Building the Project
1. Ensure you have the following prerequisites:
   - GCC compiler
   - SQLite3 development libraries
   - Make build system

2. Build the project:
   ```bash
   make
   ```

3. Clean build files:
   ```bash
   make clean
   ```

## Database Schema
The system uses SQLite3 with tables for:
- Employees
- Clients
- Accounts
- Transactions

## Usage
After building the project, run the executable to start the banking system. You'll be prompted to:
1. Login as an employee or manager
2. Perform various banking operations based on your role

## Security
- Password-protected access
- Role-based authentication
- Secure transaction handling

## Contributing
1. Fork the repository
2. Create your feature branch
3. Commit your changes
4. Push to the branch
5. Create a new Pull Request