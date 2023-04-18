#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

// Código baseado no Beej's guide, especialmente no capítulo 6

#define PORT "8330" // the port client will be connecting to

#define MAXDATASIZE 100 // max number of bytes we can get at once

// Função responsável por retornar o endereço do socket adequado, seja IPv4 ou IPv6
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)sa)->sin_addr); // IPv4
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr); // IPv6
}

int main(int argc, char *argv[])
{
    int sockfd; // Socket que será criado
    int numbytes;
    char buf[MAXDATASIZE];                // Buffer para receber mensagem
    struct addrinfo hints, *servinfo, *p; // Guardarão informações de endereço
    int rv;
    char s[INET6_ADDRSTRLEN]; // Buffer que erá utilizado apenas em caso de IPv6 da parte do servidor

    if (argc != 2) // TODO: Deve ser alterado para integrar com os projetos
    {
        fprintf(stderr, "usage: client hostname\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints); // Assegura que tudo será inicializado com zeros
    hints.ai_family = AF_UNSPEC;     // Não especifica se será IPv4 ou IPv6
    hints.ai_socktype = SOCK_STREAM; // Socket TCP

    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) // Pega informações sobre o endereço do servidor
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    for (p = servinfo; p != NULL; p = p->ai_next) // Faz um loop pelos resultados e conecta com o primeiro possível
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, // Erro ao criar socket do cliente
                             p->ai_protocol)) == -1)
        {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) // Erro ao conectar
        {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL)
    {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), // Obtém endereço do servidor, sendo IPv4 ou IPv6
              s, sizeof s);
    printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo); // Libera pois as informações não serão mais necessárias

    // TODO: alterar para receber mais bytes
    if ((numbytes = recv(sockfd, buf, MAXDATASIZE - 1, 0)) == -1) // Recebe os bytes enviados pelo servidor
    {
        perror("recv");
        exit(1);
    }

    buf[numbytes] = '\0'; // Adiciona caracter para marcar o final da string

    printf("client: received '%s'\n", buf);

    close(sockfd); // Fecha o socket

    return 0;
}
