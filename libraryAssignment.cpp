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

typedef struct {
    int id;
    char title[TITLE_LEN];
    char author[AUTHOR_LEN];
    char isbn[ISBN_LEN];
    int available;
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

int registerUser(LibrarySystem* lib, const char* username, const char* password, const char* name);
int addBook(LibrarySystem* lib, const char* title, const char* author, const char* isbn);

void initializeSystem(LibrarySystem* lib) {
    lib->bookCount = 0;
    lib->userCount = 0;
    lib->currentUser = NULL;
    
    // Add sample books
    addBook(lib, "The Great Gatsby", "F. Scott Fitzgerald", "9780743273565");
    addBook(lib, "1984", "George Orwell", "9780452284234");
    
    // Add sample users
    registerUser(lib, "admin", "admin123", "Admin User");
    registerUser(lib, "user1", "password", "John Doe");
}

int registerUser(LibrarySystem* lib, const char* username, const char* password, const char* name) {
    if (lib->userCount >= MAX_USERS) return 0;
    
    for (int i = 0; i < lib->userCount; i++) {
        if (strcmp(lib->users[i].username, username) == 0) {
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

int addBook(LibrarySystem* lib, const char* title, const char* author, const char* isbn) {
    if (lib->bookCount >= MAX_BOOKS) return 0;
    
    Book* newBook = &lib->books[lib->bookCount++];
    newBook->id = lib->bookCount;
    strcpy(newBook->title, title);
    strcpy(newBook->author, author);
    strcpy(newBook->isbn, isbn);
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
            // Remove from user's borrowed list
            for (int j = i; j < lib->currentUser->borrowedCount - 1; j++) {
                lib->currentUser->borrowedBooks[j] = lib->currentUser->borrowedBooks[j + 1];
            }
            lib->currentUser->borrowedCount--;
            
            // Mark book as available
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
    char username[USERNAME_LEN], password[PASSWORD_LEN], name[30];
    char title[TITLE_LEN], author[AUTHOR_LEN], isbn[ISBN_LEN];
    
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
                
            case 0:
                printf("Goodbye!\n");
                return 0;
                
            default:
                printf("Invalid choice.\n");
        }
    }
    
    return 0;
}
