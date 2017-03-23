#include "Glob.h"

int callbackQueries(void *data, int argc, char **argv, char **azColName)
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


void performPreparations()
{
    char buf[100];
    struct tm *deadline = malloc(sizeof(struct tm));
    time_t currentTime;
    struct tm *currentTimeInfo;
    
    time(&currentTime);
    currentTimeInfo = localtime(&currentTime);
    
    char *sql = "SELECT NextInterestRateDate From BANK_CONFIG Where Account_Type = ?";
    rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
    sqlite3_bind_text(res, 1, "Saving", -1, SQLITE_TRANSIENT);
    sqlite3_step(res);
    
    strcpy (buf, (char*)sqlite3_column_text(res, 0));
    sscanf (buf, "%d-%d-%d", &(deadline->tm_year), &(deadline->tm_mon), &(deadline->tm_mday));
    
    deadline->tm_year -= 1900;
    --(deadline->tm_mon);
    deadline->tm_hour = deadline->tm_min = deadline->tm_sec = 0;
    
    sql = "SELECT OverdraftDailyFee, OverdraftMax From BANK_CONFIG Where Account_Type = ?";
    rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
    sqlite3_bind_text(res, 1, "Overdraft", -1, SQLITE_TRANSIENT);
    sqlite3_step(res);
    double dailyFee = sqlite3_column_double(res, 0);
    double overdraftMax = sqlite3_column_double(res, 1);
    
    sql = "SELECT ID, Balance, OverdraftEnd, Debt FROM BANK_ACCOUNTS WHERE Type = ?";
    rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
    sqlite3_bind_text(res, 1, "Overdraft", -1, SQLITE_TRANSIENT);
    
    while (sqlite3_step(res) == SQLITE_ROW)
    {
        int idx;
        double debt = sqlite3_column_double(res, 3);
        sqlite3_stmt *pStmt;
        int id = sqlite3_column_int(res, 0);
        double balance = sqlite3_column_double(res, 1);
        char *currOverdraft = (char*)sqlite3_column_text(res, 2);
        if (currOverdraft == NULL)
            continue;
        
        struct tm *overdraftTerm = malloc(sizeof(struct tm));
        sscanf (buf, "%d-%d-%d", &(overdraftTerm->tm_year), &(overdraftTerm->tm_mon), &(overdraftTerm->tm_mday));
        overdraftTerm->tm_year -= 1900;
        --(overdraftTerm->tm_mon);
        overdraftTerm->tm_hour = overdraftTerm->tm_min = overdraftTerm->tm_sec = 0;
        if ((int)mktime(overdraftTerm) > (int)currentTime)
            continue;
        
        char *upd = "UPDATE BANK_ACCOUNTS set Balance = @bal, Debt = @debt, OverdraftEnd = @end WHERE Type = \'Overdraft\' AND ID = @id";
        rc = sqlite3_prepare_v2(db, upd, -1, &pStmt, 0);
        idx = sqlite3_bind_parameter_index(pStmt, "@end");
        if (balance >= 0)
        {
            sqlite3_bind_null(res, idx);
        }
        else
        {
            char end[20];
            balance -= dailyFee;
            if (balance < -overdraftMax)
            {
                debt += -balance - overdraftMax;
                balance = -overdraftMax;
            }
            sprintf(end, "%d-%d-%d", overdraftTerm->tm_year + 1900, overdraftTerm->tm_mon + 1, overdraftTerm->tm_mday + 1);
            sqlite3_bind_text(pStmt, idx, end, -1, SQLITE_TRANSIENT);
        }
        idx = sqlite3_bind_parameter_index(pStmt, "@bal");
        sqlite3_bind_double(pStmt, idx, balance);
        idx = sqlite3_bind_parameter_index(pStmt, "@debt");
        sqlite3_bind_double(pStmt, idx, debt);
        idx = sqlite3_bind_parameter_index(pStmt, "@id");
        sqlite3_bind_int(pStmt, idx, id);
        sqlite3_step(pStmt);
        sqlite3_finalize(pStmt);
        free(overdraftTerm);
    }

    
    if ((int)currentTime >= (int)mktime(deadline))
    {
        double rate;
        sql = "SELECT InterestRate FROM BANK_CONFIG WHERE Type = ?";
        rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
        sqlite3_bind_text(res, 1, "Saving", -1, SQLITE_TRANSIENT);
        sqlite3_step(res);
        rate = sqlite3_column_double(res, 0);
        
        sql = "SELECT ID, Balance FROM BANK_ACCOUNTS WHERE Type = ?";
        rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
        sqlite3_bind_text(res, 1, "Saving", -1, SQLITE_TRANSIENT);
        while (sqlite3_step(res) == SQLITE_ROW)
        {
            int idx;
            sqlite3_stmt *pStmt;
            int id = sqlite3_column_int(res, 0);
            double balance = sqlite3_column_double(res, 1) * (rate + 1);
            char *upd = "UPDATE BANK_ACCOUNTS set Balance = @bal WHERE Type = \'Saving\' AND ID = @id";
            rc = sqlite3_prepare_v2(db, upd, -1, &pStmt, 0);
            idx = sqlite3_bind_parameter_index(res, "@bal");
            sqlite3_bind_double(pStmt, idx, balance);
            idx = sqlite3_bind_parameter_index(res, "@id");
            sqlite3_bind_int(pStmt, idx, id);
            sqlite3_step(pStmt);
            sqlite3_finalize(pStmt);
        }
        
        
        int quota;
        double fee;
        double overdraftMax;
        int term;
        sql = "SELECT MonthlyQouta, PerTransactionFee, OverdraftMax, OverdraftTermDay FROM BANK_CONFIG WHERE Type = ?";
        rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
        sqlite3_bind_text(res, 1, "Overdraft", -1, SQLITE_TRANSIENT);
        sqlite3_step(res);
        quota = sqlite3_column_int(res, 0);
        fee = sqlite3_column_double(res, 1);
        overdraftMax = sqlite3_column_double(res, 2);
        term = sqlite3_column_int(res, 3);
        
        sql = "SELECT ID, Balance, TotalTransactions, Type, OverdraftEnd, Debt FROM BANK_ACCOUNTS WHERE Type = \'Checking\' OR Type = ?";
        rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
        sqlite3_bind_text(res, 1, "Overdraft", -1, SQLITE_TRANSIENT);
        while (sqlite3_step(res) == SQLITE_ROW)
        {
            int idx;
            double debt = sqlite3_column_double(res, 5);
            sqlite3_stmt *pStmt;
            int id = sqlite3_column_int(res, 0);
            double balance = sqlite3_column_double(res, 1);
            int trans = sqlite3_column_int(res, 2);
            char *type = (char*)sqlite3_column_text(res, 3);
            char *currOverdraft = (char*)sqlite3_column_text(res, 4);
            char *upd = "UPDATE BANK_ACCOUNTS set Balance = @bal, Debt = @debt, TotalTransactions = 0, OverdraftEnd = @end WHERE (Type = \'Checking\' OR Type = \'Overdraft\') AND ID = @id";
            rc = sqlite3_prepare_v2(db, upd, -1, &pStmt, 0);
            idx = sqlite3_bind_parameter_index(pStmt, "@end");
            if (trans > quota)
            {
                balance -= fee * (trans - quota);
                if (strcmp(type, "Checking") == 0)
                {
                    sqlite3_bind_null(pStmt, idx);
                    if (balance < 0)
                    {
                        debt += -balance;
                        balance = 0;
                    }
                }
                else
                {
                    if (balance < 0)
                    {
                        if (currOverdraft != NULL)
                        {
                            char end[20];
                            sprintf(end, "%d-%d-%d", currentTimeInfo->tm_year + 1900, currentTimeInfo->tm_mon + 1, currentTimeInfo->tm_mday + term);
                            sqlite3_bind_text(pStmt, idx, end, -1, SQLITE_TRANSIENT);
                        }
                        if (balance < -overdraftMax)
                        {
                            debt += -balance - overdraftMax;
                            balance = -overdraftMax;
                        }
                    }
                    else
                        sqlite3_bind_null(pStmt, idx);
                }
            }
            idx = sqlite3_bind_parameter_index(pStmt, "@bal");
            sqlite3_bind_double(pStmt, idx, balance);
            idx = sqlite3_bind_parameter_index(pStmt, "@debt");
            sqlite3_bind_double(pStmt, idx, debt);
            idx = sqlite3_bind_parameter_index(pStmt, "@id");
            sqlite3_bind_int(pStmt, idx, id);
            sqlite3_step(pStmt);
            sqlite3_finalize(pStmt);
        }

    }
    
    int i;
    sql = "UPDATE BANK_CONFIG set NextInterestRateDate = ? Where Account_Type = \'Saving\'";
    rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
    char buffer[20];
    sprintf (buffer, "%d-%d-%d", deadline->tm_year + 1900, deadline->tm_mon + 2, deadline->tm_mday);
    sqlite3_bind_text(res, 1, buffer, -1, SQLITE_TRANSIENT);
    sqlite3_step(res);
    
    free(deadline);
    
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
        {
            currUser = CLIENT;
            strcpy (currLogin, log);
        }
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
    //sqlite3_step(res);
    
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

void sendAdminQuery(int code)
{
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
            
        case 2: case 5:
        {
            if (code == 5)
            {
                char buf[100];
                printf("Enter your current card number: ");
                getchar();
                fgets(buf, 100, stdin);
                buf[strlen(buf) - 1] = '\0';
                idx = sqlite3_bind_parameter_index(res, "@oldCardNo");
                sqlite3_bind_text(res, idx, buf, -1, SQLITE_TRANSIENT);
            }
            else
            {
                idx = sqlite3_bind_parameter_index(res, "@oldCardNo");
                sqlite3_bind_null(res, idx);
            }
            idx = sqlite3_bind_parameter_index (res, "@newName");
            sqlite3_bind_null(res, idx);
            idx = sqlite3_bind_parameter_index (res, "@newSurname");
            sqlite3_bind_null(res, idx);
            idx = sqlite3_bind_parameter_index (res, "@newPass");
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
            
        case 3: case 4:
        {
            char buf[100];
            int accType;
            getchar();
            
            if (code == 4)
            {
                printf("Enter your current card number: ");
                fgets(buf, 100, stdin);
                buf[strlen(buf) - 1] = '\0';
                idx = sqlite3_bind_parameter_index(res, "@oldCardNo");
                sqlite3_bind_text(res, idx, buf, -1, SQLITE_TRANSIENT);
            }
            else
            {
                idx = sqlite3_bind_parameter_index(res, "@oldCardNo");
                sqlite3_bind_null(res, idx);
            }
            
            printf("Enter your new card number: ");
            fgets(buf, 100, stdin);
            buf[strlen(buf) - 1] = '\0';
            idx = sqlite3_bind_parameter_index(res, "@newCardNo");
            sqlite3_bind_text(res, idx, buf, -1, SQLITE_TRANSIENT);
            
            do
            {
                printf("Choose your account type:\n");
                printf ("1 - Checking\n");
                printf("2 - Saving\n");
                printf("3 - Overdraft\n");
                scanf("%d", &accType);
            }while (accType < 1 || accType > 3);
            idx = sqlite3_bind_parameter_index(res, "@newAccountTypeID");
            sqlite3_bind_int(res, idx, accType);
            
            char *conf = "SELECT Account_Type FROM BANK_CONFIG WHERE id = ?";
            sqlite3_stmt *pStmt;
            sqlite3_prepare_v2(db, conf, -1, &pStmt, 0);
            sqlite3_bind_int(pStmt, 1, accType);
            sqlite3_step(pStmt);
            strcpy (buf, (char*)sqlite3_column_text(pStmt, 0));
            idx = sqlite3_bind_parameter_index(res, "@newAccountType");
            sqlite3_bind_text(res, idx, buf, -1, SQLITE_TRANSIENT);
            
            idx = sqlite3_bind_parameter_index (res, "@newName");
            sqlite3_bind_null(res, idx);
            idx = sqlite3_bind_parameter_index (res, "@newSurname");
            sqlite3_bind_null(res, idx);
            idx = sqlite3_bind_parameter_index (res, "@newPass");
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

