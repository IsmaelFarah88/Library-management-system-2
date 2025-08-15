#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

// For cross-platform features
#ifdef _WIN32
#include <conio.h>
#include <windows.h> // For LockFileEx and Colors
#include <io.h>      // For _get_osfhandle
#else
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>   // For fcntl
#endif

// --- UI Constants ---
#define ITEMS_PER_PAGE 10

// ANSI Color Codes
#define COLOR_RESET   "\x1b[0m"
#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_CYAN    "\x1b[36m"

// --- System Constants ---
#define BOOK_FILE "books.txt"
#define MEMBER_FILE "members.txt"
#define TRANSACTION_FILE "transactions.txt"
#define FINE_PER_DAY 10.0
#define BORROW_DURATION_DAYS 7
#define SESSION_TIMEOUT_SECONDS 600
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
    char encrypted_password[256];
    int is_first_login;
} Member;

typedef struct {
    int transaction_id;
    int book_id;
    int member_id;
    time_t borrow_date;
    time_t due_date;
    time_t return_date;
    float fine;
} Transaction;

// --- Simple Caesar Cipher ---
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

// --- Global Variables ---
Book *books = NULL; int book_count = 0, book_capacity = 0, next_book_id = 1;
Member *members = NULL; int member_count = 0, member_capacity = 0, next_member_id = 1;
Transaction *transactions = NULL; int transaction_count = 0, transaction_capacity = 0, next_transaction_id = 1;
time_t last_activity_time;

// --- UI & Utility Functions ---
void enable_virtual_terminal_processing() {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;
    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return;
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(hOut, dwMode)) return;
#endif
}

void clear_screen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void press_enter_to_continue() {
    printf(COLOR_YELLOW "\n\nPress Enter to continue..." COLOR_RESET);
    // clear_input_buffer(); // Not always needed, getchar() consumes the previous newline
    getchar();
}

void get_string_input(const char *prompt, char *buffer, int size) {
    printf("%s", prompt);
    fgets(buffer, size, stdin);
    buffer[strcspn(buffer, "\n")] = 0;
}

int get_int_input(const char *prompt) {
    int value;
    char buffer[100];
    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) && sscanf(buffer, "%d", &value) == 1) {
            return value;
        }
        printf(COLOR_RED "Invalid input. Please enter an integer.\n" COLOR_RESET);
    }
}

void get_masked_password(const char *prompt, char *buffer, int size) {
    printf("%s", prompt);
    fflush(stdout);
#ifdef _WIN32
    int i = 0;
    char ch;
    while (i < size - 1 && (ch = _getch()) != '\r') {
        if (ch == '\b') {
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
    struct flock fl = {F_WRLCK, SEEK_SET, 0, 0, 0};
    fl.l_pid = getpid();
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
    struct flock fl = {F_UNLCK, SEEK_SET, 0, 0, 0};
    fl.l_pid = getpid();
    if (fcntl(fileno(fp), F_SETLK, &fl) == -1) {
        perror("Failed to unlock file");
    }
#endif
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
        }
        books[book_count++] = temp;
        if (temp.id >= next_book_id) next_book_id = temp.id + 1;
    }
    fclose(file);
}

void save_books() {
    FILE *file = fopen(BOOK_FILE, "w");
    if (!file) { perror("Could not open books file"); return; }
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
        }
        members[member_count++] = temp;
        if (temp.id >= next_member_id) next_member_id = temp.id + 1;
    }
    fclose(file);
}

void save_members() {
    FILE *file = fopen(MEMBER_FILE, "w");
    if (!file) { perror("Could not open members file"); return; }
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
        }
        transactions[transaction_count++] = temp;
        if (temp.transaction_id >= next_transaction_id) next_transaction_id = temp.transaction_id + 1;
    }
    fclose(file);
}

void save_transactions() {
    FILE *file = fopen(TRANSACTION_FILE, "w");
    if (!file) { perror("Could not open transactions file"); return; }
    lock_file(file);
    for (int i = 0; i < transaction_count; i++) {
        fprintf(file, "%d,%d,%d,%ld,%ld,%ld,%.2f\n", transactions[i].transaction_id, transactions[i].book_id, transactions[i].member_id, (long)transactions[i].borrow_date, (long)transactions[i].due_date, (long)transactions[i].return_date, transactions[i].fine);
    }
    unlock_file(file);
    fclose(file);
}

// --- Find Functions ---
Book* find_book_by_id(int id) { for (int i = 0; i < book_count; i++) if (books[i].id == id) return &books[i]; return NULL; }
Member* find_member_by_id(int id) { for (int i = 0; i < member_count; i++) if (members[i].id == id) return &members[i]; return NULL; }
Member* find_member_by_name(const char* name) { for (int i = 0; i < member_count; i++) if (strcmp(members[i].name, name) == 0) return &members[i]; return NULL; }

// --- Password Validators ---
int is_strong_admin_password(const char *pass) { int len=strlen(pass); if(len<12)return 0; int u=0,l=0,d=0,s=0; for(int i=0;i<len;i++){if(isupper(pass[i]))u=1;else if(islower(pass[i]))l=1;else if(isdigit(pass[i]))d=1;else if(ispunct(pass[i]))s=1;} return u&&l&&d&&s;}
int is_valid_member_password(const char *pass) { int len=strlen(pass); if(len<8)return 0; int d=0; for(int i=0;i<len;i++){if(isdigit(pass[i]))d=1;} return d;}

// --- Pagination Display Functions ---
void display_books_paginated(int current_page) {
    int total_pages = (book_count + ITEMS_PER_PAGE - 1) / ITEMS_PER_PAGE;
    if (total_pages == 0) total_pages = 1;

    clear_screen();
    printf(COLOR_CYAN "====================================================================================================\n" COLOR_RESET);
    printf("                                         List of All Books\n");
    printf(COLOR_CYAN "====================================================================================================\n" COLOR_RESET);
    printf("%-5s | %-30s | %-20s | %-15s | %-8s | %-8s\n", "ID", "Title", "Author", "Category", "Total", "Available");
    printf("----------------------------------------------------------------------------------------------------\n");

    int start = current_page * ITEMS_PER_PAGE;
    int end = start + ITEMS_PER_PAGE;
    if (end > book_count) end = book_count;

    if (book_count == 0) {
        printf("No books in the library.\n");
    } else {
        for (int i = start; i < end; i++) {
            printf("%-5d | %-30s | %-20s | %-15s | %-8d | %-8d\n", books[i].id, books[i].title, books[i].author, books[i].category, books[i].quantity, books[i].available);
        }
    }
    printf("----------------------------------------------------------------------------------------------------\n");
    printf(COLOR_YELLOW "--- Page %d of %d ---\n" COLOR_RESET, current_page + 1, total_pages);
}

void display_transactions_paginated(int current_page) {
    int total_pages = (transaction_count + ITEMS_PER_PAGE - 1) / ITEMS_PER_PAGE;
    if (total_pages == 0) total_pages = 1;

    clear_screen();
    printf(COLOR_CYAN "===========================================================================================\n" COLOR_RESET);
    printf("                                     All Transactions\n");
    printf(COLOR_CYAN "===========================================================================================\n" COLOR_RESET);
    printf("%-5s | %-10s | %-10s | %-20s | %-20s | %-10s\n", "ID", "Book ID", "Member ID", "Borrow Date", "Return Date", "Fine");
    printf("-------------------------------------------------------------------------------------------\n");

    int start = current_page * ITEMS_PER_PAGE;
    int end = start + ITEMS_PER_PAGE;
    if (end > transaction_count) end = transaction_count;

    if (transaction_count == 0) {
        printf("No transactions found.\n");
    } else {
        char borrow_date_str[30], return_date_str[30];
        for (int i = start; i < end; i++) {
            strftime(borrow_date_str, sizeof(borrow_date_str), "%Y-%m-%d %H:%M", localtime(&transactions[i].borrow_date));
            if (transactions[i].return_date != 0) {
                strftime(return_date_str, sizeof(return_date_str), "%Y-%m-%d %H:%M", localtime(&transactions[i].return_date));
            } else {
                strcpy(return_date_str, "Not yet returned");
            }
            printf("%-5d | %-10d | %-10d | %-20s | %-20s | $" COLOR_YELLOW "%-9.2f" COLOR_RESET "\n", transactions[i].transaction_id, transactions[i].book_id, transactions[i].member_id, borrow_date_str, return_date_str, transactions[i].fine);
        }
    }
    printf("-------------------------------------------------------------------------------------------\n");
    printf(COLOR_YELLOW "--- Page %d of %d ---\n" COLOR_RESET, current_page + 1, total_pages);
}

// --- Admin Functions ---
void add_book() {
    clear_screen(); printf(COLOR_CYAN "===================================\n" "          Add a New Book\n" "===================================\n\n" COLOR_RESET);
    if (book_count >= book_capacity) { book_capacity = (book_capacity == 0) ? 10 : book_capacity * 2; books = realloc(books, book_capacity * sizeof(Book)); }
    Book *nb = &books[book_count]; nb->id = next_book_id++;
    get_string_input("Book Title: ", nb->title, 100); get_string_input("Author: ", nb->author, 50); get_string_input("Category: ", nb->category, 30);
    nb->quantity = get_int_input("Total Quantity: "); nb->available = nb->quantity;
    book_count++; save_books(); printf(COLOR_GREEN "\nBook added successfully! Book ID: %d\n" COLOR_RESET, nb->id);
}

void delete_book() {
    clear_screen(); printf(COLOR_CYAN "===================================\n" "          Delete a Book\n" "===================================\n\n" COLOR_RESET);
    int id = get_int_input("Enter Book ID to delete: ");
    int found_index = -1;
    for (int i = 0; i < book_count; i++) if (books[i].id == id) { found_index = i; break; }
    if (found_index == -1) { printf(COLOR_RED "Book not found.\n" COLOR_RESET); return; }
    for (int i = found_index; i < book_count - 1; i++) books[i] = books[i + 1];
    book_count--; save_books(); printf(COLOR_GREEN "Book deleted successfully.\n" COLOR_RESET);
}

void list_all_books() {
    int current_page = 0;
    int total_pages = (book_count + ITEMS_PER_PAGE - 1) / ITEMS_PER_PAGE;
    if(total_pages == 0) total_pages = 1;
    char choice;
    do {
        display_books_paginated(current_page);
        printf("Enter (N)ext, (P)revious, or (Q)uit to menu: ");
        choice = getchar(); clear_input_buffer();
        switch (tolower(choice)) {
            case 'n': if (current_page < total_pages - 1) current_page++; break;
            case 'p': if (current_page > 0) current_page--; break;
        }
    } while (tolower(choice) != 'q');
}

void add_member() {
    clear_screen(); printf(COLOR_CYAN "===================================\n" "         Add a New Member\n" "===================================\n\n" COLOR_RESET);
    if (member_count >= member_capacity) { member_capacity = (member_capacity == 0) ? 10 : member_capacity * 2; members = realloc(members, member_capacity * sizeof(Member)); }
    Member *nm = &members[member_count]; nm->id = next_member_id++;
    get_string_input("Member Name: ", nm->name, 50); get_string_input("Email: ", nm->email, 100);
    char password[256];
    while (1) {
        get_masked_password("Enter initial password (min 8 chars, 1 number): ", password, sizeof(password));
        if (is_valid_member_password(password)) break;
        printf(COLOR_RED "Weak password. Must be at least 8 characters and contain one number.\n" COLOR_RESET);
    }
    caesar_encrypt(password, nm->encrypted_password); nm->is_first_login = 1;
    member_count++; save_members(); printf(COLOR_GREEN "\nMember added successfully! Member ID: %d\n" COLOR_RESET, nm->id);
}

void delete_member() {
    clear_screen(); printf(COLOR_CYAN "===================================\n" "         Delete a Member\n" "===================================\n\n" COLOR_RESET);
    int id = get_int_input("Enter Member ID to delete: ");
    int found_index = -1;
    for (int i = 0; i < member_count; i++) if (members[i].id == id) { found_index = i; break; }
    if (found_index == -1) { printf(COLOR_RED "Member not found.\n" COLOR_RESET); return; }
    for (int i = found_index; i < member_count - 1; i++) members[i] = members[i + 1];
    member_count--; save_members(); printf(COLOR_GREEN "Member deleted successfully.\n" COLOR_RESET);
}

void view_all_transactions() {
    int current_page = 0;
    int total_pages = (transaction_count + ITEMS_PER_PAGE - 1) / ITEMS_PER_PAGE;
    if(total_pages == 0) total_pages = 1;
    char choice;
    do {
        display_transactions_paginated(current_page);
        printf("Enter (N)ext, (P)revious, or (Q)uit to menu: ");
        choice = getchar(); clear_input_buffer();
        switch (tolower(choice)) {
            case 'n': if (current_page < total_pages - 1) current_page++; break;
            case 'p': if (current_page > 0) current_page--; break;
        }
    } while (tolower(choice) != 'q');
}

void reset_member_password() {
    clear_screen(); printf(COLOR_CYAN "===================================\n" "      Reset Member Password\n" "===================================\n\n" COLOR_RESET);
    int member_id = get_int_input("Enter member ID: ");
    Member* member = find_member_by_id(member_id);
    if (!member) { printf(COLOR_RED "Member not found.\n" COLOR_RESET); return; }
    char new_password[256];
    while(1) {
        get_masked_password("Enter new password for member: ", new_password, sizeof(new_password));
        if (is_valid_member_password(new_password)) break;
        printf(COLOR_RED "Weak password. Must be at least 8 characters and contain one number.\n" COLOR_RESET);
    }
    caesar_encrypt(new_password, member->encrypted_password); member->is_first_login = 1;
    save_members(); printf(COLOR_GREEN "\nPassword has been reset successfully.\n" COLOR_RESET);
}

// --- Member Functions ---
void search_books() {
    clear_screen(); printf(COLOR_CYAN "===================================\n" "        Search for a Book\n" "===================================\n\n" COLOR_RESET);
    printf("1. Search by Title\n2. Search by Author\n3. Search by Category\n");
    int choice = get_int_input("\nChoose search method: ");
    char query[100]; get_string_input("Enter search term: ", query, sizeof(query));
    int found = 0;
    
    // Convert query to lower case
    char lower_query[100];
    for(int j=0; query[j]; j++){ lower_query[j] = tolower(query[j]); }
    lower_query[strlen(query)] = '\0';
    
    clear_screen();
    printf(COLOR_CYAN "====================================================================================================\n" "                                         Search Results\n" "====================================================================================================\n" COLOR_RESET);
    printf("%-5s | %-30s | %-20s | %-15s | %-8s | %-8s\n", "ID", "Title", "Author", "Category", "Total", "Available");
    printf("----------------------------------------------------------------------------------------------------\n");
    
    for (int i = 0; i < book_count; i++) {
        int match = 0;
        char lower_field[100]; // Generic buffer for fields
        const char *field_to_check = NULL;
        
        switch(choice) {
            case 1: field_to_check = books[i].title; break;
            case 2: field_to_check = books[i].author; break;
            case 3: field_to_check = books[i].category; break;
            default: printf(COLOR_RED "Invalid choice.\n" COLOR_RESET); return;
        }

        // Convert field to lower case
        for(int j=0; field_to_check[j]; j++){ lower_field[j] = tolower(field_to_check[j]); }
        lower_field[strlen(field_to_check)] = '\0';

        if (strstr(lower_field, lower_query)) {
            printf("%-5d | %-30s | %-20s | %-15s | %-8d | %-8d\n", books[i].id, books[i].title, books[i].author, books[i].category, books[i].quantity, books[i].available);
            found = 1;
        }
    }
    if (!found) { printf("No books found matching your search.\n"); }
}

void borrow_book(int member_id) {
    int current_page = 0;
    int total_pages = (book_count + ITEMS_PER_PAGE - 1) / ITEMS_PER_PAGE;
    if(total_pages == 0) total_pages = 1;
    char choice;
    do {
        display_books_paginated(current_page);
        printf(COLOR_CYAN "Enter Book ID to borrow, or (N)ext, (P)revious, (Q)uit: " COLOR_RESET);
        char input_buffer[100];
        get_string_input("", input_buffer, sizeof(input_buffer));
        choice = tolower(input_buffer[0]);

        if (isdigit(choice)) {
            int book_id = atoi(input_buffer);
            Book* book = find_book_by_id(book_id);
            if (!book) { printf(COLOR_RED "Book not found.\n" COLOR_RESET); } 
            else if (book->available <= 0) { printf(COLOR_RED "Sorry, this book is currently unavailable.\n" COLOR_RESET); } 
            else {
                if (transaction_count >= transaction_capacity) { transaction_capacity = (transaction_capacity==0)?20:transaction_capacity*2; transactions = realloc(transactions, transaction_capacity * sizeof(Transaction)); }
                Transaction *nt = &transactions[transaction_count]; nt->transaction_id = next_transaction_id++; nt->book_id = book_id; nt->member_id = member_id; nt->borrow_date = time(NULL); nt->due_date = nt->borrow_date + (BORROW_DURATION_DAYS * 24 * 60 * 60); nt->return_date = 0; nt->fine = 0.0;
                transaction_count++; book->available--; save_books(); save_transactions();
                char due_date_str[30]; strftime(due_date_str, sizeof(due_date_str), "%Y-%m-%d", localtime(&nt->due_date));
                printf(COLOR_GREEN "\nBook borrowed successfully. The due date is: %s\n" COLOR_RESET, due_date_str);
            }
            press_enter_to_continue();
            return;
        } else {
            switch (choice) {
                case 'n': if (current_page < total_pages - 1) current_page++; break;
                case 'p': if (current_page > 0) current_page--; break;
                case 'q': break;
            }
        }
    } while (choice != 'q');
}

void return_book(int member_id) {
    clear_screen(); printf(COLOR_CYAN "===================================\n" "          Return a Book\n" "===================================\n\n" COLOR_RESET);
    printf("Books you have borrowed:\n");
    printf("%-15s | %-30s\n", "Transaction ID", "Book Title");
    printf("----------------------------------------------\n");
    int active_count = 0;
    for (int i = 0; i < transaction_count; i++) {
        if (transactions[i].member_id == member_id && transactions[i].return_date == 0) {
            Book* book = find_book_by_id(transactions[i].book_id);
            if (book) { printf("%-15d | %-30s\n", transactions[i].transaction_id, book->title); active_count++; }
        }
    }
    if (active_count == 0) { printf("\nYou have no books to return.\n"); return; }
    int trans_id = get_int_input("\nEnter the transaction ID for the book to return: ");
    Transaction* trans = NULL;
    for (int i = 0; i < transaction_count; i++) if (transactions[i].transaction_id == trans_id && transactions[i].member_id == member_id && transactions[i].return_date == 0) { trans = &transactions[i]; break; }
    if (!trans) { printf(COLOR_RED "\nInvalid transaction ID or book already returned.\n" COLOR_RESET); return; }
    trans->return_date = time(NULL);
    if (trans->return_date > trans->due_date) {
        double seconds_late = difftime(trans->return_date, trans->due_date);
        int days_late = (int)(seconds_late / (60 * 60 * 24)) + 1;
        trans->fine = days_late * FINE_PER_DAY;
        printf(COLOR_YELLOW "\nThe book is overdue! A fine of $%.2f has been charged.\n" COLOR_RESET, trans->fine);
    } else {
        printf(COLOR_GREEN "\nThank you for returning the book on time.\n" COLOR_RESET);
    }
    Book* book = find_book_by_id(trans->book_id); if (book) book->available++;
    save_books(); save_transactions(); printf(COLOR_GREEN "Book returned successfully.\n" COLOR_RESET);
}

void view_my_records(int member_id) {
    clear_screen(); printf(COLOR_CYAN "================================================================================\n" "                             My Personal Records\n" "================================================================================\n" COLOR_RESET);
    printf("%-5s | %-20s | %-12s | %-12s | %-10s\n", "ID", "Book Title", "Borrow Date", "Return Date", "Fine");
    printf("--------------------------------------------------------------------------------\n");
    int found = 0;
    char borrow_date_str[20], return_date_str[20];
    for (int i = 0; i < transaction_count; i++) {
        if (transactions[i].member_id == member_id) {
            Book* book = find_book_by_id(transactions[i].book_id); if (!book) continue;
            strftime(borrow_date_str, sizeof(borrow_date_str), "%Y-%m-%d", localtime(&transactions[i].borrow_date));
            if (transactions[i].return_date != 0) { strftime(return_date_str, sizeof(return_date_str), "%Y-%m-%d", localtime(&transactions[i].return_date)); } 
            else { strcpy(return_date_str, "Not returned"); }
            printf("%-5d | %-20s | %-12s | %-12s | $" COLOR_YELLOW "%-9.2f" COLOR_RESET "\n", transactions[i].transaction_id, book->title, borrow_date_str, return_date_str, transactions[i].fine);
            found = 1;
        }
    }
    if (!found) { printf("No records found.\n"); }
    printf("--------------------------------------------------------------------------------\n");
}

// --- Menus & Core Logic ---
void admin_menu();
void member_menu(int member_id);

int check_session_timeout() {
    if (time(NULL) - last_activity_time > SESSION_TIMEOUT_SECONDS) {
        clear_screen(); printf(COLOR_RED "\nSession timed out due to inactivity. You have been logged out.\n" COLOR_RESET); press_enter_to_continue();
        return 1;
    }
    last_activity_time = time(NULL); return 0;
}

void change_password(Member *member, int is_admin) {
    char new_pass[256], confirm_pass[256];
    clear_screen(); printf(COLOR_CYAN "===================================\n" "    Mandatory Password Change\n" "===================================\n\n" COLOR_RESET);
    while(1) {
        if (is_admin) { get_masked_password("Enter new password (min 12, upper/lower, num, symbol): ", new_pass, sizeof(new_pass)); if (!is_strong_admin_password(new_pass)) { printf(COLOR_RED "Password does not meet the security requirements.\n" COLOR_RESET); continue; } } 
        else { get_masked_password("Enter new password (min 8, at least 1 number): ", new_pass, sizeof(new_pass)); if (!is_valid_member_password(new_pass)) { printf(COLOR_RED "Password does not meet the security requirements.\n" COLOR_RESET); continue; } }
        get_masked_password("Confirm new password: ", confirm_pass, sizeof(confirm_pass));
        if (strcmp(new_pass, confirm_pass) != 0) { printf(COLOR_RED "Passwords do not match. Please try again.\n" COLOR_RESET); } 
        else { caesar_encrypt(new_pass, member->encrypted_password); member->is_first_login = 0; save_members(); printf(COLOR_GREEN "\nPassword changed successfully.\n" COLOR_RESET); press_enter_to_continue(); break; }
    }
}

void login() {
    clear_screen(); printf(COLOR_CYAN "===================================\n" "              Login\n" "===================================\n\n" COLOR_RESET);
    printf("1. Login as Librarian (Admin)\n2. Login as Member\n");
    int choice = get_int_input("\nSelect user type: ");
    char username[50]; get_string_input("Username: ", username, sizeof(username));
    int login_attempts = 0;
    while(login_attempts < MAX_LOGIN_ATTEMPTS) {
        char password[256]; get_masked_password("Password: ", password, sizeof(password));
        Member* member = (choice == 1 && strcmp(username, "admin") == 0) ? find_member_by_name("admin") : find_member_by_name(username);
        if (member) {
            char decrypted_pass[256]; caesar_decrypt(member->encrypted_password, decrypted_pass);
            if (strcmp(password, decrypted_pass) == 0) {
                printf(COLOR_GREEN "\nLogin successful.\n" COLOR_RESET); press_enter_to_continue(); last_activity_time = time(NULL);
                if (member->is_first_login) change_password(member, (choice == 1));
                if (choice == 1) admin_menu(); else member_menu(member->id);
                return;
            }
        }
        login_attempts++; printf(COLOR_RED "Incorrect username or password. Attempts remaining: %d\n" COLOR_RESET, MAX_LOGIN_ATTEMPTS - login_attempts);
    }
    printf(COLOR_RED "Maximum login attempts exceeded.\n" COLOR_RESET); press_enter_to_continue();
}

void admin_menu() {
    int choice;
    do {
        if (check_session_timeout()) return;
        clear_screen();
        printf(COLOR_CYAN "===================================\n" "          Librarian Menu\n" "===================================\n" COLOR_RESET);
        printf("1. Add Book\n2. Delete Book\n3. View All Books\n4. Add Member\n5. Delete Member\n6. View All Transactions\n7. Reset Member Password\n8. Logout\n");
        choice = get_int_input("\nSelect an option: ");
        switch(choice) {
            case 1: add_book(); press_enter_to_continue(); break;
            case 2: delete_book(); press_enter_to_continue(); break;
            case 3: list_all_books(); break;
            case 4: add_member(); press_enter_to_continue(); break;
            case 5: delete_member(); press_enter_to_continue(); break;
            case 6: view_all_transactions(); break;
            case 7: reset_member_password(); press_enter_to_continue(); break;
            case 8: printf(COLOR_YELLOW "Logged out.\n" COLOR_RESET); press_enter_to_continue(); break;
            default: printf(COLOR_RED "Invalid option.\n" COLOR_RESET); press_enter_to_continue();
        }
    } while (choice != 8);
}

void member_menu(int member_id) {
    int choice;
    do {
        if (check_session_timeout()) return;
        clear_screen();
        printf(COLOR_CYAN "===================================\n" "            Member Menu\n" "===================================\n" COLOR_RESET);
        printf("1. Search for a Book\n2. Borrow a Book\n3. Return a Book\n4. View My Records\n5. Logout\n");
        choice = get_int_input("\nSelect an option: ");
        switch(choice) {
            case 1: search_books(); press_enter_to_continue(); break;
            case 2: borrow_book(member_id); break;
            case 3: return_book(member_id); press_enter_to_continue(); break;
            case 4: view_my_records(member_id); press_enter_to_continue(); break;
            case 5: printf(COLOR_YELLOW "Logged out.\n" COLOR_RESET); press_enter_to_continue(); break;
            default: printf(COLOR_RED "Invalid option.\n" COLOR_RESET); press_enter_to_continue();
        }
    } while (choice != 5);
}

void initialize_system() {
    load_books(); load_members(); load_transactions();
    if (member_count == 0) {
        printf(COLOR_YELLOW "No users found. Creating a default admin account.\n" "Username: admin\n" "Password: AdminPassword123!\n" COLOR_RESET);
        Member admin; admin.id = next_member_id++; strcpy(admin.name, "admin"); strcpy(admin.email, "admin@library.com"); admin.is_first_login = 1;
        caesar_encrypt("AdminPassword123!", admin.encrypted_password);
        if (member_count >= member_capacity) { member_capacity = 10; members = realloc(members, member_capacity * sizeof(Member)); }
        members[member_count++] = admin; save_members(); press_enter_to_continue();
    }
}

int main() {
    enable_virtual_terminal_processing();
    initialize_system();
    int choice;
    do {
        clear_screen();
        printf(COLOR_CYAN "===================================\n" "    Library Management System\n" "===================================\n\n" COLOR_RESET);
        printf("1. Login\n2. Exit\n");
        choice = get_int_input("\nChoose an option: ");
        switch (choice) {
            case 1: login(); break;
            case 2: clear_screen(); printf(COLOR_GREEN "Saving data... Goodbye!\n" COLOR_RESET); break;
            default: printf(COLOR_RED "Invalid choice. Please try again.\n" COLOR_RESET); press_enter_to_continue();
        }
    } while (choice != 2);
    free(books); free(members); free(transactions);
    return 0;
}
