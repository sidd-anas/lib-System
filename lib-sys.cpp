#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_BOOKS 100
#define MAX_USERS 50
#define MAX_BORROWED 5
#define USERNAME_LEN 20
#define PASSWORD_LEN 20
#define TITLE_LEN 50
#define AUTHOR_LEN 30
#define ISBN_LEN 15
#define CONTENT_LEN 500

typedef struct {
    int id;
    char title[TITLE_LEN];
    char author[AUTHOR_LEN];
    char isbn[ISBN_LEN];
    int available;
    char content[CONTENT_LEN];
} Book;

typedef struct {
    int id;
    char username[USERNAME_LEN];
    char password[PASSWORD_LEN];
    char name[30];
    int borrowedBooks[MAX_BORROWED];
    int borrowedCount;
} User;

typedef struct {
    Book books[MAX_BOOKS];
    User users[MAX_USERS];
    User* currentUser;
    int bookCount;
    int userCount;
} LibrarySystem;

// Function declarations
int registerUser(LibrarySystem* lib, const char* username, const char* password, const char* name);
int addBook(LibrarySystem* lib, const char* title, const char* author, const char* isbn, const char* content);

int isValidUsername(const char* username) {
    int hasLower = 0, hasUpper = 0, hasDigit = 0;

    for (int i = 0; username[i] != '\0'; i++) {
        char c = username[i];

        if (islower(c)) hasLower = 1;
        else if (isupper(c)) hasUpper = 1;
        else if (isdigit(c)) hasDigit = 1;
        else if (c == '-' || c == '/' || c == '@' || c == '#') {
            // allowed special characters
        } else {
            return 0; // invalid character
        }
    }

    return (hasLower && hasUpper && hasDigit);
}

void initializeSystem(LibrarySystem* lib) {
    lib->bookCount = 0;
    lib->userCount = 0;
    lib->currentUser = NULL;

    // Sample books with content
    addBook(lib, "The Great Gatsby", "F. Scott Fitzgerald", "9780743273565",
            "In my younger and more vulnerable years my father gave me some advice...");
    addBook(lib, "1984", "George Orwell", "9780452284234",
            "It was a bright cold day in April, and the clocks were striking thirteen...");

    // Sample users
    registerUser(lib, "Admin123@", "admin123", "Admin User");
    registerUser(lib, "User1#1", "password", "John Doe");
}

int registerUser(LibrarySystem* lib, const char* username, const char* password, const char* name) {
    if (lib->userCount >= MAX_USERS) return 0;

    if (!isValidUsername(username)) {
        printf("Error: Username must contain at least one lowercase, one uppercase, one digit,\n");
        printf("and may only include special characters: - / @ #\n");
        return 0;
    }

    for (int i = 0; i < lib->userCount; i++) {
        if (strcmp(lib->users[i].username, username) == 0) {
            printf("Error: Username already exists.\n");
            return 0;
        }
    }

    User* newUser = &lib->users[lib->userCount++];
    newUser->id = lib->userCount;
    strcpy(newUser->username, username);
    strcpy(newUser->password, password);
    strcpy(newUser->name, name);
    newUser->borrowedCount = 0;

    return 1;
}

int login(LibrarySystem* lib, const char* username, const char* password) {
    for (int i = 0; i < lib->userCount; i++) {
        if (strcmp(lib->users[i].username, username) == 0 &&
            strcmp(lib->users[i].password, password) == 0) {
            lib->currentUser = &lib->users[i];
            return 1;
        }
    }
    return 0;
}

void logout(LibrarySystem* lib) {
    lib->currentUser = NULL;
}

int addBook(LibrarySystem* lib, const char* title, const char* author, const char* isbn, const char* content) {
    if (lib->bookCount >= MAX_BOOKS) return 0;

    Book* newBook = &lib->books[lib->bookCount++];
    newBook->id = lib->bookCount;
    strcpy(newBook->title, title);
    strcpy(newBook->author, author);
    strcpy(newBook->isbn, isbn);
    strcpy(newBook->content, content);
    newBook->available = 1;

    return 1;
}

int borrowBook(LibrarySystem* lib, int bookId) {
    if (!lib->currentUser || lib->currentUser->borrowedCount >= MAX_BORROWED) return 0;

    for (int i = 0; i < lib->bookCount; i++) {
        if (lib->books[i].id == bookId && lib->books[i].available) {
            lib->books[i].available = 0;
            lib->currentUser->borrowedBooks[lib->currentUser->borrowedCount++] = bookId;
            return 1;
        }
    }
    return 0;
}

int returnBook(LibrarySystem* lib, int bookId) {
    if (!lib->currentUser) return 0;

    for (int i = 0; i < lib->currentUser->borrowedCount; i++) {
        if (lib->currentUser->borrowedBooks[i] == bookId) {
            for (int j = i; j < lib->currentUser->borrowedCount - 1; j++) {
                lib->currentUser->borrowedBooks[j] = lib->currentUser->borrowedBooks[j + 1];
            }
            lib->currentUser->borrowedCount--;

            for (int j = 0; j < lib->bookCount; j++) {
                if (lib->books[j].id == bookId) {
                    lib->books[j].available = 1;
                    return 1;
                }
            }
        }
    }
    return 0;
}

void displayBooks(const LibrarySystem* lib) {
    printf("\n=== BOOKS ===\n");
    for (int i = 0; i < lib->bookCount; i++) {
        printf("ID: %d | %s by %s | Available: %s\n",
               lib->books[i].id, lib->books[i].title, lib->books[i].author,
               lib->books[i].available ? "Yes" : "No");
    }
}

void displayAvailableBooks(const LibrarySystem* lib) {
    printf("\n=== AVAILABLE BOOKS ===\n");
    for (int i = 0; i < lib->bookCount; i++) {
        if (lib->books[i].available) {
            printf("ID: %d | %s by %s\n",
                   lib->books[i].id, lib->books[i].title, lib->books[i].author);
        }
    }
}

void displayUserBooks(const LibrarySystem* lib) {
    if (!lib->currentUser) {
        printf("No user logged in.\n");
        return;
    }

    printf("\n=== YOUR BOOKS ===\n");
    if (lib->currentUser->borrowedCount == 0) {
        printf("No books borrowed.\n");
        return;
    }

    for (int i = 0; i < lib->currentUser->borrowedCount; i++) {
        for (int j = 0; j < lib->bookCount; j++) {
            if (lib->books[j].id == lib->currentUser->borrowedBooks[i]) {
                printf("ID: %d | %s by %s\n",
                       lib->books[j].id, lib->books[j].title, lib->books[j].author);
                break;
            }
        }
    }
}

void readBookContent(const LibrarySystem* lib, int bookId) {
    for (int i = 0; i < lib->bookCount; i++) {
        if (lib->books[i].id == bookId) {
            printf("\n=== READING BOOK: %s ===\n", lib->books[i].title);
            printf("%s\n", lib->books[i].content);
            return;
        }
    }
    printf("Book not found.\n");
}

void displayMenu() {
    printf("\n=== LIBRARY SYSTEM ===\n");
    printf("1. Login\n");
    printf("2. Register\n");
    printf("3. View Books\n");
    printf("4. View Available Books\n");
    printf("5. Borrow Book\n");
    printf("6. Return Book\n");
    printf("7. My Books\n");
    printf("8. Logout\n");
    printf("9. Read Borrowed Book Content\n");
    printf("10. Add Book (Admin Only)\n");
    printf("0. Exit\n");
    printf("Choice: ");
}

void clearInput() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int main() {
    LibrarySystem lib;
    initializeSystem(&lib);
    int choice, bookId;
    int hasBook;
    char username[USERNAME_LEN], password[PASSWORD_LEN], name[30];
    char title[TITLE_LEN], author[AUTHOR_LEN], isbn[ISBN_LEN], content[CONTENT_LEN];

    printf("Welcome to Library System!\n");

    while (1) {
        displayMenu();
        scanf("%d", &choice);
        clearInput();

        switch (choice) {
            case 1:
                printf("Username: ");
                fgets(username, USERNAME_LEN, stdin);
                username[strcspn(username, "\n")] = 0;

                printf("Password: ");
                fgets(password, PASSWORD_LEN, stdin);
                password[strcspn(password, "\n")] = 0;

                if (login(&lib, username, password)) {
                    printf("Welcome, %s!\n", lib.currentUser->name);
                } else {
                    printf("Login failed.\n");
                }
                break;

            case 2:
                printf("Note: Username must include lowercase, uppercase, digit, and only - / @ #\n");
                printf("Username: ");
                fgets(username, USERNAME_LEN, stdin);
                username[strcspn(username, "\n")] = 0;

                printf("Password: ");
                fgets(password, PASSWORD_LEN, stdin);
                password[strcspn(password, "\n")] = 0;

                printf("Name: ");
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = 0;

                if (registerUser(&lib, username, password, name)) {
                    printf("Registration successful!\n");
                } else {
                    printf("Registration failed.\n");
                }
                break;

            case 3:
                displayBooks(&lib);
                break;

            case 4:
                displayAvailableBooks(&lib);
                break;

            case 5:
                if (!lib.currentUser) {
                    printf("Please login first.\n");
                    break;
                }
                displayAvailableBooks(&lib);
                printf("Enter book ID to borrow: ");
                scanf("%d", &bookId);
                clearInput();

                if (borrowBook(&lib, bookId)) {
                    printf("Book borrowed!\n");
                } else {
                    printf("Borrow failed.\n");
                }
                break;

            case 6:
                if (!lib.currentUser) {
                    printf("Please login first.\n");
                    break;
                }
                displayUserBooks(&lib);
                printf("Enter book ID to return: ");
                scanf("%d", &bookId);
                clearInput();

                if (returnBook(&lib, bookId)) {
                    printf("Book returned!\n");
                } else {
                    printf("Return failed.\n");
                }
                break;

            case 7:
                displayUserBooks(&lib);
                break;

            case 8:
                logout(&lib);
                printf("Logged out.\n");
                break;

            case 9:
                if (!lib.currentUser) {
                    printf("Please login first.\n");
                    break;
                }
                displayUserBooks(&lib);
                printf("Enter book ID to read: ");
                scanf("%d", &bookId);
                clearInput();

                hasBook = 0;
                for (int i = 0; i < lib.currentUser->borrowedCount; i++) {
                    if (lib.currentUser->borrowedBooks[i] == bookId) {
                        hasBook = 1;
                        break;
                    }
                }

                if (hasBook) {
                    readBookContent(&lib, bookId);
                } else {
                    printf("You have not borrowed this book.\n");
                }
                break;

            case 10:
                if (!lib.currentUser || strcmp(lib.currentUser->username, "Admin123@") != 0) {
                    printf("Only admin can add books.\n");
                    break;
                }
                printf("Enter book title: ");
                fgets(title, TITLE_LEN, stdin);
                title[strcspn(title, "\n")] = 0;

                printf("Enter author: ");
                fgets(author, AUTHOR_LEN, stdin);
                author[strcspn(author, "\n")] = 0;

                printf("Enter ISBN: ");
                fgets(isbn, ISBN_LEN, stdin);
                isbn[strcspn(isbn, "\n")] = 0;

                printf("Enter book content: ");
                fgets(content, CONTENT_LEN, stdin);
                content[strcspn(content, "\n")] = 0;

                if (addBook(&lib, title, author, isbn, content)) {
                    printf("Book added successfully!\n");
                } else {
                    printf("Failed to add book.\n");
                }
                break;

            case 0:
                printf("Goodbye!\n");
                return 0;

            default:
                printf("Invalid choice.\n");
        }
    }

    return 0;
}
