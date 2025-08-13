# Library Management System - Detailed Code Walkthrough

## 📜 Project Overview

This project is a comprehensive, command-line based Library Management System written entirely in the C programming language. It serves as a digital assistant for a librarian, capable of managing books, members, and the borrowing/returning process. The system is designed to be robust, user-friendly, and secure, featuring a clean, color-coded interface and data persistence through local files.

## ✨ Key Features

- ✅ **Dual User Roles:** A powerful **Librarian (Admin)** role with full control and a standard **Member** role for borrowing books.
- ✅ **Comprehensive Management:** Full CRUD (Create, Read, Update, Delete) operations for books and members.
- ✅ **Transaction System:** Tracks every borrowed and returned book, including dates and fines.
- ✅ **Enhanced Security:**
    - Password masking (`*******`).
    - Password strength requirements for both user roles.
    - Forced password change on first login.
    - Account lockout after 3 failed login attempts.
    - Automatic session timeout after 10 minutes of inactivity.
- ✅ **Automated Fine Calculation:** Automatically calculates and applies fines for overdue books.
- ✅ **Elegant User Interface:**
    - A clean, color-coded console interface for better readability.
    - **Pagination** for browsing long lists (e.g., all books) without clutter.
    - Screen clearing for a fresh, app-like experience in each menu.
- ✅ **Data Persistence & Integrity:**
    - All data is saved to `.txt` files, ensuring it's not lost when the program closes.
    - **File locking** is implemented to prevent data corruption from concurrent access.

---

## 🏗️ Project Structure

The entire logic is contained within `library_system.c`. The program interacts with three data files that it creates automatically:

- `books.txt`: Stores the library's entire book catalog.
- `members.txt`: Stores user account information.
- `transactions.txt`: A ledger of all borrowing and return activities.

---

## 🧠 Core Concepts Explained

Before diving into the code, let's explain some key concepts in simple terms.

### Data Structures (`struct`)
Think of a `struct` as a **blueprint or a template**. For example, a `struct Book` defines that every book must have an ID, a title, an author, etc. It's like an empty contact card template waiting to be filled out for each specific book.

### Dynamic Memory (`realloc`)
Imagine your program has a folder to store book records, and it can only hold 10 records. What happens when you add the 11th book? Instead of crashing, we use `realloc`. This function is like a magic helper that:
1.  Finds a bigger folder.
2.  Copies all the old records into the new, bigger folder.
3.  Throws away the old, small folder.
This allows our program to handle any number of books or members without knowing the exact number in advance.

### Security: Password Encryption (Caesar Cipher)
Passwords are not stored as plain text. They are "scrambled" using a simple method called a **Caesar Cipher**, which shifts each letter by 3 places (e.g., `A` becomes `D`, `p` becomes `s`). This means if someone snoops into the `members.txt` file, they won't see the actual passwords.

> **⚠️ SECURITY WARNING:** The Caesar Cipher is used here for educational purposes only because it's easy to understand. **It is NOT secure.** For any real-world application, a strong, one-way **hashing algorithm** (like SHA-256 or Argon2) must be used instead.

---

## ⚙️ Detailed Function Breakdown

Here is a line-by-line explanation of how the major parts of the code work.

### `main()` function
This is the heart of the program's execution flow.
1.  `enable_virtual_terminal_processing()`: A special function for Windows to ensure the color codes work correctly.
2.  `initialize_system()`: Loads all data from the `.txt` files into memory. If it's the first time running, it creates a default `admin` account.
3.  **Main Loop (`do-while`)**: This loop keeps the program running.
    - It `clear_screen()` to provide a clean slate.
    - It prints the main menu (Login or Exit).
    - It waits for the user's choice.
    - If `1`, it calls the `login()` function.
    - If `2`, it prints a goodbye message and the loop terminates.
4.  **Cleanup**: Before exiting, it `free()`s all the dynamic memory that was allocated, preventing memory leaks.

### UI and Utility Functions

- **`clear_screen()`**: Issues a system command (`cls` for Windows, `clear` for Linux/macOS) to wipe the console clean.
- **`press_enter_to_continue()`**: Pauses the program and waits for the user to press Enter. This is crucial for allowing users to read messages before the screen is cleared.
- **`get_string_input()` / `get_int_input()`**: These are "safe" input functions. They read the entire line of user input to prevent issues where leftover characters in the input buffer could break the next input request.
- **`get_masked_password()`**: This function is responsible for printing `*` instead of the actual characters when a user types their password. It uses platform-specific libraries for this.

### File I/O and Locking

- **`load_*()` functions (e.g., `load_books()`)**:
    1.  Opens the corresponding `.txt` file for reading (`"r"`).
    2.  Loops through each line of the file.
    3.  Uses `fscanf()` to parse the comma-separated values from the line and store them in a temporary `struct`.
    4.  Checks if our in-memory array has enough space. If not, it calls `realloc` to get more memory (the magic expandable folder).
    5.  Copies the temporary `struct` into the main array.
- **`save_*()` functions (e.g., `save_books()`)**:
    1.  Opens the corresponding `.txt` file for writing (`"w"`), which erases the old content.
    2.  **`lock_file()`**: Places an exclusive lock on the file. This tells the operating system, "Hey, I'm writing to this file. Don't let anyone else touch it until I'm done!" This prevents data corruption.
    3.  Loops through the in-memory array (e.g., `books`).
    4.  Uses `fprintf()` to write the data of each `struct` to the file in the correct comma-separated format.
    5.  **`unlock_file()`**: Releases the lock, allowing other processes to access the file again.

### Librarian (Admin) Functions

- **`list_all_books()`**:
    1.  Initializes pagination variables (`current_page`, `total_pages`).
    2.  Enters a `do-while` loop to handle page navigation.
    3.  Inside the loop, it calls `display_books_paginated()` to show only the items for the current page.
    4.  It then prompts the user for navigation input (`N` for next, `P` for previous, `Q` to quit).
    5.  The loop continues until the user presses `Q`.
- **`add_book()`**:
    1.  Clears the screen and prints a nice header.
    2.  Checks if the `books` array needs to be expanded and calls `realloc` if necessary.
    3.  Prompts the admin for the new book's details (title, author, etc.).
    4.  Assigns a new, unique `id`.
    5.  Adds the new book `struct` to the `books` array.
    6.  Calls `save_books()` to write the entire updated array to disk.
    7.  Prints a green success message.

### Member Functions

- **`borrow_book(int member_id)`**:
    1.  Uses the pagination system to display available books, allowing the member to browse.
    2.  The user can enter a book ID to borrow it.
    3.  The function finds the book by its ID.
    4.  It checks if the book is available (`book->available > 0`).
    5.  If yes, it creates a new `Transaction` record with the current date, a due date (7 days from now), and the book/member IDs.
    6.  It decrements the book's `available` count by 1.
    7.  It calls `save_books()` and `save_transactions()` to persist the changes.
- **`return_book(int member_id)`**:
    1.  Finds and displays a list of only the books the current member has borrowed but not yet returned.
    2.  Asks the user to enter the transaction ID of the book they wish to return.
    3.  Finds the corresponding transaction record.
    4.  Sets the `return_date` to the current time.
    5.  **Fine Calculation**: It compares the `return_date` with the `due_date`. If the return is late, it calculates the number of overdue days and multiplies it by `FINE_PER_DAY` to get the fine.
    6.  It increments the book's `available` count by 1.
    7.  Calls `save_books()` and `save_transactions()`.

### Core Logic

- **`login()`**:
    1.  Asks the user if they are an Admin or a Member.
    2.  Prompts for username and password (masked).
    3.  Enters a loop that allows for `MAX_LOGIN_ATTEMPTS`.
    4.  It finds the member by username in the `members` array.
    5.  It **decrypts** the stored password using `caesar_decrypt()`.
    6.  It compares the decrypted password with the one the user entered.
    7.  **On success**: It prints a success message, checks `is_first_login` to force a password change if needed, and then calls the appropriate menu (`admin_menu()` or `member_menu()`).
    8.  **On failure**: It prints a red error message and decrements the attempt counter.
- **`change_password()`**:
    1.  Prompts the user to enter a new password, checking it against the required complexity rules (`is_strong_admin_password` or `is_valid_member_password`).
    2.  Asks the user to confirm the new password.
    3.  If they match, it **encrypts** the new password with `caesar_encrypt()` and saves it.

---

## 🚀 How to Compile and Run

### Requirements
- A C compiler, such as GCC. (Comes with Linux/macOS, can be installed on Windows via [MinGW](https://www.mingw-w64.org/)).

### Compilation
Open a terminal in the project directory and run the following command:
```bash
gcc -o library_system library_system.c
```
*(On some systems, you might need `-lm` if you get math library errors: `gcc -o library_system library_system.c -lm`)*

### Execution
- On Linux or macOS:
  ```bash
  ./library_system
  ```
- On Windows:
  ```bash
  library_system.exe
  ```

### First Use
The first time you run the program, it will create a default admin account for you:
- **Username:** `admin`
- **Password:** `AdminPassword123!`

You will be required to change this password immediately upon your first login.
