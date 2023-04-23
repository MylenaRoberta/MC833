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

void desalocate_memory(profile ps[]) {
    for (int i = 0; i < PROFILES; i++) {
        if (ps[i].email != NULL) {
            free(ps[i].email);
            free(ps[i].first_name);
            free(ps[i].last_name);
            free(ps[i].location);
            free(ps[i].major);
            free(ps[i].ability_a);
            free(ps[i].ability_b);
            free(ps[i].ability_c);
            
            profile p = {NULL};
            ps[i] = p;
        } else {
            break;
        }
    }
}

void get_all_profiles(sqlite3 *db, profile ps[]) {
    sqlite3_stmt *stmt;
    char *query = "SELECT * FROM Profiles";

    // Compile the query
    sqlite3_prepare_v2(db, query, -1, &stmt, NULL);

    // Prepare the profiles array
    char *col;
    int index = 0;
    desalocate_memory(ps);

    // Evaluate the statement
    while (sqlite3_step(stmt) == SQLITE_ROW) {   
        col = (char*)sqlite3_column_text(stmt, 0);
        ps[index].email = malloc(strlen(col)+1);
        strcpy(ps[index].email, col);

        col = (char*)sqlite3_column_text(stmt, 1);
        ps[index].first_name = malloc(strlen(col)+1);
        strcpy(ps[index].first_name, col);

        col = (char*)sqlite3_column_text(stmt, 2);
        ps[index].last_name = malloc(strlen(col)+1);
        strcpy(ps[index].last_name, col);

        col = (char*)sqlite3_column_text(stmt, 3);
        ps[index].location = malloc(strlen(col)+1);
        strcpy(ps[index].location, col);

        col = (char*)sqlite3_column_text(stmt, 4);
        ps[index].major = malloc(strlen(col)+1);
        strcpy(ps[index].major, col);

        ps[index].graduation_year = sqlite3_column_int(stmt, 5);

        col = (char*)sqlite3_column_text(stmt, 6);
        ps[index].ability_a = malloc(strlen(col)+1);
        strcpy(ps[index].ability_a, col);

        col = (char*)sqlite3_column_text(stmt, 7);
        ps[index].ability_b = malloc(strlen(col)+1);
        strcpy(ps[index].ability_b, col);

        col = (char*)sqlite3_column_text(stmt, 8);
        ps[index].ability_c = malloc(strlen(col)+1);
        strcpy(ps[index].ability_c, col);

        index++;
    }

    // Finalize the statement
    sqlite3_finalize(stmt);
}

void get_profile(sqlite3 *db, profile ps[], char *email) {
    sqlite3_stmt *stmt;
    char *query = "SELECT * FROM Profiles WHERE email = ?";

    // Compile the query
    sqlite3_prepare_v2(db, query, -1, &stmt, NULL);

    // Bind the parameter value to the prepared statement
    sqlite3_bind_text(stmt, 1, email, -1, SQLITE_STATIC);

    // Prepare the profiles array
    char *col;
    int index = 0;
    desalocate_memory(ps);

    // Evaluate the statement
    if (sqlite3_step(stmt) == SQLITE_ROW) {    
        col = (char*)sqlite3_column_text(stmt, 0);
        ps[index].email = malloc(strlen(col)+1);
        strcpy(ps[index].email, col);

        col = (char*)sqlite3_column_text(stmt, 1);
        ps[index].first_name = malloc(strlen(col)+1);
        strcpy(ps[index].first_name, col);

        col = (char*)sqlite3_column_text(stmt, 2);
        ps[index].last_name = malloc(strlen(col)+1);
        strcpy(ps[index].last_name, col);

        col = (char*)sqlite3_column_text(stmt, 3);
        ps[index].location = malloc(strlen(col)+1);
        strcpy(ps[index].location, col);

        col = (char*)sqlite3_column_text(stmt, 4);
        ps[index].major = malloc(strlen(col)+1);
        strcpy(ps[index].major, col);

        ps[index].graduation_year = sqlite3_column_int(stmt, 5);

        col = (char*)sqlite3_column_text(stmt, 6);
        ps[index].ability_a = malloc(strlen(col)+1);
        strcpy(ps[index].ability_a, col);

        col = (char*)sqlite3_column_text(stmt, 7);
        ps[index].ability_b = malloc(strlen(col)+1);
        strcpy(ps[index].ability_b, col);

        col = (char*)sqlite3_column_text(stmt, 8);
        ps[index].ability_c = malloc(strlen(col)+1);
        strcpy(ps[index].ability_c, col);
    }
    
    // Finalize the statement
    sqlite3_finalize(stmt);
}

void get_profiles_from_major(sqlite3 *db, profile ps[], char *major) {
    sqlite3_stmt *stmt;
    char *query = "SELECT email, first_name FROM Profiles "
                  "WHERE major = ?";

    // Compile the query
    sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    
    // Bind the parameter value to the prepared statement
    sqlite3_bind_text(stmt, 1, major, -1, SQLITE_STATIC);

    // Prepare the profiles array
    char *col;
    int index = 0;
    desalocate_memory(ps);

    // Evaluate the statement
    while (sqlite3_step(stmt) == SQLITE_ROW) {    
        col = (char*)sqlite3_column_text(stmt, 0);
        ps[index].email = malloc(strlen(col)+1);
        strcpy(ps[index].email, col);

        col = (char*)sqlite3_column_text(stmt, 1);
        ps[index].first_name = malloc(strlen(col)+1);
        strcpy(ps[index].first_name, col);

        index++;
    }
    
    // Finalize the statement
    sqlite3_finalize(stmt);
}

void get_profiles_from_ability(sqlite3 *db, profile ps[], char *ability) {
    sqlite3_stmt *stmt;
    char *query = "SELECT email, first_name FROM Profiles "
                  "WHERE (ability_a = ? OR ability_b = ? OR ability_c = ?)";

    // Compile the query
    sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    
    // Bind the parameters values to the prepared statement
    for (int i = 0; i < 3; i++) {
        sqlite3_bind_text(stmt, i+1, ability, -1, SQLITE_STATIC);
    }

    // Prepare the profiles array
    char *col;
    int index = 0;
    desalocate_memory(ps);

    // Evaluate the statement
    while (sqlite3_step(stmt) == SQLITE_ROW) {    
        col = (char*)sqlite3_column_text(stmt, 0);
        ps[index].email = malloc(strlen(col)+1);
        strcpy(ps[index].email, col);

        col = (char*)sqlite3_column_text(stmt, 1);
        ps[index].first_name = malloc(strlen(col)+1);
        strcpy(ps[index].first_name, col);

        index++;
    }
    
    // Finalize the statement
    sqlite3_finalize(stmt);
}

void get_profiles_from_graduation_year(sqlite3 *db, profile ps[], int year) {
    sqlite3_stmt *stmt;
    char *query = "SELECT email, first_name, major FROM Profiles "
                  "WHERE graduation_year = ?";

    // Compile the query
    sqlite3_prepare_v2(db, query, -1, &stmt, NULL);

    // Bind the parameter value to the prepared statement
    sqlite3_bind_int(stmt, 1, year);

    // Prepare the profiles array
    char *col;
    int index = 0;
    desalocate_memory(ps);

    // Evaluate the statement
    while (sqlite3_step(stmt) == SQLITE_ROW) {    
        col = (char*)sqlite3_column_text(stmt, 0);
        ps[index].email = malloc(strlen(col)+1);
        strcpy(ps[index].email, col);

        col = (char*)sqlite3_column_text(stmt, 1);
        ps[index].first_name = malloc(strlen(col)+1);
        strcpy(ps[index].first_name, col);

        col = (char*)sqlite3_column_text(stmt, 2);
        ps[index].major = malloc(strlen(col)+1);
        strcpy(ps[index].major, col);

        index++;
    }
    
    // Finalize the statement
    sqlite3_finalize(stmt);
}

int register_profile(sqlite3 *db, profile new_profile) {
    sqlite3_stmt *stmt;

    int ret = 0;
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
    sqlite3_stmt *stmt;

    int ret = 0;
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
