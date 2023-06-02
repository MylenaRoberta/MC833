#ifndef CLIENT_SERVER_H
#define CLIENT_SERVER_H

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

#define PORT "8330"                // Porta utilizada pelo servidor para receber requisições
#define MAX_DATA_SIZE 1024         // Maior número de bytes que pode ser enviado/recebido por vez
#define MAX_BUFFER_SIZE 250 * 1024 // Maior número de bytes das mensagens enviadas/recebidas

typedef unsigned char uchar;

// Função responsável por retornar o endereço do socket adequado, seja IPv4 ou IPv6
void* get_in_addr(struct sockaddr *sa);

#endif
