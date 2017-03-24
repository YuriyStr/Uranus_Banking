#include "Glob.h"
void showOperMenu()
{
    int code;
    //printf ("1 - Credit money\n");
    //printf ("2 - Debit money\n");
    //printf ("3 - Transfer money\n");
    //printf ("4 - Show client queries\n");
    printf ("1 - Show client queries\n");
    printf ("0 - Logout\n");
    scanf("%d", &code);
    
    switch (code)
    {
        /*case 1:
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
            showOperQueries();
            break;
        }*/
         
        case 1:
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


int credit(char *passportNo, char *cardNo, double money)
{
    char *sql;
    int ownerID;
    int idx;
    double balance;
    double debt;
    char *type;
    int trans;
    //char overdraftEnd[20];
    
    sql = "SELECT ID from BANK_CLIENTS WHERE Passport_No = ?;";
    rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
    sqlite3_bind_text(res, 1, passportNo, -1, SQLITE_TRANSIENT);
    sqlite3_step(res);
    ownerID = sqlite3_column_int(res, 0);
    
    //sql = "SELECT Balance, Debt, Type, TotalTransactions, OverdraftEnd FROM BANK_ACCOUNTS WHERE Card_No = @card AND Owner_ID = @ownerID";
    sql = "SELECT Balance, Debt, Type, TotalTransactions FROM BANK_ACCOUNTS WHERE Card_No = @card AND Owner_ID = @ownerID";
    rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
    idx = sqlite3_bind_parameter_index(res, "@card");
    sqlite3_bind_text(res, idx, cardNo, -1, SQLITE_TRANSIENT);
    idx = sqlite3_bind_parameter_index(res, "@ownerID");
    sqlite3_bind_int(res, idx, ownerID);
    
    if (sqlite3_step(res) != SQLITE_ROW)
    {
        printf ("Error: inconsistent passport and card number\n");
        return 0;
    }
    
    balance = sqlite3_column_double(res, 0);
    debt = sqlite3_column_double(res, 1);
    type = (char*)sqlite3_column_text(res, 2);
    trans = sqlite3_column_int(res, 3);
    //strcpy(overdraftEnd, (char*)sqlite3_column_text(res, 4));
    
    if (debt > 0 && money < 0)
    {
        printf ("You have unpaid debts so you can't debit money\n");
        return 0;
    }
    
    if (strcmp(type, "Overdraft") != 0)
    {
        if (money < 0 && balance + money < 0)
        {
            printf("Debit denied: you don't have enough money\n");
            return 0;
        }
        if (debt > 0)
        {
            if (money > debt)
            {
                debt = 0;
                balance += money - debt;
            }
            else
                debt -= money;
        }
        else
            balance += money;
    }
    else
    {
        sql = "SELECT OverdraftTermDays, OverdraftMax From BANK_CONFIG Where Account_Type = ?";
        rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
        sqlite3_bind_text(res, 1, "Overdraft", -1, SQLITE_TRANSIENT);
        sqlite3_step(res);
        int days = sqlite3_column_int(res, 0);
        double overdraftMax = sqlite3_column_double(res, 1);
        
        if (money < 0 && balance + money < overdraftMax)
        {
            printf("Debit denied: you don't have enough money\n");
            return 0;
        }
        if (debt > 0)
        {
            if (money > debt)
            {
                debt = 0;
                balance += money - debt;
            }
            else
                debt -= money;
        }
        else
            balance += money;
        
        /*if (balance < 0 && overdraftEnd == NULL)
        {
            struct tm *currTime;
            time_t rawtime;
            time (&rawtime);
            currTime = localtime(&rawtime);
            
            //sprintf(overdraftEnd, "%d-%d-%d", currTime->tm_year + 1900, currTime->tm_mon + 1, currTime->tm_mday + days);
        }*/
    }
    
    //sql = "UPDATE BANK_ACCOUNTS SET Balance = @bal, Debt = @debt, TotalTransactions = @trans, OverdraftEnd = @end WHERE Card_No = @card AND Owner_ID = @owner;";
    sql = "UPDATE BANK_ACCOUNTS SET Balance = @bal, Debt = @debt, TotalTransactions = @trans WHERE Card_No = @card AND Owner_ID = @owner;";
    rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
    idx = sqlite3_bind_parameter_index(res, "@card");
    sqlite3_bind_text(res, idx, cardNo, -1, SQLITE_TRANSIENT);
    idx = sqlite3_bind_parameter_index(res, "@bal");
    sqlite3_bind_double(res, idx, balance);
    idx = sqlite3_bind_parameter_index(res, "@debt");
    sqlite3_bind_double(res, idx, debt);
    idx = sqlite3_bind_parameter_index(res, "@trans");
    sqlite3_bind_int(res, idx, trans + 1);
    //idx = sqlite3_bind_parameter_index(res, "@end");
    //sqlite3_bind_text(res, idx, overdraftEnd, -1, SQLITE_TRANSIENT);
    idx = sqlite3_bind_parameter_index(res, "@owner");
    sqlite3_bind_int(res, idx, ownerID);

    sqlite3_step(res);
    return 1;
}

int debit(char *passportNo, char *cardNo, double money)
{
    return credit (passportNo, cardNo, -money);
}

int transfer(char *passportNo, char *cardNoFrom, char *cardNoTo, double money)
{
    char *sql;
    int ownerID;
    int idx;
    double commission;
    
    sql = "SELECT ID from BANK_CLIENTS WHERE Passport_No = ?;";
    rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
    sqlite3_bind_text(res, 1, passportNo, -1, SQLITE_TRANSIENT);
    sqlite3_step(res);
    ownerID = sqlite3_column_int(res, 0);
    
    sql = "SELECT Commission FROM BANK_ACCOUNTS WHERE Card_No = @card AND Owner_ID = @ownerID";
    rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
    idx = sqlite3_bind_parameter_index(res, "@card");
    sqlite3_bind_text(res, idx, cardNoTo, -1, SQLITE_TRANSIENT);
    idx = sqlite3_bind_parameter_index(res, "@ownerID");
    sqlite3_bind_int(res, idx, ownerID);
    
    if (sqlite3_step(res) != SQLITE_ROW)
    {
        printf ("Error: inconsistent passport and card number\n");
        return 0;
    }

    commission = sqlite3_column_double(res, 0);
    if (debit (passportNo, cardNoFrom, money * (1 + commission)))
    {
         if (credit(passportNo, cardNoTo, money))
             return 1;
        else
        {
            credit(passportNo, cardNoFrom, money * (1 + commission));
            return 0;
        }
    }
    return 0;
}

