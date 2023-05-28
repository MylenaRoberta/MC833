#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DB_PATH "data/profiles.db"
#define IMG_PATH "data/img"
#define RET_IMG "return_img.png"

typedef struct profile {
    char *email;
    char *first_name;
    char *last_name;
    char *location;
    char *major;
    int graduation_year;
    char *abilities;
} profile;

typedef struct result {
    char *row;
    struct result *next;
} result;

typedef struct image_file {
    char *img_array;
    int size;
} image_file;

// Função que insere um nó na lista de resultados
void insert_node(result **res, char *ret);

// Função que fecha a conexão com o banco de dados
int close_db(sqlite3 *db);

// Função que abre a conexão com o banco de dados
sqlite3 *open_db(char *path);

// Função que abre o arquivo da imagem e armazena os seus dados
image_file open_image(char *file_name);

// Função que insere uma imagem para um determinado perfil, dado seu email
void insert_profile_image(sqlite3 *db, char *email, char *file_name);

// Inicializa o banco de dados
int initialize_db(sqlite3 *db);

// Função que lista todas as informações de todos os perfis
void get_all_profiles(sqlite3 *db, result **res);

// Função que retorna todas as informações de um determinado perfil, dado seu email
void get_profile(sqlite3 *db, result **res, char *email);

// Função que retorna a imagem de um determinado perfil, dado seu email
void get_profile_image(sqlite3 *db, result **res, char *email);

// Função que lista todos os perfis (email e nome) de um determinado curso
void get_profiles_from_major(sqlite3 *db, result **res, char *major);

// Função que lista todos os perfis (email e nome) que possuem uma determinada habilidade
void get_profiles_from_ability(sqlite3 *db, result **res, char *ability);

// Função que lista todos os perfis (email, nome e curso) de um determinado ano de formação
void get_profiles_from_graduation_year(sqlite3 *db, result **res, int year);

// Função que registra um novo perfil
int register_profile(sqlite3 *db, profile new_profile);

// Função que remove um perfil a partir de seu email
int remove_profile(sqlite3 *db, char *email);

#endif
