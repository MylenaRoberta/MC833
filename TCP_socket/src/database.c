#include "database.h"

void close_db(sqlite3 *db)
{
    sqlite3_close(db);
    fprintf(stdout, "Database connection terminated\n");
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

        close_db(db);
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

    fprintf(stdout, "Database connection established\n");
    return db;
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
    sqlite3_stmt *stmt;
    char *query = "SELECT * FROM Profiles";

    // Compile the query
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);

    if (check_error(db, rc, NULL))
    {
        fprintf(stderr, "Query preparation failed\n");
        return;
    }
    
    // Evaluate the statement
    int step = sqlite3_step(stmt);

    if (step == SQLITE_ROW)
    {    
        printf("GET_ALL: ");
        printf("%s : ", sqlite3_column_text(stmt, 0));
        printf("%s : ", sqlite3_column_text(stmt, 1));
        printf("%s : ", sqlite3_column_text(stmt, 2));
        printf("%s : ", sqlite3_column_text(stmt, 3));
        printf("%s : ", sqlite3_column_text(stmt, 4));
        printf("%s : ", sqlite3_column_text(stmt, 5));
        printf("%s\n", sqlite3_column_text(stmt, 6));
    } 
    
    // Finalize the statement
    sqlite3_finalize(stmt);
}

void get_profiles_from_major(sqlite3 *db, char *major)
{
    sqlite3_stmt *stmt;
    char *query = "SELECT email, first_name FROM Profiles "
                  "WHERE major = ?";

    // Compile the query
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);

    if (check_error(db, rc, NULL))
    {
        fprintf(stderr, "Query preparation failed\n");
        return;
    }
    
    // Bind the parameter value to the prepared statement
    rc = sqlite3_bind_text(stmt, 1, major, -1, SQLITE_STATIC);

    if (check_error(db, rc, NULL))
    {
        fprintf(stderr, "Error binding the parameter\n");
        return;
    }

    // Evaluate the statement
    int step = sqlite3_step(stmt);
    
    if (step == SQLITE_ROW)
    {    
        printf("FROM_MAJOR: ");
        printf("%s : ", sqlite3_column_text(stmt, 0));
        printf("%s\n", sqlite3_column_text(stmt, 1));
    } 
    
    // Finalize the statement
    sqlite3_finalize(stmt);
}

void get_profiles_from_ability(sqlite3 *db, char *ability)
{
    sqlite3_stmt *stmt;
    char *query = "SELECT email, first_name FROM Profiles "
                  "WHERE abilities = ?";

    // Compile the query
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);

    if (check_error(db, rc, NULL))
    {
        fprintf(stderr, "Query preparation failed\n");
        return;
    }
    
    // Bind the parameter value to the prepared statement
    rc = sqlite3_bind_text(stmt, 1, ability, -1, SQLITE_STATIC);

    if (check_error(db, rc, NULL))
    {
        fprintf(stderr, "Error binding the parameter\n");
        return;
    }

    // Evaluate the statement
    int step = sqlite3_step(stmt);
    
    if (step == SQLITE_ROW)
    {    
        printf("FROM_ABILITY: ");
        printf("%s : ", sqlite3_column_text(stmt, 0));
        printf("%s\n", sqlite3_column_text(stmt, 1));
    } 
    
    // Finalize the statement
    sqlite3_finalize(stmt);
}

void get_profiles_from_graduation_year(sqlite3 *db, int year)
{
    sqlite3_stmt *stmt;
    char *query = "SELECT email, first_name, major FROM Profiles "
                  "WHERE graduation_year = ?";

    // Compile the query
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);

    if (check_error(db, rc, NULL))
    {
        fprintf(stderr, "Query preparation failed\n");
        return;
    }
    
    // Bind the parameter value to the prepared statement
    rc = sqlite3_bind_int(stmt, 1, year);

    if (check_error(db, rc, NULL))
    {
        fprintf(stderr, "Error binding the parameter\n");
        return;
    }

    // Evaluate the statement
    int step = sqlite3_step(stmt);
    
    if (step == SQLITE_ROW)
    {    
        printf("FROM_YEAR: ");
        printf("%s : ", sqlite3_column_text(stmt, 0));
        printf("%s : ", sqlite3_column_text(stmt, 1));
        printf("%s\n", sqlite3_column_text(stmt, 2));
    } 
    
    // Finalize the statement
    sqlite3_finalize(stmt);
}

void get_profile(sqlite3 *db, char *email)
{
    sqlite3_stmt *stmt;

    char *query = "SELECT * FROM Profiles WHERE email = ?";

    // Compile the query
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);

    if (check_error(db, rc, NULL))
    {
        fprintf(stderr, "Query preparation failed\n");
        return;
    }
    
    // Bind the parameter value to the prepared statement
    rc = sqlite3_bind_text(stmt, 1, email, -1, SQLITE_STATIC);

    // Evaluate the statement
    int step = sqlite3_step(stmt);
    
    if (step == SQLITE_ROW)
    {    
        printf("FROM_EMAIL: ");
        printf("%s : ", sqlite3_column_text(stmt, 0));
        printf("%s : ", sqlite3_column_text(stmt, 1));
        printf("%s : ", sqlite3_column_text(stmt, 2));
        printf("%s : ", sqlite3_column_text(stmt, 3));
        printf("%s : ", sqlite3_column_text(stmt, 4));
        printf("%s : ", sqlite3_column_text(stmt, 5));
        printf("%s\n", sqlite3_column_text(stmt, 6));
    } 
    
    // Finalize the statement
    sqlite3_finalize(stmt);
}

int register_profile(sqlite3 *db, profile new_profile);

int remove_profile(sqlite3 *db, char* email);
