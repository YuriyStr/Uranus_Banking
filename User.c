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

void checkBalance(char *passportNo, char *cardNo)
{
    // TODO
}