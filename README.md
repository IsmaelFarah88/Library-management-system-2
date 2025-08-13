# Library Management System: A Line-by-Line Code Analysis

## 📜 Document Purpose

This document provides an exhaustive, line-by-line technical breakdown of the C-based Library Management System. Its goal is to demystify the code, making it understandable for developers, students, and anyone curious about how its features are implemented. We will explore everything from the foundational building blocks to the core application logic.

---

## 🏛️ Part 1: The Building Blocks (Headers, Globals, Structs)

Before any function runs, the program sets up its environment and defines its data templates.

### Headers (`#include`)

This is the first section of the code. Each `#include` directive tells the compiler to bring in a "toolbox" of pre-written functions.

```c
#include <stdio.h>      // Standard Input/Output: For functions like printf() and fopen().
#include <stdlib.h>     // Standard Library: For memory allocation (realloc, free) and system("cls").
#include <string.h>     // String Library: For working with text (strcmp, strcpy, strstr).
#include <time.h>       // Time Library: For getting the current time (time()) for transactions.
#include <ctype.h>      // Character Type Library: For functions like tolower() and isdigit().

#ifdef _WIN32
#include <conio.h>      // Windows-specific: For _getch() used in masked password input.
#include <windows.h>    // Windows-specific: For color support and file locking.
#include <io.h>         // Windows-specific: For getting file handles for locking.
#else
#include <termios.h>    // Linux/macOS-specific: For masked password input.
#include <unistd.h>     // Linux/macOS-specific: General POSIX functions.
#include <fcntl.h>      // Linux/macOS-specific: For file control and locking.
#endif
```

### Data Structures (`struct`)

These are the custom "blueprints" for our data.

```c
typedef struct {
    int id;             // A unique number for each book.
    char title[100];    // A string to hold the book's title.
    // ... and so on for author, category, etc.
} Book;

typedef struct {
    int id;             // A unique number for each member.
    char name[50];      // The member's username.
    char email[100];    // The member's email address.
    char encrypted_password[256]; // The scrambled password.
    int is_first_login; // A flag (1 for yes, 0 for no) to force password change.
} Member;

// The Transaction struct is defined similarly.
```

### Global Variables

These variables are accessible from anywhere in the code. They hold the application's "state" (all the data currently loaded in memory).

```c
Book *books = NULL;          // A pointer that will point to our array of books in memory. Starts as NULL (empty).
int book_count = 0;          // How many books are currently loaded.
int book_capacity = 0;       // How much space we have allocated for books (our "shelf size").
int next_book_id = 1;        // A counter to ensure every new book gets a unique ID.
// The same pattern is repeated for members and transactions.
```

---

## 🛠️ Part 2: The Foundation - Utility & UI Functions

These are the helper functions that make the rest of the code cleaner and more powerful.

### `void clear_screen()`

**Purpose:** To wipe the console clean, creating an app-like feel.

```c
void clear_screen() {
#ifdef _WIN32
    system("cls"); // If on Windows, run the "cls" command.
#else
    system("clear"); // Otherwise (Linux/macOS), run the "clear" command.
#endif
}
```
1.  **`#ifdef _WIN32`**: A preprocessor directive. It checks if the code is being compiled on Windows.
2.  **`system("cls")`**: Executes the command-line command `cls`, which clears the screen on Windows.
3.  **`#else`**: If the condition above is false.
4.  **`system("clear")`**: Executes the `clear` command, which does the same on Linux and macOS.

### `void press_enter_to_continue()`

**Purpose:** To pause the program until the user is ready to proceed. This is essential for letting the user read messages before the screen is cleared.

```c
void press_enter_to_continue() {
    printf(COLOR_YELLOW "\n\nPress Enter to continue..." COLOR_RESET);
    getchar();
}
```
1.  **`printf(...)`**: Prints the "Press Enter..." message in yellow.
2.  **`getchar()`**: This is the key. It waits and reads a single character from the keyboard. The program will not proceed until a character is entered (in this case, when the user hits the Enter key).

### `int get_int_input(const char *prompt)`

**Purpose:** To safely get an integer from the user, preventing crashes from invalid text input.

```c
int get_int_input(const char *prompt) {
    int value;
    char buffer[100];
    while (1) { // Loop forever until a valid number is entered.
        printf("%s", prompt); // Display the prompt message (e.g., "Enter your choice: ").
        if (fgets(buffer, sizeof(buffer), stdin) && sscanf(buffer, "%d", &value) == 1) {
            return value; // If successful, exit the loop and return the integer.
        }
        printf(COLOR_RED "Invalid input. Please enter an integer.\n" COLOR_RESET);
    }
}
```
1.  **`while (1)`**: Creates an infinite loop. The only way to exit is with the `return` statement.
2.  **`fgets(...)`**: Reads a full line of text from the user's input into `buffer`. This is safer than `scanf` because it handles extra characters gracefully.
3.  **`sscanf(buffer, "%d", &value) == 1`**: This is the magic. It tries to "scan" the text in `buffer` for an integer (`%d`). If it successfully finds exactly one integer, it returns `1`.
4.  **`return value`**: If `sscanf` was successful, the function immediately stops and returns the extracted integer.
5.  **`printf(...)`**: If `sscanf` failed (e.g., the user typed "abc"), it prints an error message, and the loop repeats, asking for input again.

---

## 🗃️ Part 3: Data Management - File I/O and Locking

This section details how the program remembers data between sessions.

### `void save_books()` (Example for all `save_*` functions)

**Purpose:** To write the entire array of books from memory to the `books.txt` file.

```c
void save_books() {
    // 1. Open the file in "write" mode. This erases the old file.
    FILE *file = fopen(BOOK_FILE, "w");
    if (!file) { perror("Could not open books file"); return; }
    
    // 2. Lock the file to prevent other processes from writing to it.
    lock_file(file);
    
    // 3. Loop through every book currently in the 'books' array.
    for (int i = 0; i < book_count; i++) {
        // 4. Write the book's data to the file in a comma-separated format.
        fprintf(file, "%d,%s,%s,%s,%d,%d\n", 
                books[i].id, books[i].title, books[i].author, 
                books[i].category, books[i].quantity, books[i].available);
    }
    
    // 5. Release the lock.
    unlock_file(file);
    
    // 6. Close the file to save the changes.
    fclose(file);
}
```

### `void load_books()` (Example for all `load_*` functions)

**Purpose:** To read the data from `books.txt` and load it into the `books` array in memory when the program starts.

```c
void load_books() {
    // 1. Open the file in "read" mode.
    FILE *file = fopen(BOOK_FILE, "r");
    if (!file) return; // If the file doesn't exist (e.g., first run), just exit.

    Book temp; // 2. A temporary variable to hold the data for one book.

    // 3. Loop as long as we can successfully read a full line that matches the format.
    while (fscanf(file, "%d,%99[^,],%49[^,],%29[^,],%d,%d\n", 
           &temp.id, temp.title, temp.author, temp.category, &temp.quantity, &temp.available) == 6) {
        
        // 4. Check if our memory array is full.
        if (book_count >= book_capacity) {
            // 5. If full, double its capacity (our "shelf size").
            book_capacity = (book_capacity == 0) ? 10 : book_capacity * 2;
            books = realloc(books, book_capacity * sizeof(Book));
        }

        // 6. Copy the book from the 'temp' variable into the main 'books' array.
        books[book_count++] = temp;

        // 7. Update the global ID counter to avoid re-using IDs.
        if (temp.id >= next_book_id) next_book_id = temp.id + 1;
    }
    
    // 8. Close the file.
    fclose(file);
}
```
- **Line 3 (`fscanf` format string)**: This is complex but powerful. `%99[^,]` means "read up to 99 characters until you hit a comma." This prevents buffer overflows. `fscanf` returns the number of items it successfully read, so we check if it's `6` to ensure the line is not corrupted.

---

## 🎬 Part 4: The Core Application Logic

These are the functions that the user interacts with directly.

### `void admin_menu()`

**Purpose:** To display the main control panel for the Librarian and handle their actions.

```c
void admin_menu() {
    int choice;
    do { // Start a loop that continues until the user chooses to logout.
        // 1. Check for session timeout. If true, this function exits immediately.
        if (check_session_timeout()) return;
        
        // 2. Clear the screen and display the menu options.
        clear_screen();
        printf(COLOR_CYAN "===================================\n"
                         "          Librarian Menu\n"
                         "===================================\n" COLOR_RESET);
        printf("1. Add Book\n2. Delete Book\n ... 8. Logout\n");

        // 3. Get the user's choice.
        choice = get_int_input("\nSelect an option: ");

        // 4. Use a 'switch' statement to perform an action based on the choice.
        switch(choice) {
            case 1: 
                add_book(); // Call the function to add a book.
                press_enter_to_continue(); // Pause so the user can see the result.
                break;
            case 3:
                list_all_books(); // This function has its own pause system (pagination).
                break;
            case 8:
                printf(COLOR_YELLOW "Logged out.\n" COLOR_RESET);
                press_enter_to_continue();
                break;
            default:
                printf(COLOR_RED "Invalid option.\n" COLOR_RESET);
                press_enter_to_continue();
        }
    } while (choice != 8); // 5. The loop repeats unless the choice was 8.
}
```

### `void search_books()`

**Purpose:** Allows members to search the library catalog.

```c
void search_books() {
    // 1. Display search options (Title, Author, etc.) and get user's choice.
    // ...
    char query[100];
    get_string_input("Enter search term: ", query, sizeof(query));

    // 2. Convert the user's search query to lowercase for case-insensitive search.
    char lower_query[100];
    for(int j=0; query[j]; j++){ lower_query[j] = tolower(query[j]); }
    lower_query[strlen(query)] = '\0';

    // 3. Print the results header.
    // ...

    // 4. Loop through every book in the library.
    for (int i = 0; i < book_count; i++) {
        // ...
        // 5. Get the field to check based on the user's search choice (title, author, etc.).
        const char *field_to_check = books[i].title; // Example
        
        // 6. Convert that field to lowercase for comparison.
        char lower_field[100];
        for(int j=0; field_to_check[j]; j++){ lower_field[j] = tolower(field_to_check[j]); }
        lower_field[strlen(field_to_check)] = '\0';

        // 7. Use strstr() to see if the 'lower_query' is a substring of the 'lower_field'.
        if (strstr(lower_field, lower_query)) {
            // 8. If it's a match, print the book's details.
            printf("%-5d | %-30s ...\n", books[i].id, books[i].title, ...);
            found = 1;
        }
    }
    // ...
}
```
