// Código baseado no Beej's guide, especialmente no capítulo 6
#include "../include/client_server.h"
#include "../include/database.h"

// Função que garante que todos os bytes serão enviados
int send_all(int dest_socket, char *msg, int *len, struct sockaddr *their_addr, socklen_t addr_len) {
    int total = 0; // Número de bytes enviados
    int n, message_size, message_size_size, added_bytes;
    int counter = 0;                  // Número do pacote
    char datagram[MAX_DATA_SIZE - 1]; // Buffer para o pacote
    char message_size_string[10];

    // Caso não encontre nenhum perfil correspondente a query
    if (*len == 0) {
        strcpy(msg, "Não existe nenhum perfil com essas características!\n");
        *len = strlen(msg);
    }

    snprintf(message_size_string, 20, "%ld ", strlen(msg)); // Obtém o tamanho da mensagem em string

    // Garante que todos os bytes serão enviados
    while (total < *len) {
        memset(datagram, 0, MAX_DATA_SIZE - 1); // Limpa o vetor datagram
        added_bytes = 0;                        // Conta o número de bytes adicionados

        // Adiciona número do datagrama a ser enviado
        if (counter < 10) {
            // Caso seja menor que 10, serão inseridos apenas 1 número inteiro (4 bytes) e 1 espaço (1 byte) = 5 bytes
            snprintf(datagram, 5, "%d ", counter); // Converte os bytes inseridos para char
        } else if (counter < 100) {
            // OBS: Como o tamanho máximo da mensagem é 250 * MAX_DATA_SIZE, counter é no máximo 250
            // Caso maior que 10 e menor que 100, serão inseridos apenas 2 números inteiros (8 bytes) e 1 espaço (1 byte) = 9 bytes
            snprintf(datagram, 9, "%d ", counter); // Converte os bytes inseridos para char
        } else {
            // Caso maior que 100, serão inseridos apenas 3 números inteiros (12 bytes) e 1 espaço (1 byte) = 13 bytes
            snprintf(datagram, 13, "%d ", counter); // Converte os bytes inseridos para char
        }

        // Adiciona tamanho da mensagem em todos os datagramas
        strcat(datagram, message_size_string); // Adiciona o tamanho da mensagem logo após o número do pacote
        added_bytes = strlen(datagram);        // Contém o número de byts adicionados (tamanho da mensagem + número do pacote)

        strncat(datagram, msg + total, MAX_DATA_SIZE - 1 - added_bytes); // Concatena no início da mensagem
        total -= added_bytes;                                            // Subtrai os bytes adicionados da contagem de bytes da mensagem enviados

        // Envia MAX_DATA_SIZE - 1 bytes por vez
        n = sendto(dest_socket, datagram, MAX_DATA_SIZE - 1, 0,
                   their_addr, addr_len);

        if (n == -1) {
            break;
        }

        total += n;
        counter++;
    }

    *len = total; // Número de bytes realmente enviados

    if (n == -1) {
        return -1; // Em caso de algum erro
    }

    return 0; // Em caso de sucesso
}

// Função que transforma a lista de resultados em uma única string
void list_to_string(result **res, char *op_result) {
    result *r;
    char result[MAX_BUFFER_SIZE];
    memset(result, 0, MAX_BUFFER_SIZE);

    while (*res) {
        r = *res;
        strcat(result, r->row);
        *res = r->next;
        free(r->row);
        free(r);
    }

    // Copia o resultado obtido para o ponteiro de resposta
    strncpy(op_result, result, strlen(result));
}

// Função que executa a operação do cliente e retorna o seu resultado
void execute_query(sqlite3 *db, char *query, char *op_result)
{
    char *email, *first_name, *last_name, *location, *major, *grad_year, *abilities;
    int graduation_year;
    char ret[30];     // Retorno da operação
    profile p;        // Novo perfil
    result *res;      // Resultado da operação

    // Obtém o número da operação pedida pelo cliente
    long op_num = strtol(query, NULL, 10);

    switch (op_num) {
        // Obtém todos os perfis
        case 1:
            get_all_profiles(db, &res);
            list_to_string(&res, op_result);
            break;

        // Obtém todas as informações de um perfil, dado seu email
        case 2:
            strtok(query, "&");        // Despreza o número da operação
            email = strtok(NULL, "&"); // Armazena o email especificado
            get_profile(db, &res, email);
            list_to_string(&res, op_result);
            break;

        // Obtém email e nome dos perfis com uma determinada formação acadêmica
        case 3:
            strtok(query, "&");        // Despreza o número da operação
            major = strtok(NULL, "&"); // Armazena o curso especificado
            get_profiles_from_major(db, &res, major);
            list_to_string(&res, op_result);
            break;

        // Obtém email e nome dos perfis com uma determinada habilidade
        case 4:
            strtok(query, "&");            // Despreza o número da operação
            abilities = strtok(NULL, "&"); // Armazena a habilidade especificada
            get_profiles_from_ability(db, &res, abilities);
            list_to_string(&res, op_result);
            break;

        // Obtém email, nome e curso dos perfis com determinado ano de graduação
        case 5:
            strtok(query, "&");                // Despreza o número da operação
            grad_year = strtok(NULL, "&");     // Armazena, em string, o ano de graduação especificado
            graduation_year = atoi(grad_year); // Converte o ano de graduação para inteiro
            get_profiles_from_graduation_year(db, &res, graduation_year);
            list_to_string(&res, op_result);
            break;

        // Obtém a imagem de um perfil, dado seu email
        case 6:
            strtok(query, "&");        // Despreza o número da operação
            email = strtok(NULL, "&"); // Armazena o email especificado
            get_profile_image(db, &res, email);
            list_to_string(&res, op_result);

            // Deleta a imagem retornada pela operação
            char *ret_path = malloc(500);
            sprintf(ret_path, "%s/%s", IMG_PATH, RET_IMG);
            // remove(ret_path);
            free(ret_path);

            break;

        // Adiciona um novo perfil ao banco de dados
        case 7:
            strtok(query, "&"); // Despreza o número da operação

            p.email = strtok(NULL, "&");         // Armazena o email especificado
            p.first_name = strtok(NULL, "&");    // Armazena o nome especificado
            p.last_name = strtok(NULL, "&");     // Armazena o sobrenome especificado
            p.location = strtok(NULL, "&");      // Armazena o local especificado
            p.major = strtok(NULL, "&");         // Armazena o curso especificado
            grad_year = strtok(NULL, "&");       // Armazena, em string, o ano de graduação especificado
            p.graduation_year = atoi(grad_year); // Converte o ano de graduação para inteiro
            p.abilities = strtok(NULL, "&");     // Armazena as habilidades especificadas

            if (register_profile(db, p) == 0) {
                strcpy(ret, "Perfil cadastrado com sucesso");
            } else {
                strcpy(ret, "Falha no cadastro do perfil");
            }

            snprintf(op_result, 30, "%s", ret);
            break;

        // Remove um perfil do banco de dados de acordo com o email
        case 8:
            strtok(query, "&");        // Despreza o número da operação
            email = strtok(NULL, "&"); // Armazena o email especificado

            if (remove_profile(db, email) == 0) {
                strcpy(ret, "Perfil removido com sucesso");
            } else {
                strcpy(ret, "Falha na remoção do perfil");
            }

            snprintf(op_result, 30, "%s", ret);
            break;

        default:
            break;
    }
}

int main(void) {
    sqlite3 *db = open_db(DB_PATH); // Abre a conexão com o banco de dados
    initialize_db(db);              // Inicializa o banco de dados com os perfis default

    int socket_fd;                        // Socket do servidor
    struct addrinfo hints, *servinfo, *p; // Posteriormente vão guardar informações de endereço
    struct sockaddr_storage their_addr;   // Guarda informações do endereço dos clientes
    socklen_t addr_len;                   // Guarda informações sobre o tamanho do socket
    int yes = 1;
    char s[INET6_ADDRSTRLEN]; // Buffer que será utilizado em caso de IPv6 da parte do cliente
    int rv;

    memset(&hints, 0, sizeof hints); // Garantir que todos os bits estão zerados
    hints.ai_family = AF_UNSPEC;     // Deixa para definir se é IPv4 ou IPv6 posteriormente
    hints.ai_socktype = SOCK_DGRAM;  // Define socket tipo UDP
    hints.ai_flags = AI_PASSIVE;     // Pega o IP da própria máquina

    // Pega informações sobre o endereço do socket do servidor
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

    printf("servidor: esperando por requisições...\n");

    while (1) { // Loop em que o servidor espera por requisições e as atende
        int len, numbytes;
        char buf[MAX_DATA_SIZE];   // Buffer para receber mensagem
        char msg[MAX_BUFFER_SIZE]; // Mensagem a ser enviada
        memset(msg, 0, MAX_BUFFER_SIZE);
        addr_len = sizeof their_addr;

        // Recebe os bytes enviados pelo cliente
        if ((numbytes = recvfrom(socket_fd, buf, MAX_DATA_SIZE - 1, 0,
                                 (struct sockaddr*)&their_addr, &addr_len)) == -1) {
            perror("recv");
            exit(1);
        }

        buf[numbytes] = '\0'; // Adiciona caractere para marcar o final da string

        // Obtém endereço do cliente, sendo IPv4 ou IPv6
        inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr*)&their_addr), s, sizeof s);
        printf("servidor: requisição de %s\n", s);

        printf("servidor: recebido '%s'\n", buf);

        // Executa a operação pedida pelo cliente e põe a resposta em msg
        execute_query(db, buf, msg);
        len = strlen(msg);

        if (send_all(socket_fd, msg, &len, (struct sockaddr*)&their_addr, addr_len) == -1) {
            // Envia mensagem ao cliente que enviou a requisição
            perror("sendto");
            printf("Somente %d bytes foram enviados com sucesso.\n", len);
        }
    }

    close(socket_fd); // Fecha o socket do servidor
    close_db(db);     // Encerra a conexão com o banco de dados

    return 0;
}
