#include <arpa/inet.h>
#include "database.h"
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

// Código baseado no Beej's guide, especialmente no capítulo 6

#define PORT "8330"       // Porta não utilizada por nenhum outro processo. Será responsável por receber conexões
#define BACKLOG 10        // Número máximo de conexões pendentes na fila
#define MAXDATASIZE 22025 // Maior número de bytes que pode ser enviado por vez

// Função auxiliar que transforma o vetor de perfis em uma string
void transform_profile_array(profile ps[], char *final_result)
{
    char result[MAXDATASIZE];
    memset(result, 0, MAXDATASIZE);
    char aux[MAXDATASIZE];

    for (int i = 0; i < PROFILES; i++)
    {
        if (ps[i].email != NULL)
        {
            if (ps[i].email != NULL)
            {
                strcat(result, "\nEMAIL: ");
                strcpy(aux, ps[i].email);
                strcat(result, aux);
            }

            if (ps[i].first_name != NULL)
            {
                strcat(result, "\nNOME: ");
                strcpy(aux, ps[i].first_name);
                strcat(result, aux);
            }

            if (ps[i].last_name != NULL)
            {
                strcat(result, "\nSOBRENOME: ");
                strcpy(aux, ps[i].last_name);
                strcat(result, aux);
            }

            if (ps[i].location != NULL)
            {
                strcat(result, "\nRESIDÊNCIA: ");
                strcpy(aux, ps[i].location);
                strcat(result, aux);
            }

            if (ps[i].major != NULL)
            {
                strcat(result, "\nFORMAÇÃO: ");
                strcpy(aux, ps[i].major);
                strcat(result, aux);
            }

            if (ps[i].graduation_year != 0)
            {
                strcat(result, "\nFORMATURA: ");
                snprintf(aux, 5, "%d", ps[i].graduation_year);
                strcat(result, aux);
            }

            if (ps[i].abilities != NULL)
            {
                strcat(result, "\nHABILIDADES: ");
                strcpy(aux, ps[i].abilities);
                strcat(result, aux);
                strcat(result, "\n");
            }
            strcat(result, "\n");
        }
    }

    int length = strlen(result);
    snprintf(final_result, 20, "%d", length);
    snprintf(final_result, 20, "%ld", length + strlen(final_result));
    strcat(final_result, result);
}

// Função que executa a query do cliente e retorna o seu resultado
char *execute_query(char *query, sqlite3 *db, char *query_result)
{
    char *email, *first_name, *last_name, *location, *major, *grad_year, *abilities;
    int graduation_year;
    long query_option = strtol(query, NULL, 10);

    profile p = {NULL}, ps[PROFILES], new_profile;

    // Inicializa o vetor de perfis
    for (int i = 0; i < PROFILES; i++)
    {
        ps[i] = p; // Perfil dummy
    }

    switch (query_option)
    {
    case 1:
        get_all_profiles(db, ps);
        transform_profile_array(ps, query_result);
        break;

    case 2:
        strtok(query, "&");
        email = strtok(NULL, "&");
        get_profile(db, ps, email);
        transform_profile_array(ps, query_result);
        break;

    case 3:
        strtok(query, "&");
        major = strtok(NULL, "&");
        get_profiles_from_major(db, ps, major);
        transform_profile_array(ps, query_result);
        break;

    case 4:
        strtok(query, "&");
        abilities = strtok(NULL, "&");
        get_profiles_from_ability(db, ps, abilities);
        transform_profile_array(ps, query_result);
        break;

    case 5:
        strtok(query, "&");
        grad_year = strtok(NULL, "&");
        graduation_year = atoi(grad_year);
        get_profiles_from_graduation_year(db, ps, graduation_year);
        transform_profile_array(ps, query_result);
        break;

    case 6:
        strtok(query, "&");
        new_profile.email = strtok(NULL, "&");
        new_profile.first_name = strtok(NULL, "&");
        new_profile.last_name = strtok(NULL, "&");
        new_profile.location = strtok(NULL, "&");
        new_profile.major = strtok(NULL, "&");
        grad_year = strtok(NULL, "&");
        new_profile.graduation_year = atoi(grad_year);
        new_profile.abilities = strtok(NULL, "&");

        snprintf(query_result, 20, "%d", register_profile(db, new_profile));
        break;

    case 7:
        strtok(query, "&");
        email = strtok(NULL, "&");
        snprintf(query_result, 20, "%d", remove_profile(db, email));
        break;

    default:
        break;
    }
}

// Função que garante que todos os bytes serão enviados
int send_all(int dest_socket, char *msg, int *len)
{
    int total = 0;        // Número de bytes enviados
    int remainder = *len; // Número de bytes restantes
    int n;

    while (total < *len)
    {
        n = send(dest_socket, msg + total, remainder, 0);
        if (n == -1)
        {
            break;
        }
        total += n;
        remainder -= n;
    }

    *len = total; // Número de bytes realmente enviados

    if (n == -1)
    {
        return -1; // Em caso de algum erro
    }

    return 0; // Em caso de sucesso
}

// Função responsável por finalizar processos zumbis
void sigchild_handler(int s)
{
    int errno_backup = errno;

    while (waitpid(-1, NULL, WNOHANG) > 0)
        ;

    errno = errno_backup; // O waitpid pode mudar o valor do errno, mas queremos preservá-lo
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

int main(void)
{
    char *path = "../data/profiles.db";
    sqlite3 *db = open_db(path);
    initialize_db(db);

    int socket_fd; // Escuta conexões
    int new_fd;    // Novas conexões
    int yes = 1;
    char s[INET6_ADDRSTRLEN]; // Buffer que erá utilizado apenas em caso de IPv6 da parte do cliente

    struct addrinfo hints, *servinfo, *p; // posteriormente vão guardar informações de endereço
    struct sockaddr_storage their_addr;   // Guarda informações do endereço dos clientes
    socklen_t sin_size;                   // Guarda informações sobre o tamanho do socket
    struct sigaction sa;                  // Para tratar processos zumbis

    memset(&hints, 0, sizeof hints); // Garantir que todos os bits estão zerados
    hints.ai_family = AF_UNSPEC;     // Deixa para definir se é IPv4 ou IPv6 posteriormente
    hints.ai_socktype = SOCK_STREAM; // Define socket tipo TCP
    hints.ai_flags = AI_PASSIVE;     // Pega o IP da própria máquina

    int rv;

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) // Pega informações sobre o endereço do socket que recebe novas conexões
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    for (p = servinfo; p != NULL; p = p->ai_next) // Faz um loop pelos resultados e faz bind para o primeiro possível
    {
        if ((socket_fd = socket(p->ai_family, p->ai_socktype, // Erro ao criar socket do servidor
                                p->ai_protocol)) == -1)
        {
            perror("server: socket");
            continue;
        }

        if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, // Erro ao tentar permitir o reuso de endereços locais ao usar o bind
                       sizeof(int)) == -1)
        {
            perror("setsockopt");
            exit(1);
        }

        if (bind(socket_fd, p->ai_addr, p->ai_addrlen) == -1) // Erro ao tentar fazer bind
        {
            close(socket_fd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // Libera pois as informações não serão mais necessárias

    if (p == NULL)
    {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(socket_fd, BACKLOG) == -1)
    {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchild_handler; // Finaliza processos zumbis
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(1);
    }

    printf("server: waiting for connections...\n");

    while (1) // Loop em que o servidor espera por conexões e as aceita
    {
        sin_size = sizeof their_addr;
        new_fd = accept(socket_fd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1)
        {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family, // Obtém endereço do cliente, sendo IPv4 ou IPv6
                  get_in_addr((struct sockaddr *)&their_addr),
                  s, sizeof s);
        printf("server: got connection from %s\n", s);

        if (!fork())
        { // Cria processo filho para lidar com as conexões aceitas
          // Esse trecho só é executado pelos processos filhos
            int len, numbytes;
            char buf[MAXDATASIZE]; // Buffer para receber mensagem
            close(socket_fd);      // O socket que aceita conexões deve ser fechado para os processos filhos
            char *msg = malloc(sizeof(char *));

            do // Recebe e envia mensagens enquanto o cliente não fechar o socket
            {
                if ((numbytes = recv(new_fd, buf, MAXDATASIZE - 1, 0)) == -1) // Recebe os bytes enviados pelo cliente
                {
                    perror("recv");
                    exit(1);
                }
                buf[numbytes] = '\0'; // Adiciona caracter para marcar o final da string

                printf("server: received '%s'\n", buf);

                execute_query(buf, db, msg);
                len = strlen(msg);

                if (send_all(new_fd, msg, &len) == -1)
                { // Envia mensagem ao cliente conectado a este processo filho
                    perror("send");
                    printf("Only %d bytes were successfully sent.\n", len);
                }

            } while (numbytes != 0);

            free(msg);
            close(new_fd);
            exit(0);
        }
        close(new_fd); // Processo pai deve fechar o socket destinado aos processos filhos
    }

    close_db(db); // Encerra a conexão com o banco de dados

    return 0;
}
