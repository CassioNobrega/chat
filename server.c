#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> //const e structs necessárias para endereços de domínio da internet.
#include <unistd.h>     // close()
#include <sys/wait.h>  // wait()
#include <string.h> //memset()
#include <arpa/inet.h> //inet_ntoa()
#include <signal.h> // sig_atomic_t
#include <fcntl.h> //fcntl()


void error(char *message);
void handle_signal(int signal);

volatile sig_atomic_t is_running = 1; // variável segura para sinal
volatile sig_atomic_t is_conected = 1; // variável segura para sinal

int main(int argc, char *argv[])
{
    struct sockaddr_in serv_addr, client_addr;
    socklen_t client_length;
    int socket_fd, new_sock_fd, port;
    char buffer[256];
    int opt = 1;
    pid_t pid;
    if (argc < 2) {
        error("porta nao providenciada");
    }
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);
    port = atoi(argv[1]);
    // 1. criar novo socket
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        error("erro ao criar socket");
    }
    // 2. Configurar opções (permitir reuso da porta)
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    // 3. Definir o endereço do servidor
    serv_addr.sin_family = AF_INET; // IPv4
    serv_addr.sin_addr.s_addr = INADDR_ANY; // Qualquer interface
    serv_addr.sin_port = htons(port);
    // 4. Associar (bind) o socket ao endereço
    if (bind(socket_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        error("bind falhou");
    }
    // 5. Colocar em modo de escuta
    if (listen(socket_fd, 3) < 0) {
        error("listen falhou");
    }
    printf("Servidor escutando na porta %d...\n", port);
    while (is_running) {
        client_length = sizeof(client_addr);
        new_sock_fd = accept(socket_fd, (struct sockaddr *) &client_addr, &client_length);
        if (new_sock_fd < 0) {
                error("accept falhou");
        }
        pid = fork();
        switch (pid) {
        case -1:
            error("error ao criar processo");
        case 0:
            // processo filho
            char *host_name = inet_ntoa(client_addr.sin_addr);
            close(socket_fd);
            memset(buffer, 0, 256);
            while (is_conected)
            {
                if (read(new_sock_fd, buffer, 255) < 0) {
                error("error ao ler socket");
                }
                printf("mensagem de %s: %s", host_name, buffer);
                if (write(new_sock_fd, "programa finalizado", 19) < 0) {
                    error("error ao enviar mensagem");
                }
                memset(buffer, 0, 256);
            }            
            close(new_sock_fd);
            printf("conexao encerrada\n");
            exit(0);
        default:
            // processo pai
            break;
        }
    }
}


void error(char *message)
{
    perror(message);
    exit(EXIT_FAILURE);
}

void handle_signal(int signal)
{
    switch (signal)
    {
    case 2:
        printf("\nctrl + c recebido, encerrando...\n", signal);
        break;
    default:
        printf("\nSinal recebido (%d), encerrando...\n", signal);
        break;
    }
    is_running = 0;
    exit(0);
}
