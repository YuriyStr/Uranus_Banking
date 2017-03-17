#include <stdio.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define NO_USER 0
#define ADMIN 1
#define OPER 2

sqlite3 *db;
sqlite3_stmt *res;
int rc;
int currUser = NO_USER;
int working = 1;

void performPreparations();

void showDefaultMenu();
void showAdminMenu();
void showOperMenu();
void login();
void logout();

void addAccount(char *cardNo);
void modifyAccount(char *cardNo);
void deleteAccount(char *cardNo);
void addClient(char *passportNo);
void modifyClient(char *passportNo);
void deleteClient(char *passportNo);
void checkConfig();

void credit(char *passportNo, char *cardNo, double money);
void debit (char *passportNo, char *cardNo, double money);
void transfer(char *passportNo, char *cardNoFrom, char *cardNoTo, double money);
void checkBalance(char *passportNo, char *cardNo);

int main()
{
    rc = sqlite3_open("./TP_Lab5/Bank.db", &db);
    if (rc)
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return 1;
    }
    
    performPreparations();
    while(working)
    {
        if (currUser == NO_USER)
        {
            showDefaultMenu();
        }
        else if (currUser == ADMIN)
        {
            showAdminMenu();
        }
        else if (currUser == OPER)
        {
            showOperMenu();
        }
        else
            break;
    }
    
    sqlite3_close(db);
    return 0;
}

void performPreparations()
{
    // TODO
}

void showDefaultMenu()
{
    printf("1 - Login\n");
    printf("0 - Quit\n");
    int code;
    scanf ("%d", &code);
    if (code == 1)
    {
        login();
    }
    else if (code == 0)
        working = 0;
    else
        printf("Invalid command\n");
}

void showAdminMenu()
{
    int code;
    printf ("1 - Add account\n");
    printf ("2 - Modify account information\n");
    printf ("3 - Delete account\n");
    printf ("4 - Add client\n");
    printf ("5 - Modify client information\n");
    printf ("6 - Delete client\n");
    printf ("7 - Check configuration info\n");
    printf ("0 - Logout\n");
    scanf ("%d", &code);
    
    switch (code)
    {
        case 1:
        {
            addAccount (NULL);
            break;
        }
            
        case 2:
        {
            modifyAccount (NULL);
            break;
        }
            
        case 3:
        {
            deleteAccount(NULL);
            break;
        }
            
        case 4:
        {
            addClient (NULL);
            break;
        }
            
        case 5:
        {
            modifyClient (NULL);
            break;
        }
            
        case 6:
        {
            deleteClient(NULL);
            break;
        }
            
        case 7:
        {
            checkConfig();
            break;
        }
            
        case 0:
        {
            logout();
            break;
        }
            
        default:
        {
            printf ("Invalid command\n");
            break;
        }
    }

}

void showOperMenu()
{
    int code;
    printf ("1 - Credit money\n");
    printf ("2 - Debit money\n");
    printf ("3 - Transfer money\n");
    printf ("4 - Check balance\n");
    printf ("0 - Logout\n");
    scanf("%d", &code);
    
    switch (code)
    {
        case 1:
        {
            credit (NULL, NULL, 0);
            break;
        }
            
        case 2:
        {
            debit (NULL, NULL, 0);
            break;
        }
            
        case 3:
        {
            transfer(NULL, NULL, NULL, 0);
            break;
        }
        
        case 4:
        {
            checkBalance (NULL, NULL);
            break;
        }
            
        case 0:
        {
            logout();
            break;
        }
            
        default:
        {
            printf ("Invalid command\n");
            break;
        }
    }
}

void login()
{
    for (;;)
    {
        char *sql;
        char log[100];
        char *pass;
        char *req_pass;
        char *role;
        
        sql = "SELECT * FROM BANK_USERS WHERE Login = ?";
        
        printf("Enter your login (or \'-\' to quit): ");
        scanf("%s", log);
        if (strcmp(log, "-") == 0)
            return;
        
        rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
        
        if (rc == SQLITE_OK)
        {
            sqlite3_bind_text(res, 1, log, -1, SQLITE_TRANSIENT);
        }
        else
        {
            fprintf (stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
            return;
        }
        
        if (sqlite3_step(res) == SQLITE_ROW)
        {
            req_pass = (char*)sqlite3_column_text(res, 3);
            role = (char*)sqlite3_column_text(res, 1);
        }
        else
        {
            printf("No such login\n");
            continue;
        }
        
        pass = getpass("Enter password: ");
        if (strcmp(pass, req_pass) != 0)
        {
            printf("Wrong password\n");
            continue;
        }
        
        if (strcmp(role, "Admin") == 0)
            currUser = ADMIN;
        else if (strcmp(role, "Oper") == 0)
            currUser = OPER;
        break;
    }
}

void logout()
{
    currUser = NO_USER;
}



void addAccount(char *cardNo)
{
    // TODO
}

void modifyAccount(char *cardNo)
{
    // TODO
}

void deleteAccount(char *cardNo)
{
    // TODO
}

void addClient(char *passportNo)
{
    // TODO
}

void modifyClient(char *passportNo)
{
    // TODO
}

void deleteClient(char *passportNo)
{
    // TODO
}

void checkConfig()
{
    // TODO
}


void credit(char *passportNo, char *cardNo, double money)
{
    // TODO
}

void debit(char *passportNo, char *cardNo, double money)
{
    // TODO
}

void transfer(char *passportNo, char *cardNoFrom, char *cardNoTo, double money)
{
    // TODO
}

void checkBalance(char *passportNo, char *cardNo)
{
    // TODO
}