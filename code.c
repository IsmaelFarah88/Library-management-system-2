#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

// For cross-platform features
#ifdef _WIN32
#include <conio.h>
#include <windows.h> // For LockFileEx
#include <io.h>      // For _get_osfhandle
#else
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>   // For fcntl
#endif

// --- Constants ---
#define BOOK_FILE "books.txt"
#define MEMBER_FILE "members.txt"
#define TRANSACTION_FILE "transactions.txt"

#define FINE_PER_DAY 10.0
#define BORROW_DURATION_DAYS 7
#define SESSION_TIMEOUT_SECONDS 600 // 10 minutes
#define MAX_LOGIN_ATTEMPTS 3
#define CAESAR_SHIFT 3

// --- Data Structures ---
typedef struct {
    int id;
    char title[100];
    char author[50];
    char category[30];
    int quantity;
    int available;
} Book;

typedef struct {
    int id;
    char name[50];
    char email[100];
    char encrypted_password[256]; // Storing the Caesar cipher encrypted password
    int is_first_login;
} Member;

typedef struct {
    int transaction_id;
    int book_id;
    int member_id;
    time_t borrow_date;
    time_t due_date;
    time_t return_date; // 0 if not returned
    float fine;
} Transaction;


// --- Simple Caesar Cipher Encryption/Decryption ---
// WARNING: This is for educational purposes only and is NOT SECURE.
void caesar_encrypt(const char *input, char *output) {
    int i = 0;
    for (i = 0; input[i] != '\0'; i++) {
        output[i] = input[i] + CAESAR_SHIFT;
    }
    output[i] = '\0';
}

void caesar_decrypt(const char *input, char *output) {
    int i = 0;
    for (i = 0; input[i] != '\0'; i++) {
        output[i] = input[i] - CAESAR_SHIFT;
    }
    output[i] = '\0';
}


// --- Global Variables (In-memory storage) ---
Book *books = NULL;
int book_count = 0;
int book_capacity = 0;
int next_book_id = 1;

Member *members = NULL;
int member_count = 0;
int member_capacity = 0;
int next_member_id = 1;

Transaction *transactions = NULL;
int transaction_count = 0;
int transaction_capacity = 0;
int next_transaction_id = 1;

time_t last_activity_time;

// --- Utility Functions ---

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Safe string input
void get_string_input(const char *prompt, char *buffer, int size) {
    printf("%s", prompt);
    fgets(buffer, size, stdin);
    buffer[strcspn(buffer, "\n")] = 0; // Remove trailing newline
}

// Get integer input
int get_int_input(const char *prompt) {
    int value;
    char buffer[100];
    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            if (sscanf(buffer, "%d", &value) == 1) {
                return value;
            }
        }
        printf("Invalid input. Please enter an integer.\n");
    }
}

// Cross-platform function to get password without echoing
void get_masked_password(const char *prompt, char *buffer, int size) {
    printf("%s", prompt);
    fflush(stdout);

#ifdef _WIN32
    int i = 0;
    char ch;
    while (i < size - 1) {
        ch = _getch();
        if (ch == '\r' || ch == '\n') { // Enter key
            break;
        }
        if (ch == '\b') { // Backspace
            if (i > 0) {
                i--;
                printf("\b \b");
            }
        } else {
            buffer[i++] = ch;
            printf("*");
        }
    }
    buffer[i] = '\0';
#else
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    fgets(buffer, size, stdin);
    buffer[strcspn(buffer, "\n")] = 0;

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif
    printf("\n");
}


// --- File Locking Functions ---
void lock_file(FILE *fp) {
#ifdef _WIN32
    HANDLE hFile = (HANDLE)_get_osfhandle(_fileno(fp));
    OVERLAPPED overlapped = {0};
    if (!LockFileEx(hFile, LOCKFILE_EXCLUSIVE_LOCK, 0, (DWORD)-1, (DWORD)-1, &overlapped)) {
        perror("Failed to lock file");
    }
#else
    struct flock fl;
    fl.l_type   = F_WRLCK;  // Exclusive write lock
    fl.l_whence = SEEK_SET; // Start from beginning of file
    fl.l_start  = 0;        // Offset 0
    fl.l_len    = 0;        // Lock the whole file
    fl.l_pid    = getpid();
    if (fcntl(fileno(fp), F_SETLKW, &fl) == -1) {
        perror("Failed to lock file");
    }
#endif
}

void unlock_file(FILE *fp) {
#ifdef _WIN32
    HANDLE hFile = (HANDLE)_get_osfhandle(_fileno(fp));
    OVERLAPPED overlapped = {0};
    if (!UnlockFileEx(hFile, 0, (DWORD)-1, (DWORD)-1, &overlapped)) {
        perror("Failed to unlock file");
    }
#else
    struct flock fl;
    fl.l_type   = F_UNLCK; // Unlock
    fl.l_whence = SEEK_SET;
    fl.l_start  = 0;
    fl.l_len    = 0;
    fl.l_pid    = getpid();
    if (fcntl(fileno(fp), F_SETLK, &fl) == -1) {
        perror("Failed to unlock file");
    }
#endif
}


// --- Password Complexity Validators ---
int is_strong_admin_password(const char *pass) {
    int len = strlen(pass);
    if (len < 12) return 0;
    int has_upper = 0, has_lower = 0, has_digit = 0, has_special = 0;
    for (int i = 0; i < len; i++) {
        if (isupper(pass[i])) has_upper = 1;
        else if (islower(pass[i])) has_lower = 1;
        else if (isdigit(pass[i])) has_digit = 1;
        else if (ispunct(pass[i])) has_special = 1;
    }
    return has_upper && has_lower && has_digit && has_special;
}

int is_valid_member_password(const char *pass) {
    int len = strlen(pass);
    if (len < 8) return 0;
    int has_digit = 0;
    for (int i = 0; i < len; i++) {
        if (isdigit(pass[i])) has_digit = 1;
    }
    return has_digit;
}


// --- File I/O Functions ---

void load_books() {
    FILE *file = fopen(BOOK_FILE, "r");
    if (!file) return;

    Book temp;
    while (fscanf(file, "%d,%99[^,],%49[^,],%29[^,],%d,%d\n", &temp.id, temp.title, temp.author, temp.category, &temp.quantity, &temp.available) == 6) {
        if (book_count >= book_capacity) {
            book_capacity = (book_capacity == 0) ? 10 : book_capacity * 2;
            books = realloc(books, book_capacity * sizeof(Book));
            if (!books) {
                perror("Failed to reallocate memory for books");
                exit(EXIT_FAILURE);
            }
        }
        books[book_count++] = temp;
        if (temp.id >= next_book_id) {
            next_book_id = temp.id + 1;
        }
    }
    fclose(file);
}

void save_books() {
    FILE *file = fopen(BOOK_FILE, "w");
    if (!file) {
        perror("Could not open books file for writing");
        return;
    }
    lock_file(file);
    for (int i = 0; i < book_count; i++) {
        fprintf(file, "%d,%s,%s,%s,%d,%d\n", books[i].id, books[i].title, books[i].author, books[i].category, books[i].quantity, books[i].available);
    }
    unlock_file(file);
    fclose(file);
}

void load_members() {
    FILE *file = fopen(MEMBER_FILE, "r");
    if (!file) return;

    Member temp;
    while (fscanf(file, "%d,%49[^,],%99[^,],%255[^,],%d\n", &temp.id, temp.name, temp.email, temp.encrypted_password, &temp.is_first_login) == 5) {
        if (member_count >= member_capacity) {
            member_capacity = (member_capacity == 0) ? 10 : member_capacity * 2;
            members = realloc(members, member_capacity * sizeof(Member));
            if (!members) {
                perror("Failed to reallocate memory for members");
                exit(EXIT_FAILURE);
            }
        }
        members[member_count++] = temp;
        if (temp.id >= next_member_id) {
            next_member_id = temp.id + 1;
        }
    }
    fclose(file);
}

void save_members() {
    FILE *file = fopen(MEMBER_FILE, "w");
    if (!file) {
        perror("Could not open members file for writing");
        return;
    }
    lock_file(file);
    for (int i = 0; i < member_count; i++) {
        fprintf(file, "%d,%s,%s,%s,%d\n", members[i].id, members[i].name, members[i].email, members[i].encrypted_password, members[i].is_first_login);
    }
    unlock_file(file);
    fclose(file);
}

void load_transactions() {
    FILE *file = fopen(TRANSACTION_FILE, "r");
    if (!file) return;

    Transaction temp;
    long borrow_t, due_t, return_t;
    while (fscanf(file, "%d,%d,%d,%ld,%ld,%ld,%f\n", &temp.transaction_id, &temp.book_id, &temp.member_id, &borrow_t, &due_t, &return_t, &temp.fine) == 7) {
        temp.borrow_date = (time_t)borrow_t;
        temp.due_date = (time_t)due_t;
        temp.return_date = (time_t)return_t;

        if (transaction_count >= transaction_capacity) {
            transaction_capacity = (transaction_capacity == 0) ? 20 : transaction_capacity * 2;
            transactions = realloc(transactions, transaction_capacity * sizeof(Transaction));
             if (!transactions) {
                perror("Failed to reallocate memory for transactions");
                exit(EXIT_FAILURE);
            }
        }
        transactions[transaction_count++] = temp;
        if (temp.transaction_id >= next_transaction_id) {
            next_transaction_id = temp.transaction_id + 1;
        }
    }
    fclose(file);
}

void save_transactions() {
    FILE *file = fopen(TRANSACTION_FILE, "w");
    if (!file) {
        perror("Could not open transactions file for writing");
        return;
    }
    lock_file(file);
    for (int i = 0; i < transaction_count; i++) {
        fprintf(file, "%d,%d,%d,%ld,%ld,%ld,%.2f\n", transactions[i].transaction_id, transactions[i].book_id, transactions[i].member_id, (long)transactions[i].borrow_date, (long)transactions[i].due_date, (long)transactions[i].return_date, transactions[i].fine);
    }
    unlock_file(file);
    fclose(file);
}


// --- Find Functions ---
Book* find_book_by_id(int id) {
    for (int i = 0; i < book_count; i++) {
        if (books[i].id == id) {
            return &books[i];
        }
    }
    return NULL;
}

Member* find_member_by_id(int id) {
    for (int i = 0; i < member_count; i++) {
        if (members[i].id == id) {
            return &members[i];
        }
    }
    return NULL;
}

Member* find_member_by_name(const char* name) {
    for (int i = 0; i < member_count; i++) {
        if (strcmp(members[i].name, name) == 0) {
            return &members[i];
        }
    }
    return NULL;
}


// --- Admin Functions ---

void add_book() {
    printf("\n--- Add a New Book ---\n");
    if (book_count >= book_capacity) {
        book_capacity = (book_capacity == 0) ? 10 : book_capacity * 2;
        books = realloc(books, book_capacity * sizeof(Book));
        if (!books) {
            printf("Error: Failed to allocate memory.\n");
            return;
        }
    }
    
    Book *new_book = &books[book_count];
    new_book->id = next_book_id++;
    
    get_string_input("Book Title: ", new_book->title, 100);
    get_string_input("Author: ", new_book->author, 50);
    get_string_input("Category: ", new_book->category, 30);
    new_book->quantity = get_int_input("Total Quantity: ");
    new_book->available = new_book->quantity;

    book_count++;
    save_books();
    printf("Book added successfully! Book ID: %d\n", new_book->id);
}

void delete_book() {
    printf("\n--- Delete a Book ---\n");
    int id = get_int_input("Enter Book ID to delete: ");

    int found_index = -1;
    for (int i = 0; i < book_count; i++) {
        if (books[i].id == id) {
            found_index = i;
            break;
        }
    }

    if (found_index == -1) {
        printf("Book not found.\n");
        return;
    }
    
    for (int i = found_index; i < book_count - 1; i++) {
        books[i] = books[i + 1];
    }
    book_count--;
    save_books();
    printf("Book deleted successfully.\n");
}

void list_all_books() {
    printf("\n--- List of All Books ---\n");
    printf("%-5s | %-30s | %-20s | %-15s | %-8s | %-8s\n", "ID", "Title", "Author", "Category", "Total", "Available");
    printf("--------------------------------------------------------------------------------------------------\n");
    for (int i = 0; i < book_count; i++) {
        printf("%-5d | %-30s | %-20s | %-15s | %-8d | %-8d\n", books[i].id, books[i].title, books[i].author, books[i].category, books[i].quantity, books[i].available);
    }
}

void add_member() {
    printf("\n--- Add a New Member ---\n");
     if (member_count >= member_capacity) {
        member_capacity = (member_capacity == 0) ? 10 : member_capacity * 2;
        members = realloc(members, member_capacity * sizeof(Member));
        if (!members) {
            printf("Error: Failed to allocate memory.\n");
            return;
        }
    }

    Member *new_member = &members[member_count];
    new_member->id = next_member_id++;

    get_string_input("Member Name: ", new_member->name, 50);
    get_string_input("Email: ", new_member->email, 100);
    
    char password[256];
    while (1) {
        get_masked_password("Enter initial password (min 8 chars, at least 1 number): ", password, sizeof(password));
        if (is_valid_member_password(password)) {
            break;
        }
        printf("Weak password. Must be at least 8 characters and contain one number.\n");
    }

    caesar_encrypt(password, new_member->encrypted_password);
    new_member->is_first_login = 1;

    member_count++;
    save_members();
    printf("Member added successfully! Member ID: %d\n", new_member->id);
}

void delete_member() {
    printf("\n--- Delete a Member ---\n");
    int id = get_int_input("Enter Member ID to delete: ");

    int found_index = -1;
    for (int i = 0; i < member_count; i++) {
        if (members[i].id == id) {
            found_index = i;
            break;
        }
    }

    if (found_index == -1) {
        printf("Member not found.\n");
        return;
    }
    
    for (int i = found_index; i < member_count - 1; i++) {
        members[i] = members[i + 1];
    }
    member_count--;
    save_members();
    printf("Member deleted successfully.\n");
}

void view_all_transactions() {
    printf("\n--- All Transactions ---\n");
    printf("%-5s | %-10s | %-10s | %-20s | %-20s | %-10s\n", "ID", "Book ID", "Member ID", "Borrow Date", "Return Date", "Fine");
    printf("--------------------------------------------------------------------------------------------------\n");

    char borrow_date_str[30], return_date_str[30];
    for (int i = 0; i < transaction_count; i++) {
        strftime(borrow_date_str, sizeof(borrow_date_str), "%Y-%m-%d %H:%M", localtime(&transactions[i].borrow_date));
        if (transactions[i].return_date != 0) {
            strftime(return_date_str, sizeof(return_date_str), "%Y-%m-%d %H:%M", localtime(&transactions[i].return_date));
        } else {
            strcpy(return_date_str, "Not yet returned");
        }
        printf("%-5d | %-10d | %-10d | %-20s | %-20s | $%-9.2f\n", transactions[i].transaction_id, transactions[i].book_id, transactions[i].member_id, borrow_date_str, return_date_str, transactions[i].fine);
    }
}

void reset_member_password() {
    printf("\n--- Reset Member Password ---\n");
    int member_id = get_int_input("Enter member ID: ");

    Member* member = find_member_by_id(member_id);
    if (!member) {
        printf("Member not found.\n");
        return;
    }

    char new_password[256];
    while(1) {
        get_masked_password("Enter new password for member: ", new_password, sizeof(new_password));
        if (is_valid_member_password(new_password)) {
            break;
        }
        printf("Weak password. Must be at least 8 characters and contain one number.\n");
    }

    caesar_encrypt(new_password, member->encrypted_password);
    member->is_first_login = 1; // Force user to change it on next login
    save_members();
    printf("Password has been reset successfully. Member will be asked to change it on next login.\n");
}

// --- Member Functions ---

void search_books() {
    printf("\n--- Search for a Book ---\n");
    printf("1. Search by Title\n");
    printf("2. Search by Author\n");
    printf("3. Search by Category\n");
    int choice = get_int_input("Choose search method: ");
    
    char query[100];
    get_string_input("Enter search term: ", query, sizeof(query));
    
    int found = 0;
    printf("\n--- Search Results ---\n");
    printf("%-5s | %-30s | %-20s | %-15s | %-8s | %-8s\n", "ID", "Title", "Author", "Category", "Total", "Available");
    printf("--------------------------------------------------------------------------------------------------\n");

    for (int i = 0; i < book_count; i++) {
        int match = 0;
        char lower_query[100], lower_title[100], lower_author[100], lower_category[30];
        
        // Convert query and fields to lower case for case-insensitive search
        for(int j=0; query[j]; j++){ lower_query[j] = tolower(query[j]); } lower_query[strlen(query)] = '\0';
        for(int j=0; books[i].title[j]; j++){ lower_title[j] = tolower(books[i].title[j]); } lower_title[strlen(books[i].title)] = '\0';
        for(int j=0; books[i].author[j]; j++){ lower_author[j] = tolower(books[i].author[j]); } lower_author[strlen(books[i].author)] = '\0';
        for(int j=0; books[i].category[j]; j++){ lower_category[j] = tolower(books[i].category[j]); } lower_category[strlen(books[i].category)] = '\0';

        switch(choice) {
            case 1: if (strstr(lower_title, lower_query)) match = 1; break;
            case 2: if (strstr(lower_author, lower_query)) match = 1; break;
            case 3: if (strstr(lower_category, lower_query)) match = 1; break;
            default: printf("Invalid choice.\n"); return;
        }
        if (match) {
            printf("%-5d | %-30s | %-20s | %-15s | %-8d | %-8d\n", books[i].id, books[i].title, books[i].author, books[i].category, books[i].quantity, books[i].available);
            found = 1;
        }
    }

    if (!found) {
        printf("No books found matching your search.\n");
    }
}

void borrow_book(int member_id) {
    printf("\n--- Borrow a Book ---\n");
    int book_id = get_int_input("Enter the ID of the book you want to borrow: ");

    Book* book = find_book_by_id(book_id);
    if (!book) {
        printf("Book not found.\n");
        return;
    }
    if (book->available <= 0) {
        printf("Sorry, this book is currently unavailable.\n");
        return;
    }

    if (transaction_count >= transaction_capacity) {
        transaction_capacity = (transaction_capacity == 0) ? 20 : transaction_capacity * 2;
        transactions = realloc(transactions, transaction_capacity * sizeof(Transaction));
         if (!transactions) {
            printf("Error: Failed to allocate memory.\n");
            return;
        }
    }

    Transaction *new_trans = &transactions[transaction_count];
    new_trans->transaction_id = next_transaction_id++;
    new_trans->book_id = book_id;
    new_trans->member_id = member_id;
    new_trans->borrow_date = time(NULL);
    new_trans->due_date = new_trans->borrow_date + (BORROW_DURATION_DAYS * 24 * 60 * 60);
    new_trans->return_date = 0; // Not returned yet
    new_trans->fine = 0.0;

    transaction_count++;
    book->available--;
    
    save_books();
    save_transactions();

    char due_date_str[30];
    strftime(due_date_str, sizeof(due_date_str), "%Y-%m-%d", localtime(&new_trans->due_date));
    printf("Book borrowed successfully. The due date is: %s\n", due_date_str);
}

void return_book(int member_id) {
    printf("\n--- Return a Book ---\n");
    printf("Books you have borrowed and not yet returned:\n");
    
    int active_borrows[transaction_count];
    int active_count = 0;
    
    printf("%-15s | %-30s\n", "Transaction ID", "Book Title");
    printf("----------------------------------------------\n");
    for (int i = 0; i < transaction_count; i++) {
        if (transactions[i].member_id == member_id && transactions[i].return_date == 0) {
            Book* book = find_book_by_id(transactions[i].book_id);
            if (book) {
                printf("%-15d | %-30s\n", transactions[i].transaction_id, book->title);
                active_borrows[active_count++] = transactions[i].transaction_id;
            }
        }
    }
    
    if (active_count == 0) {
        printf("You have no books to return.\n");
        return;
    }

    int trans_id = get_int_input("Enter the transaction ID for the book you want to return: ");
    
    Transaction* trans = NULL;
    for (int i = 0; i < transaction_count; i++) {
        if (transactions[i].transaction_id == trans_id && transactions[i].member_id == member_id && transactions[i].return_date == 0) {
            trans = &transactions[i];
            break;
        }
    }

    if (!trans) {
        printf("Invalid transaction ID or book has already been returned.\n");
        return;
    }
    
    trans->return_date = time(NULL);
    
    // Calculate fine
    if (trans->return_date > trans->due_date) {
        double seconds_late = difftime(trans->return_date, trans->due_date);
        int days_late = (int)(seconds_late / (60 * 60 * 24)) + 1;
        trans->fine = days_late * FINE_PER_DAY;
        printf("The book is overdue! A fine of $%.2f has been charged.\n", trans->fine);
    } else {
        printf("Thank you for returning the book on time.\n");
    }

    Book* book = find_book_by_id(trans->book_id);
    if (book) {
        book->available++;
    }

    save_books();
    save_transactions();
    printf("Book returned successfully.\n");
}


void view_my_records(int member_id) {
    printf("\n--- My Personal Records ---\n");
    printf("%-5s | %-20s | %-12s | %-12s | %-10s\n", "ID", "Book Title", "Borrow Date", "Return Date", "Fine");
    printf("--------------------------------------------------------------------------------\n");
    
    int found = 0;
    char borrow_date_str[20], return_date_str[20];
    for (int i = 0; i < transaction_count; i++) {
        if (transactions[i].member_id == member_id) {
            Book* book = find_book_by_id(transactions[i].book_id);
            if (!book) continue;

            strftime(borrow_date_str, sizeof(borrow_date_str), "%Y-%m-%d", localtime(&transactions[i].borrow_date));
            if (transactions[i].return_date != 0) {
                strftime(return_date_str, sizeof(return_date_str), "%Y-%m-%d", localtime(&transactions[i].return_date));
            } else {
                strcpy(return_date_str, "Not returned");
            }
            
            printf("%-5d | %-20s | %-12s | %-12s | $%-9.2f\n", transactions[i].transaction_id, book->title, borrow_date_str, return_date_str, transactions[i].fine);
            found = 1;
        }
    }

    if (!found) {
        printf("No records found.\n");
    }
}


// --- Menus ---
void admin_menu();
void member_menu(int member_id);

int check_session_timeout() {
    if (time(NULL) - last_activity_time > SESSION_TIMEOUT_SECONDS) {
        printf("\nSession timed out due to inactivity. You have been logged out.\n");
        return 1;
    }
    last_activity_time = time(NULL);
    return 0;
}

void change_password(Member *member, int is_admin) {
    char new_pass[256], confirm_pass[256];
    printf("\n--- Mandatory Password Change ---\n");
    while(1) {
        if (is_admin) {
             get_masked_password("Enter new password (min 12 chars, upper/lower case, numbers, symbols): ", new_pass, sizeof(new_pass));
             if (!is_strong_admin_password(new_pass)) {
                 printf("Password does not meet the security requirements. Please try again.\n");
                 continue;
             }
        } else {
             get_masked_password("Enter new password (min 8 chars, at least 1 number): ", new_pass, sizeof(new_pass));
             if (!is_valid_member_password(new_pass)) {
                 printf("Password does not meet the security requirements. Please try again.\n");
                 continue;
             }
        }

        get_masked_password("Confirm new password: ", confirm_pass, sizeof(confirm_pass));

        if (strcmp(new_pass, confirm_pass) != 0) {
            printf("Passwords do not match. Please try again.\n");
        } else {
            caesar_encrypt(new_pass, member->encrypted_password);
            member->is_first_login = 0;
            save_members();
            printf("Password changed successfully.\n");
            break;
        }
    }
}


void login() {
    printf("\n--- Login ---\n");
    printf("1. Login as Librarian (Admin)\n");
    printf("2. Login as Member\n");
    int choice = get_int_input("Select user type: ");

    char username[50];
    char password[256];

    get_string_input("Username: ", username, sizeof(username));

    int login_attempts = 0;
    while(login_attempts < MAX_LOGIN_ATTEMPTS) {
        get_masked_password("Password: ", password, sizeof(password));

        Member* member_to_check = NULL;
        if (choice == 1) { // Admin Login
            if (strcmp(username, "admin") == 0) {
                member_to_check = find_member_by_name("admin");
            }
        } else { // Member Login
            member_to_check = find_member_by_name(username);
        }

        if (member_to_check) {
            char decrypted_pass[256];
            caesar_decrypt(member_to_check->encrypted_password, decrypted_pass);
            
            if (strcmp(password, decrypted_pass) == 0) {
                printf("Login successful.\n");
                last_activity_time = time(NULL);
                if (member_to_check->is_first_login) {
                    change_password(member_to_check, (choice == 1));
                }
                
                if (choice == 1) {
                    admin_menu();
                } else {
                    member_menu(member_to_check->id);
                }
                return;
            }
        }
        
        login_attempts++;
        printf("Incorrect username or password. Attempts remaining: %d\n", MAX_LOGIN_ATTEMPTS - login_attempts);
    }
    printf("Maximum login attempts exceeded. The account is temporarily locked.\n");
}


void admin_menu() {
    int choice;
    do {
        if (check_session_timeout()) return;
        printf("\n--- Librarian Menu ---\n");
        printf("1. Add Book\n");
        printf("2. Delete Book\n");
        printf("3. View All Books\n");
        printf("4. Add Member\n");
        printf("5. Delete Member\n");
        printf("6. View All Transactions\n");
        printf("7. Reset Member Password\n");
        printf("8. Logout\n");
        choice = get_int_input("Select an option: ");
        
        switch(choice) {
            case 1: add_book(); break;
            case 2: delete_book(); break;
            case 3: list_all_books(); break;
            case 4: add_member(); break;
            case 5: delete_member(); break;
            case 6: view_all_transactions(); break;
            case 7: reset_member_password(); break;
            case 8: printf("Logged out.\n"); break;
            default: printf("Invalid option.\n");
        }
    } while (choice != 8);
}


void member_menu(int member_id) {
    int choice;
    do {
        if (check_session_timeout()) return;
        printf("\n--- Member Menu ---\n");
        printf("1. Search for a Book\n");
        printf("2. Borrow a Book\n");
        printf("3. Return a Book\n");
        printf("4. View My Records\n");
        printf("5. Logout\n");
        choice = get_int_input("Select an option: ");
        
        switch(choice) {
            case 1: search_books(); break;
            case 2: borrow_book(member_id); break;
            case 3: return_book(member_id); break;
            case 4: view_my_records(member_id); break;
            case 5: printf("Logged out.\n"); break;
            default: printf("Invalid option.\n");
        }
    } while (choice != 5);
}

void initialize_system() {
    load_books();
    load_members();
    load_transactions();

    // Create default admin if no members exist
    if (member_count == 0) {
        printf("No users found. Creating a default admin account.\n");
        printf("Username: admin\n");
        printf("Password: AdminPassword123!\n");
        
        Member admin;
        admin.id = next_member_id++;
        strcpy(admin.name, "admin");
        strcpy(admin.email, "admin@library.com");
        admin.is_first_login = 1;
        caesar_encrypt("AdminPassword123!", admin.encrypted_password);
        
        if (member_count >= member_capacity) {
            member_capacity = 10;
            members = realloc(members, member_capacity * sizeof(Member));
        }
        members[member_count++] = admin;
        save_members();
    }
}


int main() {
    initialize_system();

    int choice;
    do {
        printf("\n==============================\n");
        printf("   Library Management System\n");
        printf("==============================\n");
        printf("1. Login\n");
        printf("2. Exit\n");
        choice = get_int_input("Choose an option: ");

        switch (choice) {
            case 1:
                login();
                break;
            case 2:
                printf("Saving data... Goodbye!\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 2);

    free(books);
    free(members);
    free(transactions);

    return 0;
}