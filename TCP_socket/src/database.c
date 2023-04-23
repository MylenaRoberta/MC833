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

void get_all_profiles(sqlite3 *db, profile ps[]) {
    sqlite3_stmt *stmt;
    char *query = "SELECT * FROM Profiles";

    // Prepare the profiles array
    int index = 0;
    profile p = {NULL};
    
    for (int i = 0; i < 10; i++) {
        ps[i] = p; // Dummy profile
    }

    // Compile the query
    sqlite3_prepare_v2(db, query, -1, &stmt, NULL);

    // Evaluate the statement
    while (sqlite3_step(stmt) == SQLITE_ROW) {    
        ps[index].email = (char*)sqlite3_column_text(stmt, 0);
        ps[index].first_name = (char*)sqlite3_column_text(stmt, 1);
        ps[index].last_name = (char*)sqlite3_column_text(stmt, 2);
        ps[index].location = (char*)sqlite3_column_text(stmt, 3);
        ps[index].major = (char*)sqlite3_column_text(stmt, 4);
        ps[index].graduation_year = sqlite3_column_int(stmt, 5);
        ps[index].ability_a = (char*)sqlite3_column_text(stmt, 6);
        ps[index].ability_b = (char*)sqlite3_column_text(stmt, 7);
        ps[index].ability_c = (char*)sqlite3_column_text(stmt, 8);

        printf("EMAIL: %s\n", ps[index].email);
        printf("NOME: %s\n", ps[index].first_name);
        printf("SOBRENOME: %s\n", ps[index].last_name);
        printf("RESIDÊNCIA: %s\n", ps[index].location);
        printf("FORMAÇÃO: %s\n", ps[index].major);
        printf("FORMATURA: %d\n", ps[index].graduation_year);
        printf("HABILIDADE_A: %s\n", ps[index].ability_a);
        printf("HABILIDADE_B: %s\n", ps[index].ability_b);
        printf("HABILIDADE_C: %s\n", ps[index].ability_c);

        index++;
    }

    // Finalize the statement
    sqlite3_finalize(stmt);
}

void get_profiles_from_major(sqlite3 *db, profile ps[], char *major) {
    sqlite3_stmt *stmt;
    char *query = "SELECT email, first_name FROM Profiles "
                  "WHERE major = ?";

    // Prepare the profiles array
    int index = 0;
    profile p;
    
    p.email = NULL; // Dummy profile

    for (int i = 0; i < 10; i++) {
        ps[i] = p;
    }

    // Compile the query
    sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    
    // Bind the parameter value to the prepared statement
    sqlite3_bind_text(stmt, 1, major, -1, SQLITE_STATIC);

    // Evaluate the statement
    while (sqlite3_step(stmt) == SQLITE_ROW) {    
        p.email = (unsigned char *)sqlite3_column_text(stmt, 0);
        p.first_name = (unsigned char *)sqlite3_column_text(stmt, 1);

        // Insert the profile in the array
        ps[index] = p;
        index++;
    }
    
    // Finalize the statement
    sqlite3_finalize(stmt);
}

void get_profiles_from_ability(sqlite3 *db, profile ps[], char *ability) {
    sqlite3_stmt *stmt;
    char *query = "SELECT email, first_name FROM Profiles "
                  "WHERE (ability_a = ? OR ability_b = ? OR ability_c = ?)";
    
    // Prepare the profiles array
    int index = 0;
    profile p;
    
    p.email = NULL; // Dummy profile

    for (int i = 0; i < 10; i++) {
        ps[i] = p;
    }

    // Compile the query
    sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    
    // Bind the parameters values to the prepared statement
    for (int i = 0; i < 3; i++) {
        sqlite3_bind_text(stmt, i+1, ability, -1, SQLITE_STATIC);
    }

    // Evaluate the statement
    while (sqlite3_step(stmt) == SQLITE_ROW) {    
        p.email = (unsigned char *)sqlite3_column_text(stmt, 0);
        p.first_name = (unsigned char *)sqlite3_column_text(stmt, 1);

        // Insert the profile in the array
        ps[index] = p;
        index++;
    }
    
    // Finalize the statement
    sqlite3_finalize(stmt);
}

void get_profiles_from_graduation_year(sqlite3 *db, profile ps[], int year) {
    sqlite3_stmt *stmt;
    char *query = "SELECT email, first_name, major FROM Profiles "
                  "WHERE graduation_year = ?";

    // Prepare the profiles array
    int index = 0;
    profile p;
    
    p.email = NULL; // Dummy profile

    for (int i = 0; i < 10; i++) {
        ps[i] = p;
    }

    // Compile the query
    sqlite3_prepare_v2(db, query, -1, &stmt, NULL);

    // Bind the parameter value to the prepared statement
    sqlite3_bind_int(stmt, 1, year);

    // Evaluate the statement
    while (sqlite3_step(stmt) == SQLITE_ROW) {    
        p.email = (unsigned char *)sqlite3_column_text(stmt, 0);
        p.first_name = (unsigned char *)sqlite3_column_text(stmt, 1);
        p.major = (unsigned char *)sqlite3_column_text(stmt, 2);

        // Insert the profile in the array
        ps[index] = p;
        index++;
    }
    
    // Finalize the statement
    sqlite3_finalize(stmt);
}

profile get_profile(sqlite3 *db, char *email) {
    sqlite3_stmt *stmt;
    char *query = "SELECT * FROM Profiles WHERE email = ?";

    // Prepare the profile struct
    profile p;
    p.email = NULL;

    // Compile the query
    sqlite3_prepare_v2(db, query, -1, &stmt, NULL);

    // Bind the parameter value to the prepared statement
    sqlite3_bind_text(stmt, 1, email, -1, SQLITE_STATIC);

    // Evaluate the statement
    if (sqlite3_step(stmt) == SQLITE_ROW) {    
        p.email = (char*)sqlite3_column_text(stmt, 0);
        p.first_name = (char*)sqlite3_column_text(stmt, 1);
        p.last_name = (char*)sqlite3_column_text(stmt, 2);
        p.location = (char*)sqlite3_column_text(stmt, 3);
        p.major = (char*)sqlite3_column_text(stmt, 4);
        p.graduation_year = sqlite3_column_int(stmt, 5);
        p.ability_a = (char*)sqlite3_column_text(stmt, 6);
        p.ability_b = (char*)sqlite3_column_text(stmt, 7);
        p.ability_c = (char*)sqlite3_column_text(stmt, 8);
    }
    
    // Finalize the statement
    sqlite3_finalize(stmt);
    return p;
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
