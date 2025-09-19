#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h> //struct sockaddr_in
#include <netdb.h> //struct hostent
#include <string.h> //memset()
#include <unistd.h> //write()


void error(char *mensage);


int main (int argc, char *argv[])
{
    int socket_client, port;
    struct sockaddr_in server_addr;
    struct hostent *server;
    char buffer[256];
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
    memset(buffer, 0, sizeof(buffer));
    if (connect(socket_client, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        error("error de conexao");
    }
    printf("envie sua mensagem:\n");
    fgets(buffer, 255, stdin);
    if (write(socket_client, buffer, strlen(buffer)) < 0) {///strlen?
        error("error ao enviar mensagem");
    }
    if (read(socket_client, buffer, 255) < 0) {
        error("error ao enviar mensagem");
    }
    printf("%s\n",buffer);
    close(socket_client);
    return 0;
}


void error(char *mensage)
{
    perror(mensage);
    exit(EXIT_FAILURE);
}
