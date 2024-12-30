-- Enable foreign key support
PRAGMA foreign_keys = ON;

-- Create Employee table
CREATE TABLE IF NOT EXISTS Employee (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    first_name TEXT NOT NULL,
    last_name TEXT NOT NULL,
    username TEXT NOT NULL UNIQUE,
    password TEXT NOT NULL
);

-- Insert admin employee
INSERT OR IGNORE INTO Employee (first_name, last_name, username, password) 
VALUES ('Admin', 'User', 'admin', 'admin');

-- Create Client table
CREATE TABLE IF NOT EXISTS Client (
    national_id TEXT PRIMARY KEY,
    first_name TEXT NOT NULL,
    last_name TEXT NOT NULL,
    account_type TEXT NOT NULL,
    balance REAL DEFAULT 0.0,
    pin TEXT NOT NULL,
    phone TEXT NOT NULL
);

-- Create Credit_card table
CREATE TABLE IF NOT EXISTS Credit_card (
    card_number TEXT PRIMARY KEY,
    expiry_date DATE NOT NULL,
    holder_id TEXT NOT NULL,
    network TEXT NOT NULL,
    cvv TEXT NOT NULL,
    FOREIGN KEY (holder_id) REFERENCES Client(national_id)
);

-- Create Bank_transaction table
CREATE TABLE IF NOT EXISTS Bank_transaction (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    employee_id INTEGER NOT NULL,
    client_id TEXT NOT NULL,
    date DATETIME DEFAULT CURRENT_TIMESTAMP,
    type TEXT NOT NULL,
    amount REAL NOT NULL,
    FOREIGN KEY (employee_id) REFERENCES Employee(id),
    FOREIGN KEY (client_id) REFERENCES Client(national_id)
);
