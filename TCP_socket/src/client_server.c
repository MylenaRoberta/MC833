#include "client.h"
#include "server.h"
#include "client_server.h"

void* get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *)sa)->sin_addr); // IPv4
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);   // IPv6
}

void print_menu(int admin) {
    // Exibição do menu de operações
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

void sigchild_handler(int s) {
    int errno_backup = errno;

    while (waitpid(-1, NULL, WNOHANG) > 0);

    // O waitpid pode mudar o valor do errno, mas queremos preservá-lo
    errno = errno_backup;
}

// Função que garante que todos os bytes serão enviados
int send_all(int dest_socket, char *msg, int *len) {
    int total = 0; // Número de bytes enviados
    int n;

    // Garante que todos os bytes serão enviados
    while (total < *len) {
        // Envia MAX_DATA_SIZE - 1 bytes por vez
        n = send(dest_socket, msg + total, MAX_DATA_SIZE - 1, 0);

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

char* execute_query(sqlite3 *db, char *query, char *query_result) {
    char *email, *first_name, *last_name, *location, *major, *grad_year, *abilities;
    int graduation_year;
    profile p;  // Novo perfil
    result *res; // Resultado da query

    // Obtém o número da operação pedida pelo cliente
    long query_option = strtol(query, NULL, 10);

    switch (query_option) {
        // Obtém todos os perfis
        case 1:
            get_all_profiles(db, res);
            //transform_profile_array(&res, query_result);
            break;

        // Obtém todas as informações de um perfil, dado seu email
        case 2:
            strtok(query, "&");        // Despreza o número da operação
            email = strtok(NULL, "&"); // Armazena o email especificado
            get_profile(db, &res, email);
            //transform_profile_array(&res, query_result);
            break;

        // Obtém email e nome dos perfis com uma determinada formação acadêmica
        case 3:
            strtok(query, "&");        // Despreza o número da operação
            major = strtok(NULL, "&"); // Armazena o curso especificado
            get_profiles_from_major(db, &res, major);
            //transform_profile_array(&res, query_result);
            break;

        // Obtém email e nome dos perfis com uma determinada habilidade
        case 4:
            strtok(query, "&");            // Despreza o número da operação
            abilities = strtok(NULL, "&"); // Armazena a habilidade especificada
            get_profiles_from_ability(db, &res, abilities);
            //transform_profile_array(&res, query_result);
            break;

        // Obtém email, nome e curso dos perfis com determinado ano de graduação
        case 5:
            strtok(query, "&");                // Despreza o número da operação
            grad_year = strtok(NULL, "&");     // Armazena, em string, o ano de graduação especificado
            graduation_year = atoi(grad_year); // Converte o ano de graduação para inteiro
            get_profiles_from_graduation_year(db, &res, graduation_year);
            //transform_profile_array(&res, query_result);
            break;

        // Adiciona um novo perfil ao banco de dados
        case 6:
            strtok(query, "&");                  // Despreza o número da operação
            p.email = strtok(NULL, "&");         // Armazena o email especificado
            p.first_name = strtok(NULL, "&");    // Armazena o nome especificado
            p.last_name = strtok(NULL, "&");     // Armazena o sobrenome especificado
            p.location = strtok(NULL, "&");      // Armazena o local especificado
            p.major = strtok(NULL, "&");         // Armazena o curso especificado
            grad_year = strtok(NULL, "&");       // Armazena, em string, o ano de graduação especificado
            p.graduation_year = atoi(grad_year); // Converte o ano de graduação para inteiro
            p.abilities = strtok(NULL, "&");     // Armazena as habilidades especificadas

            snprintf(query_result, 20, "%d", register_profile(db, p));
            break;

        // Remove um perfil do banco de dados de acordo com o email
        case 7:
            strtok(query, "&");        // Despreza o número da operação
            email = strtok(NULL, "&"); // Armazena o email especificado
            snprintf(query_result, 20, "%d", remove_profile(db, email));
            break;

        default:
            break;
    }
}
