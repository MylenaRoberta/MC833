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

#define PORT "8330"             // Porta não utilizada por nenhum outro processo. Será responsável por receber conexões
#define BACKLOG 10              // Número máximo de conexões pendentes na fila

// Função responsável por finalizar processos zumbis
void sigchild_handler(int s)
{
    int errno_backup = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = errno_backup;       // O waitpid pode mudar o valor do errno, mas queremos preservá-lo
}

// Função responsável por retornar o endereço do socket adequado, seja IPv4 ou IPv6
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);          // IPv4
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);            // IPv6
}

int main(void) {
    int socket_fd;                          // Escuta conexões
    int new_fd;                             // Novas conexões
    int yes=1;
    char s[INET6_ADDRSTRLEN];

    struct addrinfo hints, *servinfo, *p;   // posteriormente vão guardar informações de endereço
    struct sockaddr_storage their_addr;     // Guarda informações do endereço dos clientes
    socklen_t sin_size;                     // Guarda informações sobre o tamanho do socket
    struct sigaction sa;                    // Para tratar processos zumbis

    memset(&hints, 0, sizeof hints);        // Garantir que todos os bits estão zerados
    hints.ai_family = AF_UNSPEC;            // Deixa para definir se é IPv4 ou IPv6 posteriormente
    hints.ai_socktype = SOCK_STREAM;        // Define socket tipo TCP
    hints.ai_flags = AI_PASSIVE;            // Pega o IP da própria máquina

    int rv;

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((socket_fd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(socket_fd, p->ai_addr, p->ai_addrlen) == -1) {
            close(socket_fd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(socket_fd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchild_handler; // finaliza processos zumbis
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    printf("server: waiting for connections...\n");

    while(1) {  // main accept() loop
        sin_size = sizeof their_addr;
        new_fd = accept(socket_fd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family,
            get_in_addr((struct sockaddr *)&their_addr),
            s, sizeof s);
        printf("server: got connection from %s\n", s);

        if (!fork()) { // this is the child process
            close(socket_fd); // child doesn't need the listener
            if (send(new_fd, "Hello, world!", 13, 0) == -1)
                perror("send");
            close(new_fd);
            exit(0);
        }
        close(new_fd);  // parent doesn't need this
    }

    return 0;
}
