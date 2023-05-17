// Código baseado no Beej's guide, especialmente no capítulo 6
#include "../include/client_server.h"
#include "../include/database.h"

// Função que garante que todos os bytes serão enviados
int send_all(int dest_socket, char *msg, int *len, struct addrinfo* p) {
    int total = 0; // Número de bytes enviados
    int n;

    // Garante que todos os bytes serão enviados
    while (total < *len) {
        // Envia MAX_DATA_SIZE - 1 bytes por vez
        n = sendto(dest_socket, msg + total, MAX_DATA_SIZE - 1, 0,
        p->ai_addr, p->ai_addrlen);

        if (n == -1) {
            break;
        }

        total += n;
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

    while(*res) {
        r = *res;
        strcat(result, r->row);
        *res = r->next;
        free(r->row);
        free(r);
    }

    int length = strlen(result); // Obtém tamanho da string
    snprintf(op_result, 20, "%d", length);
    snprintf(op_result, 20, "%ld", length+strlen(op_result));

    // Concatena, no início da resposta, o número de caracteres presentes nela
    strcat(op_result, result);
}

// Função que executa a operação do cliente e retorna o seu resultado
void execute_query(sqlite3 *db, char *query, char *op_result) {
    char *email, *first_name, *last_name, *location, *major, *grad_year, *abilities;
    int graduation_year;
    char ret[30]; // Retorno da operação
    profile p;    // Novo perfil
    result *res;  // Resultado da operação

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

        // Adiciona um novo perfil ao banco de dados
        case 6:
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
        case 7:
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
    socklen_t addr_len;         // Guarda informações sobre o tamanho do socket
    int yes = 1;
    char s[INET6_ADDRSTRLEN]; // Buffer que será utilizado em caso de IPv6 da parte do cliente
    int rv;

    memset(&hints, 0, sizeof hints); // Garantir que todos os bits estão zerados
    hints.ai_family = AF_UNSPEC;     // Deixa para definir se é IPv4 ou IPv6 posteriormente
    hints.ai_socktype = SOCK_DGRAM; // Define socket tipo UDP
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

    printf("servidor: esperando por conexões...\n");

    while (1) { // Loop em que o servidor espera por conexões e as atende

        int len, numbytes;
        char buf[MAX_DATA_SIZE];   // Buffer para receber mensagem
        char msg[MAX_BUFFER_SIZE]; // Mensagem a ser enviada
        addr_len = sizeof their_addr;

        // Recebe os bytes enviados pelo cliente
        if ((numbytes = recvfrom(socket_fd, buf, MAX_DATA_SIZE - 1, 0,
        (struct sockaddr *) &their_addr, &addr_len)) == -1) {
            perror("recv");
            exit(1);
        }

        buf[numbytes] = '\0'; // Adiciona caractere para marcar o final da string

        // Obtém endereço do cliente, sendo IPv4 ou IPv6
        inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
        printf("servidor: conexão com %s\n", s);
        
        printf("servidor: recebido '%s'\n", buf);

        // Executa a operação pedida pelo cliente e põe a resposta em msg
        execute_query(db, buf, msg);
        len = strlen(msg);

        if (send_all(socket_fd, msg, &len, p) == -1) { 
            // Envia mensagem ao cliente conectado a este processo filho
            perror("send");
            printf("Somente %d bytes foram enviados com sucesso.\n", len);
        }
        
    }

    close(socket_fd); // Fecha o socket do servidor
    close_db(db); // Encerra a conexão com o banco de dados

    return 0;
}
