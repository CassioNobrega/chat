#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h> //struct sockaddr_in
#include <netdb.h> //struct hostent
#include <string.h> //memset()
#include <unistd.h> //write()
#include <signal.h> //signal()

void error(char *message);
void handle_signal(int signal);
volatile __sig_atomic_t is_connected = 1;

int main (int argc, char *argv[])
{
    int socket_client, port;
    struct sockaddr_in server_addr;
    struct hostent *server;
    char buffer[256];
    //signal(SIGINT, handle_signal);
    //signal(SIGTERM, handle_signal);
    if (argc < 3) {
        fprintf(stderr,"usage: %s 0.0.0.0 8080\n", argv[0]);
        exit(-1);
    }
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr, "error, host nao encontrado\n");
    }
    port = atoi(argv[2]);
    socket_client = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_client < 0) {
        error("error ao criar socket");
    }
    // zera server_addr
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    //memcpy(&server_addr.sin_addr.s_addr, "192.168.0.10", server->h_length);
    memcpy(&server_addr.sin_addr.s_addr, server->h_addr_list[0], server->h_length);
    // zera o buffer
    if (connect(socket_client, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        error("error de conexao");
    }
    printf("envie sua mensagem:\n");
    memset(buffer, 0, 256);
    while (is_connected) {
        fgets(buffer, 255, stdin);
        if (send(socket_client, buffer, strlen(buffer), 0) < 0) {
            error("error ao enviar mensagem");
        }
        memset(buffer, 0, 256);
    }
    close(socket_client);
    return 0;
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
        printf("\nctrl + c recebido. Encerrando conexao.\n");
        break;
    default:
        printf("sinal %d recebido. Encerrando conexao.\n");
        break;
    }
    is_connected = 0;
}
