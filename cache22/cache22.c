#include "cache22.h"

bool scontinuation;
bool ccontinuation;

void zero(int8* buf, int16 size) {
    int8 *p;
    int16 n; 

    for (n = 0, p = buf; n < size; ++n, ++p) {
        *p = 0;
    }
    return;
}

void childLoop(Client *cli) {
    sleep(1);
    return;
}

void mainLoop(int sock) {
    struct sockaddr_in clientAddress;
    int32 len;

    int client_socket = accept(sock, (struct sockaddr *)&clientAddress, (unsigned int*)&len);
    if (client_socket < 0) {
        sleep(1);
        return;
    }

    int16 port = (int16)htons((int)clientAddress.sin_port);
    char *ip = inet_ntoa(clientAddress.sin_addr);
    printf("Client Connection from %s:%d\n", ip, port);

    Client *client = (Client *)malloc(sizeof(struct s_client));
    assert(client);
    zero((int8 *)client, sizeof(struct s_client));
    
    client->s = sock;
    client->port = port;
    strncpy(client->ip, ip, 15);

    pid_t pid = fork();
    if (pid) {
        free(client);
        return;
    } else {
        dprintf(client_socket, "100 Connected to Cache22 server\n");
        ccontinuation = true;
        while (ccontinuation) {
            childLoop(client);
        }
        close(client_socket);
        free(client);
        return;
    }
    return;
}

int initServer(int16 port) {
    struct sockaddr_in addressInfo;
    addressInfo.sin_family = AF_INET; 
    addressInfo.sin_port = htons((int)port); // htons ensures port number is in network byte order
    addressInfo.sin_addr.s_addr = inet_addr(HOST); // inet_addr converts a dotted-decimal string to uint32_t in network byte order

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    assert(serverSocket >= 0);

    if (bind(serverSocket, (struct sockaddr *)&addressInfo, sizeof(addressInfo))) {
        perror("Bind failed");
        assert(false);
    }
    if (listen(serverSocket, 20)) {
        perror("Listen failed");
        assert(false);
    }
    printf("Server listening on %s:%d\n", HOST, port);
    fflush(stdout);
    return serverSocket;
}

int main(int argc, char *argv[]) {
    char *sport;

    if (argc < 2) sport = PORT;
    else sport = argv[1];

    int sock = initServer((int16)atoi(sport));
    scontinuation = true;

    while (scontinuation) {
        mainLoop(sock);
    }

    printf("Shutting down");
    close(sock);
    return 0;
}