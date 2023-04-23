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

#define ADMIN 1

// Código baseado no Beej's guide, especialmente no capítulo 6

#define PORT "8330" // Porta do servidor

#define MAXDATASIZE 1025 // Maior número de bytes que pode ser recebido por vez

// Função que imprime as opções de query
void print_menu()
{
    printf("Type the number of the query you wish to do:\n");
    printf("1 - Get all profiles\n");
    printf("2 - Get profile by email\n");
    printf("3 - Get all people from a certain major\n");
    printf("4 - Get all people that have a certain ability\n");
    printf("5 - Get all people from a certain graduation year\n");

#if ADMIN == 1
    printf("6 - Register a new profile\n");
    printf("7 - Delete a profile by email\n");

#endif
}

// Função responsável por retornar o endereço do socket adequado, seja IPv4 ou IPv6
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)sa)->sin_addr); // IPv4
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr); // IPv6
}

int connect_to_server(char *server_ip, char *msg)
{
    int sockfd;                           // Socket que será criado
    int numbytes;                         // Guardará o número de bytes recebidos
    char buf[MAXDATASIZE];                // Buffer para receber mensagem
    struct addrinfo hints, *servinfo, *p; // Guardarão informações de endereço
    int rv;
    char s[INET6_ADDRSTRLEN]; // Buffer que erá utilizado apenas em caso de IPv6 da parte do servidor

    memset(&hints, 0, sizeof hints); // Assegura que tudo será inicializado com zeros
    hints.ai_family = AF_UNSPEC;     // Não especifica se será IPv4 ou IPv6
    hints.ai_socktype = SOCK_STREAM; // Socket TCP

    if ((rv = getaddrinfo(server_ip, PORT, &hints, &servinfo)) != 0) // Pega informações sobre o endereço do servidor
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

    int len = strlen(msg);

    if (send(sockfd, msg, len, 0) == -1)
    { // Envia mensagem ao servidor
        perror("send");
        printf("Only %d bytes were succesfully sent.\n", len);
    }

    // TODO: alterar para receber mais bytes
    if ((numbytes = recv(sockfd, buf, MAXDATASIZE - 1, 0)) == -1) // Recebe os bytes enviados pelo servidor
    {
        perror("recv");
        exit(1);
    }

    long bytes_to_be_received = strtol(buf, NULL, 10); // Verifica quantos bytes deveriam ser recebidos

    while (bytes_to_be_received > numbytes) // Garante que todos os bytes serão recebidos
    {
        if ((numbytes = recv(sockfd, buf, MAXDATASIZE - 1, 0)) == -1) // Recebe os bytes enviados pelo servidor
        {
            perror("recv");
            exit(1);
        }
    }

    buf[numbytes] = '\0'; // Adiciona caracter para marcar o final da string

    printf("client: received '%s'\n", buf);

    close(sockfd); // Fecha o socket

    return 0;
}

int main()
{
    char email[50], first_name[50], last_name[50], location[50], major[50], grad_year[4], abilities[100];
    print_menu();
    int option;
    scanf("%d", &option);

    char aux_string[MAXDATASIZE];

    snprintf(aux_string, sizeof(int), "%d", option);
    strcat(aux_string, "&");

#if ADMIN != 1
    if (option == 6 || option == 7)
    {
        printf("You do not have permission to do this!");
    }
#endif

    switch (option)
    {
    case 1:
        break;
    case 2:
        printf("Type the profile's email:\n");
        scanf("%s", email);
        strcat(aux_string, email);
        break;
    case 3:
        printf("Type the profile's major:\n");
        scanf("%s", major);
        strcat(aux_string, major);
        break;
    case 4:
        printf("Type the profile's ability:\n");
        scanf("%s", abilities);
        strcat(aux_string, abilities);
        break;
    case 5:
        printf("Type the profile's graduation year:\n");
        scanf("%s", grad_year);
        strcat(aux_string, grad_year);
        break;

#if ADMIN == 1
    case 6:
        printf("Type the profile's email:\n");
        scanf("%s", email);
        strcat(aux_string, email);
        strcat(aux_string, "&");
        printf("Type the profile's first_name:\n");
        scanf("%s", first_name);
        strcat(aux_string, first_name);
        strcat(aux_string, "&");
        printf("Type the profile's last_name:\n");
        scanf("%s", last_name);
        strcat(aux_string, last_name);
        strcat(aux_string, "&");
        printf("Type the profile's location:\n");
        scanf("%s", location);
        strcat(aux_string, location);
        strcat(aux_string, "&");
        printf("Type the profile's major:\n");
        scanf("%s", major);
        strcat(aux_string, major);
        strcat(aux_string, "&");
        printf("Type the profile's graduation year:\n");
        scanf("%s", grad_year);
        strcat(aux_string, grad_year);
        strcat(aux_string, "&");
        printf("Type the profile's abilities separated by comma (like ability_a,abilityb,ability_c):\n");
        scanf("%s", abilities);
        strcat(aux_string, abilities);
        break;
    case 7:
        printf("Type the profile's email:\n");
        scanf("%s", email);
        strcat(aux_string, email);
        break;
#endif

    default:
        break;
    }

    connect_to_server("localhost", aux_string);
    return 0;
}
