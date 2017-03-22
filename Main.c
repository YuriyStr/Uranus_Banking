#include "Glob.h"

sqlite3 *db;
sqlite3_stmt *res;
int rc;
int currUser = NO_USER;
int working = 1;
char currLogin[100];

void (* adminQueries[])() = {addClient, modifyClient, deleteClient, addAccount, modifyAccount, deleteAccount };
void (* adminQueriesFromBase[])(int) = {addClientFromBase, modifyClientFromBase, deleteClientFromBase, addAccountFromBase, modifyAccountFromBase, deleteAccountFromBase };
void (*operQueries[])(char*, char*, double) = { credit, debit };

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
    
    sqlite3_finalize(res);
    sqlite3_close(db);
    return 0;
}

