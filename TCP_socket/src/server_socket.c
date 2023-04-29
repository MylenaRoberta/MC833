// Código baseado no Beej's guide, especialmente no capítulo 6
#include "server.h"
#include "database.h"

// Função auxiliar que transforma o vetor de perfis em uma string
// void transform_profile_array(profile &res[], char *final_result)
// {
//     char result[MAX_BUFFER_SIZE];
//     memset(result, 0, MAX_BUFFER_SIZE);
//     char aux[MAX_BUFFER_SIZE];

//     for (int i = 0; i < PROFILES; i++) // Transforma o vetor em uma string
//     {
//         if (&res[i].email != NULL)
//         {
//             if (&res[i].email != NULL)
//             {
//                 strcat(result, "\nEMAIL: ");
//                 strcpy(aux, &res[i].email);
//                 strcat(result, aux);
//             }

//             if (&res[i].first_name != NULL)
//             {
//                 strcat(result, "\nNOME: ");
//                 strcpy(aux, &res[i].first_name);
//                 strcat(result, aux);
//             }

//             if (&res[i].last_name != NULL)
//             {
//                 strcat(result, "\nSOBRENOME: ");
//                 strcpy(aux, &res[i].last_name);
//                 strcat(result, aux);
//             }

//             if (&res[i].location != NULL)
//             {
//                 strcat(result, "\nRESIDÊNCIA: ");
//                 strcpy(aux, &res[i].location);
//                 strcat(result, aux);
//             }

//             if (&res[i].major != NULL)
//             {
//                 strcat(result, "\nFORMAÇÃO: ");
//                 strcpy(aux, &res[i].major);
//                 strcat(result, aux);
//             }

//             if (&res[i].graduation_year != 0)
//             {
//                 strcat(result, "\nFORMATURA: ");
//                 snprintf(aux, 5, "%d", &res[i].graduation_year);
//                 strcat(result, aux);
//             }

//             if (&res[i].abilities != NULL)
//             {
//                 strcat(result, "\nHABILIDADES: ");
//                 strcpy(aux, &res[i].abilities);
//                 strcat(result, aux);
//                 strcat(result, "\n");
//             }
//             strcat(result, "\n");
//         }
//     }

//     int length = strlen(result); // Obtém tamanho da string
//     snprintf(final_result, 20, "%d", length);
//     snprintf(final_result, 20, "%ld", length + strlen(final_result));
//     strcat(final_result, result); // Concatena, no início da resposta, o número de caracteres presentes na resposta
// }

int main(void) {
    sqlite3 *db = open_db(DB_PATH); // Abre a conexão com o banco de dados
    initialize_db(db);              // Inicializa o banco de dados com os perfis default

    int socket_fd;                        // Escuta conexões
    int new_fd;                           // Novas conexões
    struct addrinfo hints, *servinfo, *p; // Posteriormente vão guardar informações de endereço
    struct sockaddr_storage their_addr;   // Guarda informações do endereço dos clientes
    socklen_t sin_size;                   // Guarda informações sobre o tamanho do socket
    struct sigaction sa;                  // Para tratar processos zumbis
    int yes = 1;
    char s[INET6_ADDRSTRLEN]; // Buffer que será utilizado em caso de IPv6 da parte do cliente
    int rv;

    memset(&hints, 0, sizeof hints); // Garantir que todos os bits estão zerados
    hints.ai_family = AF_UNSPEC;     // Deixa para definir se é IPv4 ou IPv6 posteriormente
    hints.ai_socktype = SOCK_STREAM; // Define socket tipo TCP
    hints.ai_flags = AI_PASSIVE;     // Pega o IP da própria máquina

    // Pega informações sobre o endereço do socket que recebe novas conexões
    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // Faz um loop pelos resultados e faz bind para o primeiro possível
    for (p = servinfo; p != NULL; p = p->ai_next) {
        // Erro ao criar socket do servidor
        if ((socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("servidor: socket");
            continue;
        }

        // Erro ao tentar permitir o reuso de endereços locais ao usar o bind
        if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        // Erro ao tentar fazer bind
        if (bind(socket_fd, p->ai_addr, p->ai_addrlen) == -1) {
            close(socket_fd);
            perror("servidor: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // Libera, pois as informações não serão mais necessárias

    if (p == NULL) {
        fprintf(stderr, "servidor: falha ao fazer o bind\n");
        exit(1);
    }

    if (listen(socket_fd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchild_handler; // Finaliza processos zumbis
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    printf("servidor: esperando por conexões...\n");

    while (1) { // Loop em que o servidor espera por conexões e as aceita
        sin_size = sizeof their_addr;
        new_fd = accept(socket_fd, (struct sockaddr*)&their_addr, &sin_size);

        if (new_fd == -1) {
            perror("accept");
            continue;
        }

        // Obtém endereço do cliente, sendo IPv4 ou IPv6
        inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
        printf("servidor: conexão com %s\n", s);

        if (!fork()) { // Cria processo filho para lidar com as conexões aceitas
            close(socket_fd);          // Socket que aceita conexões deve ser fechado para os processos filhos
            int len, numbytes;
            char buf[MAX_DATA_SIZE];   // Buffer para receber mensagem
            char msg[MAX_BUFFER_SIZE]; // Mensagem a ser enviada

            do { // Recebe e envia mensagens enquanto o cliente não fechar o socket    
                // Recebe os bytes enviados pelo cliente
                if ((numbytes = recv(new_fd, buf, MAX_DATA_SIZE - 1, 0)) == -1) {
                    perror("recv");
                    exit(1);
                }

                buf[numbytes] = '\0'; // Adiciona caractere para marcar o final da string

                printf("servidor: recebido '%s'\n", buf);

                // Executa a operação pedida pelo cliente e põe a resposta em msg
                execute_query(buf, db, msg);
                len = strlen(msg);

                if (send_all(new_fd, msg, &len) == -1) { 
                    // Envia mensagem ao cliente conectado a este processo filho
                    perror("send");
                    printf("Somente %d bytes foram enviados com sucesso.\n", len);
                }

            } while (numbytes != 0);

            close(new_fd); // Fecha a conexão com o cliente
            exit(0);
        }

        close(new_fd); // Processo pai deve fechar o socket destinado aos processos filhos
    }

    close_db(db); // Encerra a conexão com o banco de dados

    return 0;
}
