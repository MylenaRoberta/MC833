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

#define PORT "8330" // Porta do servidor

#define MAXDATASIZE 1024        // Maior número de bytes que pode ser recebido por vez
#define MAXBUFFERSIZE 64 * 1024 // Maior número de bytes das mensagens recebidas

// Função que imprime as opções de query
void print_menu(int admin)
{
    printf("Type the number of the query you wish to do:\n");
    printf("1 - Get all profiles\n");
    printf("2 - Get profile by email\n");
    printf("3 - Get all people from a certain major\n");
    printf("4 - Get all people that have a certain ability\n");
    printf("5 - Get all people from a certain graduation year\n");

    if (admin)
    {
        printf("6 - Register a new profile\n");
        printf("7 - Delete a profile by email\n");
    }

    printf("0 - Quit\n");
}

char *get_query()
{
    char email[50], first_name[50], last_name[50], location[50], major[50], grad_year[4], abilities[100];
    print_menu(0);
    int option;
    scanf("%d", &option); // Obtém o número da operação

    if (option == 0) // Caso o cliente queira fechar a conexão
    {
        return NULL;
    }

    char *aux_string = malloc(sizeof(char *));

    snprintf(aux_string, sizeof(int), "%d", option); // Concatena o número da operação na query
    strcat(aux_string, "&");                         // & é o símbolo que separa os parâmetros na query

    if (option == 6 || option == 7)
    {
        printf("You do not have permission to do this!\n");
        return NULL;
    }

    switch (option) // Lê os parâmetros e concatena na query, de acordo com cada operação, separados por &
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
        scanf(" %[^\n]s", major);
        strcat(aux_string, major);
        break;
    case 4:
        printf("Type the profile's ability:\n");
        scanf(" %[^\n]s", abilities);
        strcat(aux_string, abilities);
        break;
    case 5:
        printf("Type the profile's graduation year:\n");
        scanf("%s", grad_year);
        strcat(aux_string, grad_year);
        break;
    default:
        printf("Invalid option!\n");
        return NULL;
        break;
    }

    return aux_string;
}

char *get_query_admin()
{
    char email[50], first_name[50], last_name[50], location[50], major[50], grad_year[4], abilities[100];
    print_menu(1);
    int option;
    scanf("%d", &option); // Obtém o número da operação

    if (option == 0) // Caso o cliente queira fechar a conexão
    {
        return NULL;
    }

    char *aux_string = malloc(sizeof(char *));

    snprintf(aux_string, sizeof(int), "%d", option); // Concatena o número da operação na query
    strcat(aux_string, "&");                         // & é o símbolo que separa os parâmetros na query

    switch (option) // Lê os parâmetros e concatena na query, de acordo com cada operação, separados por &
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
        scanf(" %[^\n]s", major);
        strcat(aux_string, major);
        break;
    case 4:
        printf("Type the profile's ability:\n");
        scanf(" %[^\n]s", abilities);
        strcat(aux_string, abilities);
        break;
    case 5:
        printf("Type the profile's graduation year:\n");
        scanf("%s", grad_year);
        strcat(aux_string, grad_year);
        break;
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
        scanf(" %[^\n]s", major);
        strcat(aux_string, major);
        strcat(aux_string, "&");
        printf("Type the profile's graduation year:\n");
        scanf("%s", grad_year);
        strcat(aux_string, grad_year);
        strcat(aux_string, "&");
        printf("Type the profile's abilities separated by comma (like ability_a,abilityb,ability_c):\n");
        scanf(" %[^\n]s", abilities);
        strcat(aux_string, abilities);
        break;
    case 7:
        printf("Type the profile's email:\n");
        scanf("%s", email);
        strcat(aux_string, email);
        break;
    default:
        break;
    }

    return aux_string;
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

int connect_to_server(char const *server_ip, int admin)
{
    int sockfd;                           // Socket que será criado
    int numbytes;                         // Guardará o número de bytes recebidos
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

    while (1)
    {
        char received_message[MAXBUFFERSIZE]; // Buffer para guardar a mensagem recebida
        char buf[MAXDATASIZE];                // Buffer para receber mensagem
        memset(buf, 0, MAXDATASIZE);
        memset(received_message, 0, MAXBUFFERSIZE);

        char *msg; // Obtém a operação e os campos, se necessário, especificados pelo cliente

        if (admin) // Caso seja admin
        {
            msg = get_query_admin();
        }
        else // Caso não seja admin
        {
            msg = get_query();
        }

        if (msg == NULL) // Quando o cliente deseja encerrar a conexão
        {
            break;
        }

        int len = strlen(msg);

        if (send(sockfd, msg, len, 0) == -1) // Envia mensagem ao servidor indicando operação e seus parâmetros
        {
            perror("send");
            printf("Only %d bytes were successfully sent.\n", len);
        }

        int received_bytes = 0;
        long bytes_to_be_received = 0;
        int total = MAXDATASIZE - 1;

        do // Garante que todos os bytes serão recebidos
        {
            if ((numbytes = recv(sockfd, buf, total, 0)) == -1) // Recebe os bytes enviados pelo servidor
            {
                perror("recv");
                exit(1);
            }
            buf[MAXDATASIZE - 1] = '\0'; // Adiciona caracter para marcar o final da string

            strcpy(received_message + received_bytes, buf); // Copia bytes recebidos para buffer final que armazena toda a mensagem
            received_bytes += numbytes;

            if (bytes_to_be_received == 0) // Na primeira iteração, lê quantos bytes serão enviados
            {
                bytes_to_be_received = strtol(buf, NULL, 10); // Verifica quantos bytes deveriam ser recebidos
            }

        } while (bytes_to_be_received > received_bytes);

        printf("client: received '%s'\n", received_message);
        free(msg);
    }

    close(sockfd); // Fecha o socket

    return 0;
}

int main(int argc, char const *argv[])
{
    int admin = 0;

    if (argc < 2)
    {
        printf("Especifique o IP do servidor!\n");
        printf("Se o cliente for admin, digite admin depois do IP\n");
        return 1;
    }

    if (argv[2] != NULL && strcmp(argv[2], "admin") == 0)
    {
        admin = 1;
    }

    connect_to_server(argv[1], admin) == 0;
    return 0;
}
