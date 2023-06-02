#include "../include/database.h"

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
    sqlite3 *db; // Handler do banco de dados

    // Abre uma nova conexão com o banco de dados
    int rc = sqlite3_open(path, &db);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return NULL;
    }

    fprintf(stdout, "Database connection established\n");
    return db;
}

void insert_profile_image(sqlite3 *db, char *email, char *file_name) {
    // Monta o caminho da imagem a ser associada
    char *file_path = malloc(100);
    sprintf(file_path, "%s/%s", IMG_PATH, file_name);

    FILE *fp = fopen(file_path, "rb");   // Abre o arquivo da imagem
    fseek(fp, 0, SEEK_END);              // Move o ponteiro para o final do arquivo
    int flen = ftell(fp);                // Obtém o tamanho do arquivo em bytes
    fseek(fp, 0, SEEK_SET);              // Move o ponteiro para o começo do arquivo
    char *img = malloc(flen+1);          // Monta um array para armazenar a imagem
    int bytes = fread(img, 1, flen, fp); // Armazena a imagem no array

    fclose(fp);      // Fecha o file handler
    free(file_path); // Libera a memória alocada

    sqlite3_stmt *stmt;
    char *query = "UPDATE Profiles SET image = ? WHERE email = ?";

    sqlite3_prepare_v2(db, query, -1, &stmt, NULL); // Compila a query
    
    // Vincula os valores dos parâmetros à declaração
    sqlite3_bind_blob(stmt, 1, img, bytes, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, email, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_DONE) {    
        fprintf(stdout, "Image registered successfuly\n");
    } else {
        fprintf(stderr, "Image registration failed\n");
    }

    sqlite3_finalize(stmt); // Finaliza a declaração
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
                  "   abilities TEXT,"
                  "   image BLOB"
                  ");"
                  "INSERT INTO Profiles(email, first_name, "
                  "last_name, location, major, "
                  "graduation_year, abilities) VALUES("
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
                  "INSERT INTO Profiles(email, first_name, "
                  "last_name, location, major, "
                  "graduation_year, abilities) VALUES("
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
                  "INSERT INTO Profiles(email, first_name, "
                  "last_name, location, major, "
                  "graduation_year, abilities) VALUES("
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
                  "INSERT INTO Profiles(email, first_name, "
                  "last_name, location, major, "
                  "graduation_year, abilities) VALUES("
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
                  "INSERT INTO Profiles(email, first_name, "
                  "last_name, location, major, "
                  "graduation_year, abilities) VALUES("
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

    int rc = sqlite3_exec(db, query, 0, 0, &err_msg); // Executa a query

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return -1;
    }

    // Insere as imagens dos perfis no banco de dados
    insert_profile_image(db, "ana.oliveira@hotmail.com", "woman_1.png");
    insert_profile_image(db, "felipelima@yahoo.com", "man_1.png");
    insert_profile_image(db, "juliana.fernandes@outlook.com", "woman_2.png");
    insert_profile_image(db, "lucas.santos@uol.com.br", "man_2.png");
    insert_profile_image(db, "rafaelacosta@gmail.com", "woman_3.png");

    fprintf(stdout, "Database initialized\n");
    return 0;
}

void get_all_profiles(sqlite3 *db, result **res) {
    sqlite3_stmt *stmt;
    char *query = "SELECT email, first_name, last_name, "
                  "location, major, graduation_year, "
                  "abilities FROM Profiles";

    sqlite3_prepare_v2(db, query, -1, &stmt, NULL); // Compila a query

    *res = NULL; // Inicializa a lista

    // Avalia a declaração
    while (sqlite3_step(stmt) == SQLITE_ROW) {   
        char *email = (char*) sqlite3_column_text(stmt, 0);
        char *first_name = (char*) sqlite3_column_text(stmt, 1);
        char *last_name = (char*) sqlite3_column_text(stmt, 2);
        char *location = (char*) sqlite3_column_text(stmt, 3);
        char *major = (char*) sqlite3_column_text(stmt, 4);
        char *grad_year = (char*) sqlite3_column_text(stmt, 5);
        char *abilities = (char*) sqlite3_column_text(stmt, 6);

        // Monta a string de retorno
        char *ret = malloc(1024);
        sprintf(ret, "\nEMAIL: %s\nNOME: %s\nSOBRENOME: %s\n"
                "RESIDÊNCIA: %s\nFORMAÇÃO ACADÊMICA: %s\n"
                "ANO DE FORMATURA: %s\nHABILIDADES: %s\n",
                email, first_name, last_name, location,
                major, grad_year, abilities);

        insert_node(res, ret); // Insere o nó na lista
    }

    sqlite3_finalize(stmt); // Finaliza a declaração
}

void get_profile(sqlite3 *db, result **res, char *email) {
    sqlite3_stmt *stmt;
    char *query = "SELECT email, first_name, last_name, "
                  "location, major, graduation_year, "
                  "abilities FROM Profiles WHERE email = ?";

    sqlite3_prepare_v2(db, query, -1, &stmt, NULL); // Compila a query

    // Vincula o valor do parâmetro à declaração
    sqlite3_bind_text(stmt, 1, email, -1, SQLITE_STATIC);

    *res = NULL; // Inicializa a lista

    // Avalia a declaração
    if (sqlite3_step(stmt) == SQLITE_ROW) {   
        char *email = (char*) sqlite3_column_text(stmt, 0);
        char *first_name = (char*) sqlite3_column_text(stmt, 1);
        char *last_name = (char*) sqlite3_column_text(stmt, 2);
        char *location = (char*) sqlite3_column_text(stmt, 3);
        char *major = (char*) sqlite3_column_text(stmt, 4);
        char *grad_year = (char*) sqlite3_column_text(stmt, 5);
        char *abilities = (char*) sqlite3_column_text(stmt, 6);

        // Monta a string de retorno
        char *ret = malloc(1024);
        sprintf(ret, "\nEMAIL: %s\nNOME: %s\nSOBRENOME: %s\n"
                "RESIDÊNCIA: %s\nFORMAÇÃO ACADÊMICA: %s\n"
                "ANO DE FORMATURA: %s\nHABILIDADES: %s\n",
                email, first_name, last_name, location,
                major, grad_year, abilities);

        insert_node(res, ret); // Insere o nó na lista
    }

    sqlite3_finalize(stmt); // Finaliza a declaração
}

void get_profile_image(sqlite3 *db, result **res, char *email) {
    sqlite3_stmt *stmt;
    char *query = "SELECT image FROM Profiles WHERE email = ?";

    sqlite3_prepare_v2(db, query, -1, &stmt, NULL); // Compila a query

    // Vincula o valor do parâmetro à declaração
    sqlite3_bind_text(stmt, 1, email, -1, SQLITE_STATIC);

    *res = NULL; // Inicializa a lista
    
    // Avalia a declaração
    if (sqlite3_step(stmt) == SQLITE_ROW) {   
        // Monta o caminho da imagem recuperada
        char *file_path = malloc(500);
        sprintf(file_path, "%s/%s", IMG_PATH, RET_IMG);

        FILE *fp = fopen(file_path, "wb");                // Cria um arquivo para salvar a imagem
        int bytes = sqlite3_column_bytes(stmt, 0);        // Calcula o tamanho da imagem em bytes
        uchar *img = (uchar*) sqlite3_column_blob(stmt, 0); // Obtém o array que representa a imagem
        fwrite(img, bytes, 1, fp);                        // Armazena a imagem no arquivo criado
        
        fclose(fp);      // Fecha o file handler
        free(file_path); // Libera a memória alocada
        
        // Monta a string de retorno de retorno
        uchar *ret = malloc(2*sizeof(img));
        sprintf(ret, "%d %hhn", bytes, img);

        insert_node(res, ret); // Insere o nó na lista
    }

    sqlite3_finalize(stmt); // Finaliza a declaração
}

void get_profiles_from_major(sqlite3 *db, result **res, char *major) {
    sqlite3_stmt *stmt;
    char *query = "SELECT email, first_name FROM Profiles "
                  "WHERE major = ?";

    sqlite3_prepare_v2(db, query, -1, &stmt, NULL); // Compila a query
    
    // Vincula o valor do parâmetro à declaração
    sqlite3_bind_text(stmt, 1, major, -1, SQLITE_STATIC);

    *res = NULL; // Inicializa a lista

    // Avalia a declaração
    while (sqlite3_step(stmt) == SQLITE_ROW) {   
        char *email = (char*) sqlite3_column_text(stmt, 0);
        char *first_name = (char*) sqlite3_column_text(stmt, 1);

        // Monta a string de retorno
        char *ret = malloc(512);
        sprintf(ret, "\nEMAIL: %s\nNOME: %s\n", email, first_name);

        insert_node(res, ret); // Insere o nó na lista
    }
    
    sqlite3_finalize(stmt); // Finaliza a declaração
}

void get_profiles_from_ability(sqlite3 *db, result **res, char *ability) {
    sqlite3_stmt *stmt;
    char *query = "SELECT email, first_name FROM Profiles "
                  "WHERE abilities LIKE ?";

    sqlite3_prepare_v2(db, query, -1, &stmt, NULL); // Compila a query
    
    // Vincula o valor do parâmetro à declaração
    char *op = "%";
    char *param = malloc(strlen(ability)+3);
    sprintf(param, "%s%s%s", op, ability, op);
    sqlite3_bind_text(stmt, 1, param, -1, SQLITE_STATIC);

    *res = NULL; // Inicializa a lista

    // Avalia a declaração
    while (sqlite3_step(stmt) == SQLITE_ROW) {   
        char *email = (char*) sqlite3_column_text(stmt, 0);
        char *first_name = (char*) sqlite3_column_text(stmt, 1);

        // Monta a string de retorno
        char *ret = malloc(512);
        sprintf(ret, "\nEMAIL: %s\nNOME: %s\n", email, first_name);

        insert_node(res, ret); // Insere o nó na lista
    }
    
    sqlite3_finalize(stmt); // Finaliza a declaração
    free(param);
}

void get_profiles_from_graduation_year(sqlite3 *db, result **res, int year) {
    sqlite3_stmt *stmt;
    char *query = "SELECT email, first_name, major FROM Profiles "
                  "WHERE graduation_year = ?";

    sqlite3_prepare_v2(db, query, -1, &stmt, NULL); // Compila a query

    // Vincula o valor do parâmetro à declaração
    sqlite3_bind_int(stmt, 1, year);

    *res = NULL; // Inicializa a lista

    // Avalia a declaração
    while (sqlite3_step(stmt) == SQLITE_ROW) {   
        char *email = (char*) sqlite3_column_text(stmt, 0);
        char *first_name = (char*) sqlite3_column_text(stmt, 1);
        char *major = (char*) sqlite3_column_text(stmt, 2);

        // Monta a string de retorno
        char *ret = malloc(512);
        sprintf(ret, "\nEMAIL: %s\nNOME: %s\nFORMAÇÃO ACADÊMICA: %s\n", 
                email, first_name, major);

        insert_node(res, ret); // Insere o nó na lista
    }

    sqlite3_finalize(stmt); // Finaliza a declaração
}

int register_profile(sqlite3 *db, profile new_profile) {
    sqlite3_stmt *stmt;
    char *query = "INSERT INTO Profiles VALUES(?, ?, ?, ?, ?, ?, ?, ?)";

    // Monta o caminho da imagem a ser associada
    char *file_path = malloc(100);
    sprintf(file_path, "%s/default_img.png", IMG_PATH);

    FILE *fp = fopen(file_path, "rb");   // Abre o arquivo da imagem
    fseek(fp, 0, SEEK_END);              // Move o ponteiro para o final do arquivo
    int flen = ftell(fp);                // Obtém o tamanho do arquivo em bytes
    fseek(fp, 0, SEEK_SET);              // Move o ponteiro para o começo do arquivo
    char *img = malloc(flen+1);          // Monta um array para armazenar a imagem
    int bytes = fread(img, 1, flen, fp); // Armazena a imagem no array

    fclose(fp);      // Fecha o file handler
    free(file_path); // Libera a memória alocada

    sqlite3_prepare_v2(db, query, -1, &stmt, NULL); // Compila a query
    
    // Vincula os valores dos parâmetros à declaração
    sqlite3_bind_text(stmt, 1, new_profile.email, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, new_profile.first_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, new_profile.last_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, new_profile.location, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, new_profile.major, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 6, new_profile.graduation_year);
    sqlite3_bind_text(stmt, 7, new_profile.abilities, -1, SQLITE_STATIC);
    sqlite3_bind_blob(stmt, 8, img, bytes, SQLITE_STATIC);
    
    int ret = 0; // Avalia a declaração   
    if (sqlite3_step(stmt) == SQLITE_DONE) {    
        fprintf(stdout, "Profile registered successfuly\n");
    } else {
        fprintf(stderr, "Registration failed\n");
        ret = -1;
    }
    
    sqlite3_finalize(stmt); // Finaliza a declaração
    return ret;
}

int remove_profile(sqlite3 *db, char *email) {
    sqlite3_stmt *stmt;
    char *query = "DELETE FROM Profiles WHERE email = ?";

    sqlite3_prepare_v2(db, query, -1, &stmt, NULL); // Compila a query
    
    // Vincula o valor do parâmetro à declaração
    sqlite3_bind_text(stmt, 1, email, -1, SQLITE_STATIC);

    // Verifica se o perfil está cadastrado
    result *res;
    get_profile(db, &res, email);

    int ret = 0; // Avalia a declaração 
    if ((sqlite3_step(stmt) == SQLITE_DONE) && (res)) {    
        fprintf(stdout, "Profile removed successfuly\n");
    } else {
        fprintf(stderr, "Removal failed\n");
        ret = -1;
    }
    
    sqlite3_finalize(stmt); // Finaliza a declaração
    return ret;
}
