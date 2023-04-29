#include "../include/database.h"

void print_result(result **res) {
    result *r;

    while(*res) {
        r = *res;
        printf("%s\n", r->row);
        *res = r->next;
        free(r->row);
        free(r);
    }
}

void insert_node(result **res, char *ret) {
    result *r = (result*) malloc(sizeof(result));
    r->row = ret;
    r->next = *res;
    *res = r;
}

int close_db(sqlite3 *db) {
    // Fecha a conexão com o banco de dados
    int rc = sqlite3_close(db);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    fprintf(stdout, "Database connection terminated\n");;
    return 0;
}

sqlite3 *open_db(char *path) {
    sqlite3 *db; // Handle do banco de dados

    // Abre uma nova conexão com o banco de dados
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

    // Cria a tabela e insere os perfis iniciais
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
                  "   'ana.oliveira@hotmail.com',"
                  "   'Ana',"
                  "   'Oliveira',"
                  "   'São Paulo',"
                  "   'Engenharia de Computação',"
                  "   2016,"
                  "   'Ciência de Dados,"
                  " Processamento de Imagens,"
                  " Banco de Dados,"
                  " Visão Computacional,"
                  " Deep Learning'"
                  ");"
                  "INSERT INTO Profiles VALUES("
                  "   'felipelima@yahoo.com',"
                  "   'Felipe',"
                  "   'Lime',"
                  "   'Recife',"
                  "   'Ciência da Computação',"
                  "   2019,"
                  "   'Desenvolvimento Web,"
                  " Banco de Dados,"
                  " Programação em JavaScript,"
                  " Versionamento de Código,"
                  " Testes de Software'"
                  ");"
                  "INSERT INTO Profiles VALUES("
                  "   'juliana.fernandes@outlook.com',"
                  "   'Juliana',"
                  "   'Fernandes',"
                  "   'Porto Alegre',"
                  "   'Engenharia de Software',"
                  "   2016,"
                  "   'Arquitetura de Software,"
                  " Versionamento de Código,"
                  " Testes de Software,"
                  " Qualidade de Software,"
                  " Métodos Ágeis'"
                  ");"
                  "INSERT INTO Profiles VALUES("
                  "   'lucas.santos@uol.com.br',"
                  "   'Lucas',"
                  "   'Santos',"
                  "   'Salvador',"
                  "   'Engenharia de Computação',"
                  "   2017,"
                  "   'Desenvolvimento de Jogos,"
                  " Processamento de Imagens,"
                  " Realidade Virtual,"
                  " Versionamento de Código,"
                  " Programação em C#'"
                  ");"
                  "INSERT INTO Profiles VALUES("
                  "   'rafaelacosta@gmail.com',"
                  "   'Rafaela',"
                  "   'Costa',"
                  "   'Florianópolis',"
                  "   'Ciência da Computação',"
                  "   2021,"
                  "   'Computação Quântica,"
                  " Programação Paralela,"
                  " Segurança da Informação,"
                  " Criptografia,"
                  " Programação em Python'"
                  ");";

    // Executa a query
    int rc = sqlite3_exec(db, query, 0, 0, &err_msg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return -1;
    }

    fprintf(stdout, "Database initialized\n");
    return 0;
}

void get_all_profiles(sqlite3 *db, result **res) {
    sqlite3_stmt *stmt;
    char *query = "SELECT * FROM Profiles";

    // Compila a query
    sqlite3_prepare_v2(db, query, -1, &stmt, NULL);

    // Inicializa a lista
    *res = NULL;

    // Avalia a declaração
    while (sqlite3_step(stmt) == SQLITE_ROW) {   
        char *email = (char*) sqlite3_column_text(stmt, 0);
        char *first_name = (char*) sqlite3_column_text(stmt, 1);
        char *last_name = (char*) sqlite3_column_text(stmt, 2);
        char *location = (char*) sqlite3_column_text(stmt, 3);
        char *major = (char*) sqlite3_column_text(stmt, 4);
        char *grad_year = (char*) sqlite3_column_text(stmt, 5);
        char *abilities = (char*) sqlite3_column_text(stmt, 6);

        // Monta a string
        char *ret = malloc(1024);
        sprintf(ret, "\nEMAIL: %s\nNOME: %s\nSOBRENOME: %s\n"
                "RESIDÊNCIA: %s\nFORMAÇÃO ACADÊMICA: %s\n"
                "ANO DE FORMATURA: %s\nHABILIDADES: %s\n",
                email, first_name, last_name, location,
                major, grad_year, abilities);

        // Insere o nó na lista
        insert_node(res, ret);
    }

    // Finaliza a declaração
    sqlite3_finalize(stmt);
}

void get_profile(sqlite3 *db, result **res, char *email) {
    sqlite3_stmt *stmt;
    char *query = "SELECT * FROM Profiles WHERE email = ?";

    // Compila a query
    sqlite3_prepare_v2(db, query, -1, &stmt, NULL);

    // Vincula o valor do parâmetro à declaração
    sqlite3_bind_text(stmt, 1, email, -1, SQLITE_STATIC);

    // Inicializa a lista
    *res = NULL;

    // Avalia a declaração
    if (sqlite3_step(stmt) == SQLITE_ROW) {   
        char *email = (char*) sqlite3_column_text(stmt, 0);
        char *first_name = (char*) sqlite3_column_text(stmt, 1);
        char *last_name = (char*) sqlite3_column_text(stmt, 2);
        char *location = (char*) sqlite3_column_text(stmt, 3);
        char *major = (char*) sqlite3_column_text(stmt, 4);
        char *grad_year = (char*) sqlite3_column_text(stmt, 5);
        char *abilities = (char*) sqlite3_column_text(stmt, 6);

        // Monta a string
        char *ret = malloc(1024);
        sprintf(ret, "\nEMAIL: %s\nNOME: %s\nSOBRENOME: %s\n"
                "RESIDÊNCIA: %s\nFORMAÇÃO ACADÊMICA: %s\n"
                "ANO DE FORMATURA: %s\nHABILIDADES: %s\n",
                email, first_name, last_name, location,
                major, grad_year, abilities);

        // Insere o nó na lista
        insert_node(res, ret);
    }

    // Finaliza a declaração
    sqlite3_finalize(stmt);
}

void get_profiles_from_major(sqlite3 *db, result **res, char *major) {
    sqlite3_stmt *stmt;
    char *query = "SELECT email, first_name FROM Profiles "
                  "WHERE major = ?";

    // Compila a query
    sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    
    // Vincula o valor do parâmetro à declaração
    sqlite3_bind_text(stmt, 1, major, -1, SQLITE_STATIC);

    // Inicializa a lista
    *res = NULL;

    // Avalia a declaração
    while (sqlite3_step(stmt) == SQLITE_ROW) {   
        char *email = (char*) sqlite3_column_text(stmt, 0);
        char *first_name = (char*) sqlite3_column_text(stmt, 1);

        // Monta a string
        char *ret = malloc(512);
        sprintf(ret, "\nEMAIL: %s\nNOME: %s\n", email, first_name);

        // Insere o nó na lista
        insert_node(res, ret);
    }
    
    // Finaliza a declaração
    sqlite3_finalize(stmt);
}

void get_profiles_from_ability(sqlite3 *db, result **res, char *ability) {
    sqlite3_stmt *stmt;
    char *query = "SELECT email, first_name FROM Profiles "
                  "WHERE abilities LIKE ?";

    // Compila a query
    sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    
    // Vincula o valor do parâmetro à declaração
    char *op = "%";
    char *param = malloc(strlen(ability)+3);
    sprintf(param, "%s%s%s", op, ability, op);
    sqlite3_bind_text(stmt, 1, param, -1, SQLITE_STATIC);

    // Inicializa a lista
    *res = NULL;

    // Avalia a declaração
    while (sqlite3_step(stmt) == SQLITE_ROW) {   
        char *email = (char*) sqlite3_column_text(stmt, 0);
        char *first_name = (char*) sqlite3_column_text(stmt, 1);

        // Monta a string
        char *ret = malloc(512);
        sprintf(ret, "\nEMAIL: %s\nNOME: %s\n", email, first_name);

        // Insere o nó na lista
        insert_node(res, ret);
    }
    
    // Finaliza a declaração
    sqlite3_finalize(stmt);
    free(param);
}

void get_profiles_from_graduation_year(sqlite3 *db, result **res, int year) {
    sqlite3_stmt *stmt;
    char *query = "SELECT email, first_name, major FROM Profiles "
                  "WHERE graduation_year = ?";

    // Compila a query
    sqlite3_prepare_v2(db, query, -1, &stmt, NULL);

    // Vincula o valor do parâmetro à declaração
    sqlite3_bind_int(stmt, 1, year);

    // Inicializa a lista
    *res = NULL;

    // Avalia a declaração
    while (sqlite3_step(stmt) == SQLITE_ROW) {   
        char *email = (char*) sqlite3_column_text(stmt, 0);
        char *first_name = (char*) sqlite3_column_text(stmt, 1);
        char *major = (char*) sqlite3_column_text(stmt, 2);

        // Monta a string
        char *ret = malloc(512);
        sprintf(ret, "\nEMAIL: %s\nNOME: %s\nFORMAÇÃO ACADÊMICA: %s\n", 
                email, first_name, major);

        // Insere o nó na lista
        insert_node(res, ret);
    }

    // Finaliza a declaração
    sqlite3_finalize(stmt);
}

int register_profile(sqlite3 *db, profile new_profile) {
    sqlite3_stmt *stmt;

    int ret = 0;
    char *query = "INSERT INTO Profiles VALUES(?, ?, ?, ?, ?, ?, ?)";

    // Compila a query
    sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    
    // Vincula os valores dos parâmetros à declaração
    sqlite3_bind_text(stmt, 1, new_profile.email, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, new_profile.first_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, new_profile.last_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, new_profile.location, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, new_profile.major, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 6, new_profile.graduation_year);
    sqlite3_bind_text(stmt, 7, new_profile.abilities, -1, SQLITE_STATIC);

    // Avalia a declaração
    if (sqlite3_step(stmt) == SQLITE_DONE) {    
        fprintf(stdout, "Profile registered successfuly\n");
    } else {
        fprintf(stderr, "Registration failed\n");
        ret = -1;
    }
    
    // Finaliza a declaração
    sqlite3_finalize(stmt);
    return ret;
}

int remove_profile(sqlite3 *db, char *email) {
    sqlite3_stmt *stmt;

    int ret = 0;
    char *query = "DELETE FROM Profiles WHERE email = ?";

    // Compila a query
    sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    
    // Vincula o valor do parâmetro à declaração
    sqlite3_bind_text(stmt, 1, email, -1, SQLITE_STATIC);

    // Verifica se o perfil está cadastrado
    result *res;
    get_profile(db, &res, email);

    // Avalia a declaração
    if ((sqlite3_step(stmt) == SQLITE_DONE) && (res)) {    
        fprintf(stdout, "Profile removed successfuly\n");
    } else {
        fprintf(stderr, "Removal failed\n");
        ret = -1;
    }
    
    // Finaliza a declaração
    sqlite3_finalize(stmt);
    return ret;
}
