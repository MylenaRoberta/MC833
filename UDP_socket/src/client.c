// Código baseado no Beej's guide, especialmente no capítulo 6
#include "../include/client_server.h"
#include <poll.h>

#define TIMEOUT_MS 5000
#define RET_IMG_NAME "op_return_img.png"
#define DOWNLOAD_PATH "data/download"

// Função que imprime as opções de operações
void print_menu(int admin) {
    printf("----------------------------------------------------------------------\n");
    printf("Digite o número da operação que você deseja:\n");

    printf("[1] Listar todas as informações de todos os perfis\n");
    printf("[2] Listar todas as informações de um perfil\n");
    printf("[3] Listar todas as pessoas formadas em um determinado curso\n");
    printf("[4] Listar todas as pessoas que possuem uma determinada habilidade\n");
    printf("[5] Listar todas as pessoas formadas em um determinado ano\n");
    printf("[6] Fazer o download da imagem de um perfil\n");

    if (admin) { // Cliente administrador
        printf("[7] Cadastrar um novo perfil\n");
        printf("[8] Remover um perfil a partir de seu email\n");
    }

    printf("[0] Sair\n");
    printf("----------------------------------------------------------------------\n");
}

// Função obtém a operação desejada pelo cliente e os seus parâmetros, se necessário
char* get_client_operation(int admin) {
    char email[50], first_name[50], last_name[50], location[50], major[50], grad_year[4], abilities[100];

    int option;
    print_menu(admin); // Exibe o menu de operações
    printf("+ Operação: ");
    scanf("%d", &option); // Obtém o número da operação

    if (option == 0) { // Cliente não deseja fazer mais nenhuma requisição
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
            printf("> Digite o email desejado: ");
            scanf("%s", email);
            strcat(param, email);
            break;

        case 7:
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

        case 8:
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
    hints.ai_socktype = SOCK_DGRAM;  // Socket UDP

    // Pega informações sobre o endereço do servidor
    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // Faz um loop pelos resultados e "conecta" com o primeiro possível
    // Quando se trata de UDP, não existe conexão no sentido tradicional,
    // mas a função connect() faz com que não precisemos especificar o IP do servidor a cada send() e recv()
    // Usaremos a palavra conectar para se referir a isso, mesmo que ela não possua o sentido tradicional
    for (p = servinfo; p != NULL; p = p->ai_next) {
        // Erro ao criar socket do cliente
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("cliente: socket");
            continue;
        }

        // Erro ao "conectar"
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
    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
    printf("cliente: conectando a %s\n", s);

    freeaddrinfo(servinfo); // Libera, pois as informações não serão mais necessárias

    // Criando a struct pollfd para o polling
    struct pollfd fds[1];
    fds[0].fd = sockfd;
    fds[0].events = POLLIN;

    while (1) {
        // Obtém os parâmetros da operação, se necessário, especificados pelo cliente
        char *msg = get_client_operation(admin);

        if (msg == NULL) { // Cliente não deseja fazer mais nenhuma requisição
            break;
        }

        int len = strlen(msg);
        int option = atoi(&msg[0]);             // Operação selecionada pelo usuário
        char received_message[MAX_BUFFER_SIZE]; // Buffer para guardar a mensagem recebida
        char buf[MAX_DATA_SIZE];                // Buffer para receber mensagem
        memset(buf, 0, MAX_DATA_SIZE);
        memset(received_message, 0, MAX_BUFFER_SIZE);

        // Envia mensagem ao servidor indicando operação e seus parâmetros
        if (send(sockfd, msg, len, 0) == -1) {
            perror("send");
            printf("Somente %d bytes foram enviados com sucesso\n", len);
            continue;
        }

        int received_bytes = 0;
        int datagram_number, dismissed_bytes, datagram_position, timeout;
        long bytes_to_be_received = 0;
        int total = MAX_DATA_SIZE - 1;
        char message_size_string[10];

        do { // Garante que todos os bytes serão recebidos ou que a operação será abortada
            timeout = 0;

            // Fazendo polling
            int numReady = poll(fds, 1, TIMEOUT_MS);

            if (numReady == -1) { // Erro de polling
                perror("Poll error");
                exit(EXIT_FAILURE);
            }

            else if (numReady == 0) { // Ocorreu timeout
                timeout = 1;
                break;
            }
            
            else if (fds[0].revents & POLLIN) {
                // Recebe os bytes enviados pelo servidor
                if ((numbytes = recv(sockfd, buf, total, 0)) == -1) {
                    perror("recv");
                    exit(1);
                }

                // Adiciona caractere para marcar o final da string
                buf[MAX_DATA_SIZE - 1] = '\0';

                // Obtém o número do pacote, através do primeiro inteiro na mensagem
                datagram_number = strtol(buf, NULL, 10);

                // Calcula o número de bytes correspondentes ao número do pacote e ao espaço adicionado, para remover da mensagem
                if (datagram_number < 10) {
                    dismissed_bytes = 2;
                } else if (datagram_number < 100) {
                    // OBS: Como o tamanho máximo da mensagem é 250 * MAX_DATA_SIZE, datagram_number é no máximo 250
                    dismissed_bytes = 3;
                } else {
                    dismissed_bytes = 4;
                }

                // Na primeira iteração, lê quantos bytes serão enviados, lembrando que os pacotes podem vir fora de ordem
                if (bytes_to_be_received == 0) {
                    // Verifica quantos bytes deveriam ser recebidos, através do primeiro inteiro depois do número do pacote e do espaço adicionado
                    bytes_to_be_received = strtol(buf + dismissed_bytes, NULL, 10);
                    snprintf(message_size_string, 20, "%ld ", bytes_to_be_received); // String que armazena o tamanho da mensagem
                }

                dismissed_bytes += strlen(message_size_string); // Contém o número de bytes a ser desprezado (tamanho da mensagem e número do pacote)

                // Calcula a posição em que o datagrama será inserido no buffer da mensagem final
                if (datagram_number < 10) {
                    // Caso o número de datagramas seja menor que 10, ele adicionou sempre o mesmo número de bytes de cabeçalho
                    datagram_position = datagram_number * (total - dismissed_bytes);
                } else if (datagram_number < 100) {
                    // Caso o número de datagramas seja maior ou igual a 10 e menor que 100, ele adicionou 1 byte de cabeçalho a menos nos 10 primeiros datagramas
                    // Por isso, adicionamos esse 10, para compensar essa diferença, já que o dismissed bytes agora é 1 número maior que para datagramas de número menor que 10
                    datagram_position = datagram_number * (total - dismissed_bytes) + 10;
                } else {
                    datagram_position = datagram_number * (total - dismissed_bytes) + 10 + 100;
                }

                // Copia bytes recebidos para buffer final que armazena toda a mensagem, descartando o tamanho da mensagem, o número do pacote e o espaço adicionado
                memcpy(received_message + datagram_position, buf + dismissed_bytes, numbytes - dismissed_bytes);
                // Incrementa o número de bytes recebidos, descontando o tamanho da mensagem, o número do pacote e o espaço adicionado
                received_bytes += numbytes - dismissed_bytes;
            }
        } while (bytes_to_be_received > received_bytes);

        if (!timeout && strlen(received_message) > 0) {
            if (option == 6) {
                // Monta o caminho da imagem recuperada
                char *file_path = malloc(500);
                sprintf(file_path, "%s/%s", DOWNLOAD_PATH, RET_IMG_NAME);

                FILE *fp = fopen(file_path, "wb");                 // Cria um arquivo para salvar a imagem
                int bytes = sizeof(received_message) - 1;          // Calcula o tamanho da imagem em bytes
                fwrite(received_message, 1, bytes, fp); // Armazena a imagem no arquivo criado

                fclose(fp); // Fecha o file handler
                free(file_path);

                // Monta a mensagem a ser exibida para o cliente
                printf("cliente: recebeu a imagem '%s' em '%s'\n", RET_IMG_NAME, DOWNLOAD_PATH);

            } else {
                printf("cliente: recebeu '%s'\n", received_message);
            }

            free(msg);
        } else if (strlen(received_message) == 0 && !timeout) {
            printf("Houve algum erro ao tentar se comunicar com o servidor. Verifique se ele está funcionando corretamente!\n");
        } else {
            printf("A requisição demorou tempo demais para ser respondida. Tente novamente!\n");
        }
    }

    close(sockfd); // Fecha o socket

    return 0;
}
