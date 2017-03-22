#include "Glob.h"

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
            addAccount ();
            break;
        }
            
        case 2:
        {
            modifyAccount ();
            break;
        }
            
        case 3:
        {
            deleteAccount();
            break;
        }
            
        case 4:
        {
            addClient ();
            break;
        }
            
        case 5:
        {
            modifyClient ();
            break;
        }
            
        case 6:
        {
            deleteClient();
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


void addAccount()
{
    int rowid;
    char buf[100];
    char *zErrMsg = 0;
    char del[200];
    printf ("Enter passport number of the client to be modified: ");
    scanf ("%s", currLogin);
    sendAdminQuery(3);
    rowid = (int)sqlite3_last_insert_rowid(db);
    sprintf (buf, "%d", rowid);
    addAccountFromBase(rowid);
    strcpy (del, "DELETE FROM ADMIN_QUERIES WHERE id = ");
    strcat(del, buf);
    sqlite3_exec(db, del, callbackQueries, NULL, &zErrMsg);
}

void modifyAccount(char *cardNo)
{
    int rowid;
    char buf[100];
    char *zErrMsg = 0;
    char del[200];
    printf ("Enter passport number of the client to be modified: ");
    scanf ("%s", currLogin);
    sendAdminQuery(4);
    rowid = (int)sqlite3_last_insert_rowid(db);
    sprintf (buf, "%d", rowid);
    modifyAccountFromBase(rowid);
    strcpy (del, "DELETE FROM ADMIN_QUERIES WHERE id = ");
    strcat(del, buf);
    sqlite3_exec(db, del, callbackQueries, NULL, &zErrMsg);
}

void deleteAccount(char *cardNo)
{
    int rowid;
    char buf[100];
    char *zErrMsg = 0;
    char del[200];
    printf ("Enter passport number of the client to be modified: ");
    scanf ("%s", currLogin);
    sendAdminQuery(5);
    rowid = (int)sqlite3_last_insert_rowid(db);
    sprintf (buf, "%d", rowid);
    deleteAccountFromBase(rowid);
    strcpy (del, "DELETE FROM ADMIN_QUERIES WHERE id = ");
    strcat(del, buf);
    sqlite3_exec(db, del, callbackQueries, NULL, &zErrMsg);
}

void addClient(char *passportNo)
{
    int rowid;
    char buf[100];
    char *zErrMsg = 0;
    char del[200];
    signup();
    rowid = (int)sqlite3_last_insert_rowid(db);
    sprintf (buf, "%d", rowid);
    addClientFromBase(rowid);
    strcpy (del, "DELETE FROM ADMIN_QUERIES WHERE id = ");
    strcat(del, buf);
    sqlite3_exec(db, del, callbackQueries, NULL, &zErrMsg);
    
}

void modifyClient(char *passportNo)
{
    int rowid;
    char buf[100];
    char *zErrMsg = 0;
    char del[200];
    printf ("Enter passport number of the client to be modified: ");
    scanf ("%s", currLogin);
    sendAdminQuery(1);
    rowid = (int)sqlite3_last_insert_rowid(db);
    sprintf (buf, "%d", rowid);
    modifyClientFromBase(rowid);
    strcpy (del, "DELETE FROM ADMIN_QUERIES WHERE id = ");
    strcat(del, buf);
    sqlite3_exec(db, del, callbackQueries, NULL, &zErrMsg);
}

void deleteClient(char *passportNo)
{
    int rowid;
    char buf[100];
    char *zErrMsg = 0;
    char del[200];
    printf ("Enter passport number of the client to be deleted: ");
    scanf ("%s", currLogin);
    sendAdminQuery(2);
    rowid = (int)sqlite3_last_insert_rowid(db);
    sprintf (buf, "%d", rowid);
    deleteClientFromBase(rowid);
    strcpy (del, "DELETE FROM ADMIN_QUERIES WHERE id = ");
    strcat(del, buf);
    sqlite3_exec(db, del, callbackQueries, NULL, &zErrMsg);
}


void addAccountFromBase(int query)
{
    char *passport;
    char *cardNo;
    char *type;
    int typeID;
    double commission;
    int ownerID;
    int idx;
    char *sel = "SELECT PassportNo, newCardNo, newAccountTypeID, newAccountType from ADMIN_QUERIES WHERE id = ?;";
    rc = sqlite3_prepare_v2(db, sel, -1, &res, 0);
    sqlite3_bind_int(res, 1, query);
    sqlite3_step(res);
    
    passport = (char*)sqlite3_column_text(res, 0);
    cardNo = (char*)sqlite3_column_text(res, 1);
    type = (char*)sqlite3_column_text(res, 3);
    typeID = sqlite3_column_int(res, 2);
    
    sel = "SELECT Commission FROM BANK_CONFIG WHERE id = ?";
    rc = sqlite3_prepare_v2(db, sel, -1, &res, 0);
    sqlite3_bind_int(res, 1, typeID);
    sqlite3_step(res);
    commission = sqlite3_column_double(res, 0);
    
    sel = "SELECT id FROM BANK_CLIENTS WHERE Passport_No = ?";
    rc = sqlite3_prepare_v2(db, sel, -1, &res, 0);
    sqlite3_bind_text(res, 1, passport, -1, SQLITE_TRANSIENT);
    sqlite3_step(res);
    ownerID = sqlite3_column_int(res, 0);
    
    char *sql = "INSERT INTO BANK_ACCOUNTS (Owner_ID, Type, Balance, TotalTransactions, Commission, Debt, Card_No) Values(@ownerID, @type, 0, 0, @comm, 0, @card);";
    rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
    idx = sqlite3_bind_parameter_index(res, "@ownerID");
    sqlite3_bind_int(res, idx, ownerID);
    
    idx = sqlite3_bind_parameter_index(res, "@type");
    sqlite3_bind_text(res, idx, type, -1, SQLITE_TRANSIENT);
    
    idx = sqlite3_bind_parameter_index(res, "@comm");
    sqlite3_bind_double(res, idx, commission);
    
    idx = sqlite3_bind_parameter_index(res, "@card");
    sqlite3_bind_text(res, idx, cardNo, -1, SQLITE_TRANSIENT);
    
    sqlite3_step(res);
    
}

void modifyAccountFromBase(int query)
{
    char *passport;
    char *cardNo;
    char *oldCardNo;
    char *type;
    int typeID;
    double commission;
    int ownerID;
    int idx;
    char *sel = "SELECT PassportNo, newCardNo, newAccountTypeID, newAccountType, oldCardNo from ADMIN_QUERIES WHERE id = ?;";
    rc = sqlite3_prepare_v2(db, sel, -1, &res, 0);
    sqlite3_bind_int(res, 1, query);
    sqlite3_step(res);
    
    passport = (char*)sqlite3_column_text(res, 0);
    cardNo = (char*)sqlite3_column_text(res, 1);
    type = (char*)sqlite3_column_text(res, 3);
    typeID = sqlite3_column_int(res, 2);
    oldCardNo = (char*)sqlite3_column_text(res, 4);
    
    sel = "SELECT id FROM BANK_CLIENTS WHERE Passport_No = ?";
    rc = sqlite3_prepare_v2(db, sel, -1, &res, 0);
    sqlite3_bind_text(res, 1, passport, -1, SQLITE_TRANSIENT);
    sqlite3_step(res);
    ownerID = sqlite3_column_int(res, 0);
    
    sel = "SELECT Commission FROM BANK_CONFIG WHERE id = ?";
    rc = sqlite3_prepare_v2(db, sel, -1, &res, 0);
    sqlite3_bind_int(res, 1, typeID);
    sqlite3_step(res);
    commission = sqlite3_column_double(res, 0);
    
    char *sql = "UPDATE BANK_ACCOUNTS set Card_No = @newCardNo, Type = @type, Commission = @comm WHERE Owner_ID = @id AND Card_No = @oldCardNo;";
    rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
    idx = sqlite3_bind_parameter_index (res, "@newCardNo");
    sqlite3_bind_text(res, idx, cardNo, -1, SQLITE_TRANSIENT);
    idx = sqlite3_bind_parameter_index (res, "@type");
    sqlite3_bind_text(res, idx, type, -1, SQLITE_TRANSIENT);
    idx = sqlite3_bind_parameter_index (res, "@comm");
    sqlite3_bind_double(res, idx, commission);
    idx = sqlite3_bind_parameter_index (res, "@id");
    sqlite3_bind_int(res, idx, ownerID);
    idx = sqlite3_bind_parameter_index (res, "@oldCardNo");
    sqlite3_bind_text(res, idx, oldCardNo, -1, SQLITE_TRANSIENT);
    sqlite3_step(res);
    
    
}

void deleteAccountFromBase(int query)
{
    char *cardNo;
    char *passport;
    int ownerID;
    int idx;
    char *sel = "SELECT PassportNo, oldCardNo from ADMIN_QUERIES WHERE id = ?;";
    rc = sqlite3_prepare_v2(db, sel, -1, &res, 0);
    sqlite3_bind_int(res, 1, query);
    sqlite3_step(res);
    passport = (char*)sqlite3_column_text(res, 0);
    cardNo = (char*)sqlite3_column_text(res, 1);
    
    sel = "SELECT id FROM BANK_CLIENTS WHERE Passport_No = ?";
    rc = sqlite3_prepare_v2(db, sel, -1, &res, 0);
    sqlite3_bind_text(res, 1, passport, -1, SQLITE_TRANSIENT);
    sqlite3_step(res);
    ownerID = sqlite3_column_int(res, 0);
    
    char *sql = "DELETE FROM BANK_ACCOUNTS WHERE Owner_ID = @ownerID AND Card_No = @cardNo";
    rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
    
    idx = sqlite3_bind_parameter_index (res, "@ownerID");
    sqlite3_bind_int(res, idx, ownerID);
    idx = sqlite3_bind_parameter_index (res, "@cardNo");
    sqlite3_bind_text(res, idx, cardNo, -1, SQLITE_TRANSIENT);
    sqlite3_step(res);
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
    int ownerID;
    char *sel = "SELECT PassportNo from ADMIN_QUERIES WHERE id = ?;";
    rc = sqlite3_prepare_v2(db, sel, -1, &res, 0);
    sqlite3_bind_int(res, 1, query);
    sqlite3_step(res);
    passport = (char*)sqlite3_column_text(res, 0);
    
    sel = "SELECT id FROM BANK_CLIENTS WHERE Passport_No = ?";
    rc = sqlite3_prepare_v2(db, sel, -1, &res, 0);
    sqlite3_bind_text(res, 1, passport, -1, SQLITE_TRANSIENT);
    sqlite3_step(res);
    ownerID = sqlite3_column_int(res, 0);
    
    char *sql = "DELETE FROM BANK_CLIENTS WHERE Passport_No = ?;";
    rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
    sqlite3_bind_text(res, 1, passport, -1, SQLITE_TRANSIENT);
    sqlite3_step(res);
    
    sql = "DELETE FROM BANK_USERS WHERE Login = ?;";
    rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
    sqlite3_bind_text(res, 1, passport, -1, SQLITE_TRANSIENT);
    sqlite3_step(res);
    
    sql = "DELETE FROM BANK_ACCOUNTS WHERE Owner_ID = ?";
    rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
    sqlite3_bind_int(res, 1, ownerID);
    while (sqlite3_step(res) == SQLITE_ROW);
}


void checkConfig()
{
    char *zErrMsg = 0;
    sqlite3_exec(db, "SELECT * FROM BANK_CONFIG;", callbackQueries, NULL, &zErrMsg);
}
