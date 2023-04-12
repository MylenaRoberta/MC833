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

        sqlite3_close(db); // Close the database handle
        return 1;
    }

    return 0;
}

sqlite3 *open_db(char *path)
{
    sqlite3 *db; // Database handle

    // Open a new database connection
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

    // Run the query
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

    char *sql = "SELECT * FROM Profiles";

    // Run the query
    int rc = sqlite3_exec(db, sql, callback, 0, &err_msg);

    if (check_error(db, rc, err_msg))
    {
        fprintf(stderr, "Query execution failed\n");
    }
}

void get_profiles_from_major(sqlite3 *db, char *major);

void get_profiles_from_ability(sqlite3 *db, char *ability);

void get_profiles_from_graduation_year(sqlite3 *db, int year)
{
    char *err_msg = NULL;
    sqlite3_stmt *stmt;

    char *query = "SELECT email, first_name, major FROM Profiles "
                  "WHERE graduation_year = ?";

    // Compile the query
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);

    if (check_error(db, rc, err_msg))
    {
        fprintf(stderr, "Query preparation failed\n");
        return;
    }
    
    // Bind the parameter value to the prepared statement
    sqlite3_bind_int(stmt, 1, year);

    // Evaluate the statement
    int step = sqlite3_step(stmt);
    
    if (step == SQLITE_ROW)
    {    
        printf("%s : ", sqlite3_column_text(stmt, 0));
        printf("%s\n", sqlite3_column_text(stmt, 1));
    } 
    
    // Finalize the statement
    sqlite3_finalize(stmt);
}

void get_profile(sqlite3 *db, char *email);

int register_profile(sqlite3 *db, profile new_profile);

int remove_profile(sqlite3 *db, char* email);
