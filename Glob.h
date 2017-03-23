#ifndef _GLOBH_DEFINED_
#define _GLOBH_DEFINED_
#include <stdio.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define NO_USER 0
#define ADMIN 1
#define OPER 2
#define CLIENT 3

extern sqlite3 *db;
extern sqlite3_stmt *res;
extern int rc;
extern int currUser;
extern int working;
extern char currLogin[100];

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

void sendAdminQuery(int code);
void sendOperQuery();

void credit(char *passportNo, char *cardNo, double money);
void debit (char *passportNo, char *cardNo, double money);
void transfer(char *passportNo, char *cardNoFrom, char *cardNoTo, double money);
void checkBalance();

extern void (* adminQueries[])();
extern void (* adminQueriesFromBase[])(int);
extern void (*operQueries[])(char*, char*, double);

int callbackQueries(void *data, int argc, char **argv, char **azColName);
#endif // _GLOBH_DEFINED_