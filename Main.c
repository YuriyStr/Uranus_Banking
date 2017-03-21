#include <stdio.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define NO_USER 0
#define ADMIN 1
#define OPER 2
#define CLIENT 3

sqlite3 *db;
sqlite3_stmt *res;
int rc;
int currUser = NO_USER;
int working = 1;
char currLogin[100];

void performPreparations();

void showDefaultMenu();
void showAdminMenu();
void showOperMenu();
void showClientMenu();

void showAdminQueries();
void showOperQueries();

void login();
void signup();
void logout();

void addAccount();
void modifyAccount();
void deleteAccount();
void addClient();
void modifyClient();
void deleteClient();

void addAccountFromBase(int query);
void modifyAccountFromBase(int query);
void deleteAccountFromBase(int query);
void addClientFromBase(int query);
void modifyClientFromBase(int query);
void deleteClientFromBase(int query);

void checkConfig();

void sendAdminQuery();
void sendOperQuery();

void credit(char *passportNo, char *cardNo, double money);
void debit (char *passportNo, char *cardNo, double money);
void transfer(char *passportNo, char *cardNoFrom, char *cardNoTo, double money);
void checkBalance(char *passportNo, char *cardNo);

void (* adminQueries[])() = {addClient, modifyClient, deleteClient, addAccount, modifyAccount, deleteAccount };
void (* adminQueriesFromBase[])(int) = {addClientFromBase, modifyClientFromBase, deleteClientFromBase, addAccountFromBase, modifyAccountFromBase, deleteAccountFromBase };
void (*operQueries[])(char*, char*, double) = { credit, debit };

static int callbackQueries(void *data, int argc, char **argv, char **azColName)
{
    if (argc == 0)
    {
        printf ("No queries found\n\n");
        return 0;
    }
    int i;
    for(i = 0; i < argc; i++)
    {
        printf("%s = %s\n", azColName[i], argv[i]);
    }
    printf("\n");
    return 0;
}

int main()
{
    rc = sqlite3_open("./Uranus_Banking/Bank.db", &db);
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
        else if (currUser == CLIENT)
        {
            showClientMenu();
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
    printf ("2 - Sign up\n");
    printf("0 - Quit\n");
    int code;
    scanf ("%d", &code);
    if (code == 1)
    {
        login();
    }
    else if (code == 2)
    {
        signup();
    }
    else if (code == 0)
        working = 0;
    else
        printf("Invalid command\n\n");
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
    printf ("8 - Check client queries\n");
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
            
        case 8:
        {
            showAdminQueries();
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
    printf ("5 - Show client queries\n");
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
            
        case 5:
        {
            showOperQueries();
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

void showClientMenu()
{
    int code;
    printf ("1 - Send query to administrator\n");
    printf ("2 - Send query to operationsist\n");
    printf ("3 - Check balance\n");
    printf ("0 - Logout\n");
    scanf("%d", &code);
    
    switch (code)
    {
        case 1:
        {
            sendAdminQuery();
            break;
        }
            
        case 2:
        {
            sendAdminQuery();
            break;
        }
            
        case 3:
        {
            
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
        else if (strcmp(role, "Client") == 0)
            currUser = CLIENT;
        strcpy (currLogin, log);
        break;
    }
}

void signup()
{
    char log[100];
    char *pass;
    char *repPass;
    char firstName[100];
    char lastName[100];
    char *type;
    char *sql = "INSERT into ADMIN_QUERIES (PassportNo, FirstName, LastName, QueryType, QueryID, password) Values (@passport, @name, @surname, @type, 1, @password)";
    char *sel = "SELECT Type from ADMIN_QUERY_TYPES WHERE id = ?";
    
    rc = sqlite3_prepare_v2(db, sel, -1, &res, 0);
    sqlite3_bind_int(res, 1, 1);
    sqlite3_step(res);
    type = (char*)sqlite3_column_text(res, 0);
    
    printf("Enter your passport number: ");
    scanf("%s", log);
    printf ("Enter your first name: ");
    scanf("%s", firstName);
    printf ("Enter your last name: ");
    scanf("%s", lastName);
    
    do
    {
        pass = getpass("Enter your password: ");
        repPass = getpass("Repeat your password: ");
    } while (strcmp (pass, repPass) != 0);
    
    rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
    int idx = sqlite3_bind_parameter_index(res, "@passport");
    sqlite3_bind_text(res, idx, log, -1, SQLITE_TRANSIENT);
    
    idx = sqlite3_bind_parameter_index(res, "@name");
    sqlite3_bind_text(res, idx, firstName, -1, SQLITE_TRANSIENT);
    
    idx = sqlite3_bind_parameter_index(res, "@surname");
    sqlite3_bind_text(res, idx, lastName, -1, SQLITE_TRANSIENT);
    
    idx = sqlite3_bind_parameter_index(res, "@type");
    sqlite3_bind_text(res, idx, type, -1, SQLITE_TRANSIENT);
    
    idx = sqlite3_bind_parameter_index(res, "@password");
    sqlite3_bind_text(res, idx, pass, -1, SQLITE_TRANSIENT);
    
    if (sqlite3_step(res) != SQLITE_DONE)
        printf("Execution failed: %s\n\n", sqlite3_errmsg(db));
    else
        printf ("Your data has been sent to administrator for verification. Thank you for choosing Uranus Banking\n\n");
}

void logout()
{
    currUser = NO_USER;
}

void showAdminQueries()
{
    int code;
    char *zErrMsg = 0;
    for(;;)
    {
        sqlite3_exec(db, "SELECT * FROM ADMIN_QUERIES;", callbackQueries, NULL, &zErrMsg);
        printf ("<ID> - perform query\n");
        printf ("0 - Exit\n");
        int accept;
        scanf("%d", &code);
        if (code == 0)
            break;
        printf ("1 - accept query\n");
        printf ("2 - deny query\n");
        scanf("%d", &accept);
        if (accept != 1 && accept != 2)
        {
            printf ("Invalid command");
            continue;
        }
        
        if (accept == 1)
        {
            char *sql;
            int id;
            sql = "SELECT QueryID from ADMIN_QUERIES WHERE id = ?;";
            rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
            sqlite3_bind_int(res, 1, code);
            sqlite3_step(res);
            id = sqlite3_column_int(res, 0);
            adminQueriesFromBase[id - 1](code);
        }
        char del[1000];
        char buf[8];
        sprintf(buf, "%d", code);
        strcpy(del, "DELETE FROM ADMIN_QUERIES WHERE id = ");
        strcat(del, buf);
        sqlite3_exec(db, del, callbackQueries, NULL, &zErrMsg);
        
        printf ("Done\n\n");
    }
}

void showOperQueries()
{
    // TODO
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


void addAccountFromBase(int query)
{
    // TODO
}

void modifyAccountFromBase(int query)
{
    // TODO
}

void deleteAccountFromBase(int query)
{
    // TODO
}

void addClientFromBase(int query)
{
    char *passport;
    char *firstName;
    char *lastName;
    char *password;
    char *sel = "SELECT PassportNo, FirstName, LastName, Password from ADMIN_QUERIES WHERE id = ?;";
    rc = sqlite3_prepare_v2(db, sel, -1, &res, 0);
    sqlite3_bind_int(res, 1, query);
    sqlite3_step(res);
    
    passport = (char*)sqlite3_column_text(res, 0);
    firstName = (char*)sqlite3_column_text(res, 1);
    lastName = (char*)sqlite3_column_text(res, 2);
    password = (char*)sqlite3_column_text(res, 3);
    
    char *insUsers = "INSERT into BANK_USERS(Login, Password, Role) Values(@log, @pass, @role);";
    rc = sqlite3_prepare_v2(db, insUsers, -1, &res, 0);
    int idx = sqlite3_bind_parameter_index(res, "@log");
    sqlite3_bind_text(res, idx, passport, -1, SQLITE_TRANSIENT);
    
    idx = sqlite3_bind_parameter_index(res, "@pass");
    sqlite3_bind_text(res, idx, password, -1, SQLITE_TRANSIENT);
    
    idx = sqlite3_bind_parameter_index(res, "@role");
    sqlite3_bind_text(res, idx, "Client", -1, SQLITE_TRANSIENT);
    
    sqlite3_step(res);
    
    char *insClients = "INSERT into BANK_CLIENTS(Passport_No, First_Name, Surname) Values(@pass, @name, @surname);";

    rc = sqlite3_prepare_v2(db, insClients, -1, &res, 0);
    idx = sqlite3_bind_parameter_index(res, "@pass");
    sqlite3_bind_text(res, idx, passport, -1, SQLITE_TRANSIENT);
    
    idx = sqlite3_bind_parameter_index(res, "@name");
    sqlite3_bind_text(res, idx, firstName, -1, SQLITE_TRANSIENT);
    
    idx = sqlite3_bind_parameter_index(res, "@surname");
    sqlite3_bind_text(res, idx, lastName, -1, SQLITE_TRANSIENT);
    
    sqlite3_step(res);

}


void modifyClientFromBase(int query)
{
    int idx;
    char *passport;
    char *firstName;
    char *lastName;
    char *newPassport;
    char *sel = "SELECT PassportNo, newFirstName, newLastName, newPassportNo from ADMIN_QUERIES WHERE id = ?;";
    rc = sqlite3_prepare_v2(db, sel, -1, &res, 0);
    sqlite3_bind_int(res, 1, query);
    sqlite3_step(res);
    
    passport = (char*)sqlite3_column_text(res, 0);
    firstName = (char*)sqlite3_column_text(res, 1);
    lastName = (char*)sqlite3_column_text(res, 2);
    newPassport = (char*)sqlite3_column_text(res, 3);
    
    char *sql = "UPDATE BANK_CLIENTS set Passport_No = @newPass, First_Name = @name, Surname = @surname WHERE Passport_No = @oldPass";
    rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
    idx = sqlite3_bind_parameter_index (res, "@newPass");
    sqlite3_bind_text(res, idx, newPassport, -1, SQLITE_TRANSIENT);
    idx = sqlite3_bind_parameter_index (res, "@name");
    sqlite3_bind_text(res, idx, firstName, -1, SQLITE_TRANSIENT);
    idx = sqlite3_bind_parameter_index (res, "@surname");
    sqlite3_bind_text(res, idx, lastName, -1, SQLITE_TRANSIENT);
    idx = sqlite3_bind_parameter_index (res, "@oldPass");
    sqlite3_bind_text(res, idx, passport, -1, SQLITE_TRANSIENT);
    sqlite3_step(res);
    
    sql = "UPDATE BANK_USERS set Login = @newPass WHERE Login = @oldPass";
    rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
    idx = sqlite3_bind_parameter_index (res, "@newPass");
    sqlite3_bind_text(res, idx, newPassport, -1, SQLITE_TRANSIENT);
    idx = sqlite3_bind_parameter_index (res, "@oldPass");
    sqlite3_bind_text(res, idx, passport, -1, SQLITE_TRANSIENT);
    
    sqlite3_step(res);
}

void deleteClientFromBase(int query)
{
    char *passport;
    char *sel = "SELECT PassportNo from ADMIN_QUERIES WHERE id = ?;";
    rc = sqlite3_prepare_v2(db, sel, -1, &res, 0);
    sqlite3_bind_int(res, 1, query);
    sqlite3_step(res);
    
    passport = (char*)sqlite3_column_text(res, 0);
    char *sql = "DELETE FROM BANK_CLIENTS WHERE Passport_No = ?;";
    rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
    sqlite3_bind_text(res, 1, passport, -1, SQLITE_TRANSIENT);
    sqlite3_step(res);
    
    sql = "DELETE FROM BANK_USERS WHERE Login = ?;";
    rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
    sqlite3_bind_text(res, 1, passport, -1, SQLITE_TRANSIENT);
    sqlite3_step(res);
}


void checkConfig()
{
    char *zErrMsg = 0;
    sqlite3_exec(db, "SELECT * FROM BANK_CONFIG;", callbackQueries, NULL, &zErrMsg);
}

void sendAdminQuery()
{
    int code;
    printf ("1 - Modify personal information\n");
    printf ("2 - Delete personal information\n");
    printf ("3 - Add new account\n");
    printf ("4 - Modify existing account\n");
    printf ("5 - Delete existing account\n");
    scanf ("%d", &code);
    if (code < 1 || code > 5)
        return;
    
    char *firstName;
    char *lastName;
    char *type;
    char *selType = "SELECT Type from ADMIN_QUERY_TYPES WHERE id = ?";
    char *selPersonal = "SELECT First_Name, Surname FROM BANK_CLIENTS WHERE Passport_No = ?";
    
    rc = sqlite3_prepare_v2(db, selType, -1, &res, 0);
    sqlite3_bind_int(res, 1, code + 1);
    sqlite3_step(res);
    type = (char*)sqlite3_column_text(res, 0);
    
    rc = sqlite3_prepare_v2(db, selPersonal, -1, &res, 0);
    sqlite3_bind_text(res, 1, currLogin, -1, SQLITE_TRANSIENT);
    sqlite3_step(res);
    firstName = (char*)sqlite3_column_text(res, 0);
    lastName = (char*)sqlite3_column_text(res, 1);
    
    int idx;
    char *sql = "INSERT into ADMIN_QUERIES (PassportNo, FirstName, LastName, QueryType, QueryID, newPassportNo, newFirstName, newLastName, oldCardNo, newCardNo, newAccountTypeID, newAccountType) Values (@passport, @name, @surname, @type, @queryID, @newPass, @newName, @newSurname, @oldCardNo, @newCardNo, @newAccountTypeID, @newAccountType);";
    rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
    
    idx = sqlite3_bind_parameter_index (res, "@passport");
    sqlite3_bind_text(res, idx, currLogin, -1, SQLITE_TRANSIENT);
    idx = sqlite3_bind_parameter_index (res, "@name");
    sqlite3_bind_text(res, idx, firstName, -1, SQLITE_TRANSIENT);
    idx = sqlite3_bind_parameter_index (res, "@surname");
    sqlite3_bind_text(res, idx, lastName, -1, SQLITE_TRANSIENT);
    idx = sqlite3_bind_parameter_index (res, "@type");
    sqlite3_bind_text(res, idx, type, -1, SQLITE_TRANSIENT);
    idx = sqlite3_bind_parameter_index(res, "@queryID");
    sqlite3_bind_int(res, idx, code + 1);
    
    switch (code)
    {
        case 1:
        {
            char buf[100];
            printf ("Enter first name: ");
            scanf("%s", buf);
            idx = sqlite3_bind_parameter_index (res, "@newName");
            sqlite3_bind_text(res, idx, buf, -1, SQLITE_TRANSIENT);
            
            printf ("Enter last name: ");
            scanf("%s", buf);
            idx = sqlite3_bind_parameter_index (res, "@newSurname");
            sqlite3_bind_text(res, idx, buf, -1, SQLITE_TRANSIENT);
            
            printf ("Enter passport number: ");
            scanf("%s", buf);
            idx = sqlite3_bind_parameter_index (res, "@newPass");
            sqlite3_bind_text(res, idx, buf, -1, SQLITE_TRANSIENT);
            
            idx = sqlite3_bind_parameter_index(res, "@oldCardNo");
            sqlite3_bind_null(res, idx);
            idx = sqlite3_bind_parameter_index(res, "@newCardNo");
            sqlite3_bind_null(res, idx);
            idx = sqlite3_bind_parameter_index(res, "@newAccountTypeID");
            sqlite3_bind_null(res, idx);
            idx = sqlite3_bind_parameter_index(res, "@newAccountType");
            sqlite3_bind_null(res, idx);
            
            if (sqlite3_step(res) != SQLITE_DONE)
                printf("Execution failed: %s\n\n", sqlite3_errmsg(db));
            else
                printf ("Your data has been sent to administrator for verification. Thank you for choosing Uranus Banking\n\n");
            break;
        }
            
        case 2:
        {
            idx = sqlite3_bind_parameter_index (res, "@newName");
            sqlite3_bind_null(res, idx);
            idx = sqlite3_bind_parameter_index (res, "@newSurname");
            sqlite3_bind_null(res, idx);
            idx = sqlite3_bind_parameter_index (res, "@newPass");
            sqlite3_bind_null(res, idx);
            idx = sqlite3_bind_parameter_index(res, "@oldCardNo");
            sqlite3_bind_null(res, idx);
            idx = sqlite3_bind_parameter_index(res, "@newCardNo");
            sqlite3_bind_null(res, idx);
            idx = sqlite3_bind_parameter_index(res, "@newAccountTypeID");
            sqlite3_bind_null(res, idx);
            idx = sqlite3_bind_parameter_index(res, "@newAccountType");
            sqlite3_bind_null(res, idx);
            
            if (sqlite3_step(res) != SQLITE_DONE)
                printf("Execution failed: %s\n\n", sqlite3_errmsg(db));
            else
                printf ("Your data has been sent to administrator for verification. Thank you for choosing Uranus Banking\n\n");
            break;

        }
        default:
            break;
    }
}

void sendOperQuery()
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