#ifndef CLIENT_H
#define CLIENT_H

#include "client_server.h"

// Função que imprime as opções de query
void print_menu(int admin);

// Função obtém a operação desejada pelo cliente e os seus parâmetros, se necessário
char* get_client_operation(int admin);

#endif
