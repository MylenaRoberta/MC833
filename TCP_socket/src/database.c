#include "database.h"

int callback(void *NotUsed, int argc, char **argv, char **azColName)
{

    NotUsed = 0;

    for (int i = 0; i < argc; i++)
    {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }

    printf("\n");

    return 0;
}

int check_error(sqlite3 *db, int rc, char *err_msg)
{
    if (rc != SQLITE_OK)
    {
        if (err_msg == NULL)
        {
            fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        }
        else
        {
            fprintf(stderr, "SQL error: %s\n", err_msg);
            sqlite3_free(err_msg);
        }

        sqlite3_close(db); // Closes the database handle
        return 1;
    }

    return 0;
}

sqlite3 *open_db(char *path)
{
    sqlite3 *db; // Database handle

    // Opens a new database connection
    int rc = sqlite3_open(path, &db);

    if (check_error(db, rc, NULL))
    {
        fprintf(stderr, "Cannot open database\n");
        return NULL;
    }

    fprintf(stdout, "Connection established\n");
    return db;
}

void close_db(sqlite3 *db)
{
    sqlite3_close(db);
    fprintf(stdout, "Connection terminated\n");
}

int initialize_db(sqlite3 *db)
{
    char *err_msg = NULL;

    // Insert default profiles
    char *query = "DROP TABLE IF EXISTS Profiles;"
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

    // Runs the query
    int rc = sqlite3_exec(db, query, 0, 0, &err_msg);

    if (check_error(db, rc, err_msg))
    {
        fprintf(stderr, "Database initialization failed\n");
        return 0;
    }

    fprintf(stdout, "Database initialized\n");
    return 1;
}

void get_all_profiles(sqlite3 *db)
{
    char *err_msg = NULL;

    char *query = "SELECT * FROM Profiles";

    // Runs the query
    int rc = sqlite3_exec(db, query, callback, 0, &err_msg);

    if (check_error(db, rc, err_msg))
    {
        fprintf(stderr, "Query execution failed\n");
    }
}

void get_all_profiles_from_major(sqlite3 *db, char *major)
{
    char *err_msg = NULL;

    char query[100];
    sprintf(query, "SELECT email, first_name FROM Profiles WHERE major = %s", major);
    printf("%s\n", query);

    // Runs the query
    int rc = sqlite3_exec(db, query, callback, 0, &err_msg);

    if (check_error(db, rc, err_msg))
    {
        fprintf(stderr, "Query execution failed\n");
    }
}

void get_all_profiles_from_ability(sqlite3 *db, char *ability)
{
    char *err_msg = NULL;

    char query[100];
    sprintf(query, "SELECT email, first_name FROM Profiles WHERE abilities = %s", ability);
    printf("%s\n", query);

    // Runs the query
    int rc = sqlite3_exec(db, query, callback, 0, &err_msg);

    if (check_error(db, rc, err_msg))
    {
        fprintf(stderr, "Query execution failed\n");
    }
}

void get_all_profiles_from_graduation_year(sqlite3 *db, int year)
{
    char *err_msg = NULL;

    char query[100];
    sprintf(query, "SELECT email, first_name, major FROM Profiles WHERE graduation_year = %d", year);
    printf("%s\n", query);

    // Runs the query
    int rc = sqlite3_exec(db, query, callback, 0, &err_msg);

    if (check_error(db, rc, err_msg))
    {
        fprintf(stderr, "Query execution failed\n");
    }
}

void get_profile(sqlite3 *db, char *email)
{
    char *err_msg = NULL;

    char query[100];
    sprintf(query, "SELECT * FROM Profiles WHERE email = %s", email);
    printf("%s\n", query);

    // Runs the query
    int rc = sqlite3_exec(db, query, callback, 0, &err_msg);

    if (check_error(db, rc, err_msg))
    {
        fprintf(stderr, "Query execution failed\n");
    }
}

// Função que registra um novo perfil
int register_profile(sqlite3 *db, profile new_profile);

// Função que remove um perfil a partir de seu email
int remove_profile(sqlite3 *db, char *email);
