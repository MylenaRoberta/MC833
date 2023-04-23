#include "database.h"

int close_db(sqlite3 *db) {
    // Close the database connection
    int rc = sqlite3_close(db);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    fprintf(stdout, "Database connection terminated\n");;
    return 0;
}

sqlite3 *open_db(char *path) {
    sqlite3 *db; // Database handle

    // Open a new database connection
    int rc = sqlite3_open(path, &db);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return NULL;
    }

    fprintf(stdout, "Database connection established\n");
    return db;
}

int initialize_db(sqlite3 *db) {
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
                  "   ability_a TEXT,"
                  "   ability_b TEXT,"
                  "   ability_c TEXT"
                  ");"
                  "INSERT INTO Profiles VALUES("
                  "   'maria_souza@gmail.com',"
                  "   'Maria',"
                  "   'Souza',"
                  "   'Campinas',"
                  "   'Ciência da Computação',"
                  "   2018,"
                  "   'Ciência de Dados',"
                  "   'Internet das Coisas',"
                  "   'Computação em Nuvem'"
                  ");";

    // Run the query
    int rc = sqlite3_exec(db, query, 0, 0, &err_msg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return -1;
    }

    fprintf(stdout, "Database initialized\n");
    return 0;
}

void get_all_profiles(sqlite3 *db) {
    sqlite3_stmt *stmt;
    char *query = "SELECT * FROM Profiles";

    // Compile the query
    sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    
    // Evaluate the statement
    while (sqlite3_step(stmt) == SQLITE_ROW) {    
        printf("GET_ALL: ");
        printf("%s : ", sqlite3_column_text(stmt, 0));
        printf("%s : ", sqlite3_column_text(stmt, 1));
        printf("%s : ", sqlite3_column_text(stmt, 2));
        printf("%s : ", sqlite3_column_text(stmt, 3));
        printf("%s : ", sqlite3_column_text(stmt, 4));
        printf("%s : ", sqlite3_column_text(stmt, 5));
        printf("%s : ", sqlite3_column_text(stmt, 6));
        printf("%s : ", sqlite3_column_text(stmt, 7));
        printf("%s\n", sqlite3_column_text(stmt, 8));
    }
    
    // Finalize the statement
    sqlite3_finalize(stmt);
}

void get_profiles_from_major(sqlite3 *db, char *major) {
    sqlite3_stmt *stmt;
    char *query = "SELECT email, first_name FROM Profiles "
                  "WHERE major = ?";

    // Compile the query
    sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    
    // Bind the parameter value to the prepared statement
    sqlite3_bind_text(stmt, 1, major, -1, SQLITE_STATIC);

    // Evaluate the statement
    while (sqlite3_step(stmt) == SQLITE_ROW) {    
        printf("FROM_MAJOR: ");
        printf("%s : ", sqlite3_column_text(stmt, 0));
        printf("%s\n", sqlite3_column_text(stmt, 1));
    }
    
    // Finalize the statement
    sqlite3_finalize(stmt);
}

void get_profiles_from_ability(sqlite3 *db, char *ability) {
    sqlite3_stmt *stmt;
    char *query = "SELECT email, first_name FROM Profiles "
                  "WHERE (ability_a = ? OR ability_b = ? OR ability_c = ?)";

    // Compile the query
    sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    
    // Bind the parameters values to the prepared statement
    for (int i = 0; i < 3; i++) {
        sqlite3_bind_text(stmt, i+1, ability, -1, SQLITE_STATIC);
    }

    // Evaluate the statement
    while (sqlite3_step(stmt) == SQLITE_ROW) {    
        printf("FROM_ABILITY: ");
        printf("%s : ", sqlite3_column_text(stmt, 0));
        printf("%s\n", sqlite3_column_text(stmt, 1));
    }
    
    // Finalize the statement
    sqlite3_finalize(stmt);
}

void get_profiles_from_graduation_year(sqlite3 *db, int year) {
    sqlite3_stmt *stmt;
    char *query = "SELECT email, first_name, major FROM Profiles "
                  "WHERE graduation_year = ?";

    // Compile the query
    sqlite3_prepare_v2(db, query, -1, &stmt, NULL);

    // Bind the parameter value to the prepared statement
    sqlite3_bind_int(stmt, 1, year);

    // Evaluate the statement
    while (sqlite3_step(stmt) == SQLITE_ROW) {    
        printf("FROM_YEAR: ");
        printf("%s : ", sqlite3_column_text(stmt, 0));
        printf("%s : ", sqlite3_column_text(stmt, 1));
        printf("%s\n", sqlite3_column_text(stmt, 2));
    }
    
    // Finalize the statement
    sqlite3_finalize(stmt);
}

void get_profile(sqlite3 *db, char *email) {
    sqlite3_stmt *stmt;
    char *query = "SELECT * FROM Profiles WHERE email = ?";

    // Compile the query
    sqlite3_prepare_v2(db, query, -1, &stmt, NULL);

    // Bind the parameter value to the prepared statement
    sqlite3_bind_text(stmt, 1, email, -1, SQLITE_STATIC);

    // Evaluate the statement
    if (sqlite3_step(stmt) == SQLITE_ROW) {    
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

int register_profile(sqlite3 *db, profile new_profile) {
    int ret = 0;
    sqlite3_stmt *stmt;
    char *query = "INSERT INTO Profiles VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?)";

    // Compile the query
    sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    
    // Bind the parameters values to the prepared statement
    sqlite3_bind_text(stmt, 1, new_profile.email, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, new_profile.first_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, new_profile.last_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, new_profile.location, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, new_profile.major, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 6, new_profile.graduation_year);
    sqlite3_bind_text(stmt, 7, new_profile.ability_a, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 8, new_profile.ability_b, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 9, new_profile.ability_c, -1, SQLITE_STATIC);

    // Evaluate the statement
    if (sqlite3_step(stmt) == SQLITE_DONE) {    
        fprintf(stdout, "Profile registered successfuly\n");
    } else {
        fprintf(stderr, "Registration failed\n");
        ret = -1;
    }
    
    // Finalize the statement
    sqlite3_finalize(stmt);
    return ret;
}

int remove_profile(sqlite3 *db, char* email) {
    int ret = 0;
    sqlite3_stmt *stmt;
    char *query = "DELETE FROM Profiles WHERE email = ?";

    // Compile the query
    sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    
    // Bind the parameter value to the prepared statement
    sqlite3_bind_text(stmt, 1, email, -1, SQLITE_STATIC);

    // Evaluate the statement
    if (sqlite3_step(stmt) == SQLITE_DONE) {    
        fprintf(stdout, "Profile removed successfuly\n");
    } else {
        fprintf(stderr, "Removal failed\n");
        ret = -1;
    }
    
    // Finalize the statement
    sqlite3_finalize(stmt);
    return ret;
}
