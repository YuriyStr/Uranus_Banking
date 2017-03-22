#include "Glob.h"
void showOperMenu()
{
    int code;
    printf ("1 - Credit money\n");
    printf ("2 - Debit money\n");
    printf ("3 - Transfer money\n");
    printf ("4 - Show client queries\n");
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

