#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

/*
    Installing: sudo apt-get install libsqlite3-dev
    Compiling:  gcc -o test db_test.c -l sqlite3
*/

int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
    NotUsed = 0;
    
    for (int i = 0; i < argc; i++) {

        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    
    printf("\n");
    
    return 0;
}

int main(void)
{
    sqlite3 *db;
    char *err_msg = 0;

    int rc = sqlite3_open("../data/test.db", &db);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);

        return 1;
    }

    char *sql = "DROP TABLE IF EXISTS Profiles;"
                "CREATE TABLE Profiles("
                "   email TEXT PRIMARY KEY,"
                "   first_name TEXT,"
                "   last_name TEXT,"
                "   location TEXT,"
                "   major TEXT,"
                "   graduation_year INT,"
                "   abilities TEXT"
                ");"
                "INSERT INTO Profiles VALUES("
                "   'maria_souza@gmail.com',"
                "   'Maria',"
                "   'Souza',"
                "   'Campinas',"
                "   'Ciência da Computação',"
                "   2018,"
                "   'Ciência de Dados'"
                ");";

                
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", err_msg);

        sqlite3_free(err_msg);
        sqlite3_close(db);

        return 1;
    }

    char *select = "SELECT * FROM Profiles";

    rc = sqlite3_exec(db, select, callback, 0, &err_msg);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);

        sqlite3_free(err_msg);
        sqlite3_close(db);

        return 1;
    }

    sqlite3_close(db);

    return 0;
}
