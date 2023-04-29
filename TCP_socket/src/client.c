// Código baseado no Beej's guide, especialmente no capítulo 6
#include "../include/client_server.h"

// Função que imprime as opções de operações
void print_menu(int admin) {
    printf("----------------------------------------------------------------------\n");
    printf("Digite o número da operação que você deseja:\n");

    printf("[1] Listar todas as informações de todos os perfis\n");
    printf("[2] Listar todas as informações de um perfil\n");
    printf("[3] Listar todas as pessoas formadas em um determinado curso\n");
    printf("[4] Listar todas as pessoas que possuem uma determinada habilidade\n");
    printf("[5] Listar todas as pessoas formadas em um determinado ano\n");

    if (admin) {  // Cliente administrador
        printf("[6] Cadastrar um novo perfil\n");
        printf("[7] Remover um perfil a partir de seu email\n");
    }

    printf("[0] Sair\n");
    printf("----------------------------------------------------------------------\n");
}

// Função obtém a operação desejada pelo cliente e os seus parâmetros, se necessário
char* get_client_operation(int admin) {
    char email[50], first_name[50], last_name[50], location[50], major[50], grad_year[4], abilities[100];

    int option;
    print_menu(admin);    // Exibe o menu de operações
    printf("+ Operação: ");
    scanf("%d", &option); // Obtém o número da operação

    if (option == 0) {    // Fechamento de conexão
        return NULL;
    }

    char *param = malloc(sizeof(char*));        // Parâmetro(s) para a operação
    snprintf(param, sizeof(int), "%d", option); // Concatena o número da operação na query
    strcat(param, "&");                         // & é o símbolo que separa os parâmetros na query

    // Lê os parâmetros e concatena na query, de acordo com cada operação, separados por &
    switch (option) {
        case 1:
            break;
        
        case 2:
            printf("> Digite o email desejado: ");
            scanf("%s", email);
            strcat(param, email);
            break;
        
        case 3:
            printf("> Digite o curso desejado: ");
            scanf(" %[^\n]s", major);
            strcat(param, major);
            break;
        
        case 4:
            printf("> Digite a habilidade desejada: ");
            scanf(" %[^\n]s", abilities);
            strcat(param, abilities);
            break;
        
        case 5:
            printf("> Digite o ano de formatura desejado: ");
            scanf("%s", grad_year);
            strcat(param, grad_year);
            break;
        
        case 6:
            if (admin) {
                printf("> Digite o email: ");
                scanf("%s", email);
                strcat(param, email);
                strcat(param, "&");
            
                printf("> Digite o nome: ");
                scanf("%s", first_name);
                strcat(param, first_name);
                strcat(param, "&");

                printf("> Digite o sobrenome: ");
                scanf("%s", last_name);
                strcat(param, last_name);
                strcat(param, "&");
            
                printf("> Digite a cidade de residência: ");
                scanf("%s", location);
                strcat(param, location);
                strcat(param, "&");

                printf("> Digite o curso: ");
                scanf(" %[^\n]s", major);
                strcat(param, major);
                strcat(param, "&");

                printf("> Digite o ano de formatura: ");
                scanf("%s", grad_year);
                strcat(param, grad_year);
                strcat(param, "&");

                printf("> Digite as habilidades separadas por vírgula: ");
                scanf(" %[^\n]s", abilities);
                strcat(param, abilities);
            } else {
                printf("Operação inválida!\n");
                param = NULL;
            }
            break;
        
        case 7:
            if (admin) {
                printf("> Digite o email desejado: ");
                scanf("%s", email);
                strcat(param, email);
            } else {
                printf("Operação inválida!\n");
                param = NULL;
            }
            break;
        
        default:
            printf("Operação inválida!\n");
            param = NULL;
            break;
    }

    return param;
}

int main(int argc, char const *argv[]) {
    int admin = 0;

    if (argc < 2) {
        printf("Especifique o IP do servidor!\n");
        printf("Se o cliente for admin, digite admin depois do IP\n");
        return 1;
    }

    if ((argv[2] != NULL) && (strcmp(argv[2], "admin")) == 0) {
        admin = 1;
    }

    int sockfd;                           // Socket que será criado
    int numbytes;                         // Guardará o número de bytes recebidos
    struct addrinfo hints, *servinfo, *p; // Guardarão informações de endereço
    int rv;
    char s[INET6_ADDRSTRLEN]; // Buffer que erá utilizado apenas em caso de IPv6 da parte do servidor

    memset(&hints, 0, sizeof hints); // Assegura que tudo será inicializado com zeros
    hints.ai_family = AF_UNSPEC;     // Não especifica se será IPv4 ou IPv6
    hints.ai_socktype = SOCK_STREAM; // Socket TCP

    // Pega informações sobre o endereço do servidor
    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // Faz um loop pelos resultados e conecta com o primeiro possível
    for (p = servinfo; p != NULL; p = p->ai_next) {
        // Erro ao criar socket do cliente
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("cliente: socket");
            continue;
        }

        // Erro ao conectar
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("cliente: conexão");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "cliente: falha ao conectar\n");
        return 2;
    }

    // Obtém endereço do servidor, sendo IPv4 ou IPv6
    inet_ntop(p->ai_family, get_in_addr((struct sockaddr*)p->ai_addr), s, sizeof s);
    printf("cliente: conectando a %s\n", s);

    freeaddrinfo(servinfo); // Libera, pois as informações não serão mais necessárias

    while (1) {
        // Obtém a operação e os parâmetros, se necessário, especificados pelo cliente
        char *msg = get_client_operation(admin);

        if (msg == NULL) { // Cliente deseja encerrar a conexão
            break;
        }

        int len = strlen(msg);
        char received_message[MAX_BUFFER_SIZE]; // Buffer para guardar a mensagem recebida
        char buf[MAX_DATA_SIZE];                // Buffer para receber mensagem
        memset(buf, 0, MAX_DATA_SIZE);
        memset(received_message, 0, MAX_BUFFER_SIZE);

        // Envia mensagem ao servidor indicando operação e seus parâmetros
        if (send(sockfd, msg, len, 0) == -1) {
            perror("send");
            printf("Somente %d bytes foram enviados com sucesso\n", len);
        }

        int received_bytes = 0;
        long bytes_to_be_received = 0;
        int total = MAX_DATA_SIZE - 1;

        do { // Garante que todos os bytes serão recebidos
            // Recebe os bytes enviados pelo servidor
            if ((numbytes = recv(sockfd, buf, total, 0)) == -1) {
                perror("recv");
                exit(1);
            }

            // Adiciona caractere para marcar o final da string
            buf[MAX_DATA_SIZE-1] = '\0';

            // Copia bytes recebidos para buffer final que armazena toda a mensagem
            strcpy(received_message + received_bytes, buf);
            received_bytes += numbytes;

            // Na primeira iteração, lê quantos bytes serão enviados
            if (bytes_to_be_received == 0) {
                // Verifica quantos bytes deveriam ser recebidos
                bytes_to_be_received = strtol(buf, NULL, 10);
            }

        } while (bytes_to_be_received > received_bytes);

        printf("cliente: recebeu '%s'\n", received_message);
        free(msg);
    }

    close(sockfd); // Fecha o socket

    return 0;
}
