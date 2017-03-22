#include "Glob.h"
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
            int code;
            printf ("1 - Modify personal information\n");
            printf ("2 - Delete personal information\n");
            printf ("3 - Add new account\n");
            printf ("4 - Modify existing account\n");
            printf ("5 - Delete existing account\n");
            scanf ("%d", &code);
            if (code < 1 || code > 5)
                return;
            
            sendAdminQuery(code);
            break;
        }
            
        case 2:
        {
            sendOperQuery();
            break;
        }
            
        case 3:
        {
            checkBalance();
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

void checkBalance()
{
    int ownerID;
    char cardNo[100];
    int idx;
    printf ("Enter your card number: ");
    getchar();
    fgets(cardNo, 100, stdin);
    cardNo [strlen(cardNo) - 1] = '\0';
    
    char *sel = "SELECT id FROM BANK_CLIENTS WHERE Passport_No = ?";
    rc = sqlite3_prepare_v2(db, sel, -1, &res, 0);
    sqlite3_bind_text(res, 1, currLogin, -1, SQLITE_TRANSIENT);
    sqlite3_step(res);
    ownerID = sqlite3_column_int(res, 0);

    
    char *sql = "SELECT Balance, Debt FROM BANK_ACCOUNTS WHERE Card_No = @cardNo AND Owner_ID = @id";
    rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
    idx = sqlite3_bind_parameter_index(res, "@cardNo");
    sqlite3_bind_text(res, idx, cardNo, -1, SQLITE_TRANSIENT);
    idx = sqlite3_bind_parameter_index(res, "@id");
    sqlite3_bind_int(res, idx, ownerID);
    sqlite3_step(res);
    
    printf ("Balance: %f\n", sqlite3_column_double(res, 0));
    printf ("Debt: %f\n", sqlite3_column_double(res, 1));
}