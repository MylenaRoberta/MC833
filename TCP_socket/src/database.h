#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include <stdio.h>

typedef struct
{
    char* email;
    char* first_name;
    char* last_name;
    char* location;
    char* major;
    int graduation_year;
    char* abilities;
} profile;

// Imprime o resultado de uma query
int callback(void *, int, char **, char **);

// Checa se houve algum erro a partir do return code (rc) de uma operação
int check_error(sqlite3 *db, int rc, char *err_msg);

// Abre a conexão com o banco de dados
sqlite3* open_db(char *path);

// Fecha a conexão com o banco de dados
void close_db(sqlite3 *db);

// Inicializa o banco de dados
int initialize_db(sqlite3 *db);

// Função que lista todas as informações de todos os perfis
void get_all_profiles(sqlite3 *db);

// Função que lista todos os perfis (email e nome) de um determinado curso
void get_all_profiles_from_major(sqlite3 *db, char *major);

// Função que lista todos os perfis (email e nome) que possuem uma determinada habilidade
void get_all_profiles_from_ability(sqlite3 *db, char *ability);

// Função que lista todos os perfis (email, nome e curso) de um determinado ano de formação
void get_all_profiles_from_graduation_year(sqlite3 *db, int year);

// Função que retorna todas as informações de um determinado perfil, dado seu email
void get_profile(sqlite3 *db, char *email);

// Função que registra um novo perfil
int register_profile(sqlite3 *db, profile new_profile);

// Função que remove um perfil a partir de seu email
int remove_profile(sqlite3 *db, char* email);

#endif
