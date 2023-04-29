#ifndef SERVER_H
#define SERVER_H

#include "client_server.h"
#include "database.h"

#define BACKLOG 10 // Número máximo de conexões pendentes na fila

// Função responsável por finalizar processos zumbis
void sigchild_handler(int s);

// Função que garante que todos os bytes serão enviados
int send_all(int dest_socket, char *msg, int *len);

// Função auxiliar que transforma o vetor de perfis em uma string
// void transform_profile_array(profile &res[], char *final_result);

// Função que executa a operação do cliente e retorna o seu resultado
char* execute_query(sqlite3 *db, char *query, char *query_result)

#endif
