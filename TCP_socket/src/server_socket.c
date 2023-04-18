#include <arpa/inet.h>
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

#define PORT "8330" // Porta não utilizada por nenhum outro processo. Será responsável por receber conexões
#define BACKLOG 10  // Número máximo de conexões pendentes na fila

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
        {                                                   // Cria processo filho para lidar com as conexões aceitas
                                                            // Esse trecho só é executado pelos processos filhos
            close(socket_fd);                               // O socket que aceita conexões deve ser fechado para os processos filhos
            if (send(new_fd, "Hello, world!", 13, 0) == -1) // Envia mensagem ao cliente conectado a este processo filho
                perror("send");
            close(new_fd);
            exit(0);
        }
        close(new_fd); // Processo pai deve fechar o socket destinado aos processos filhos
    }

    return 0;
}
