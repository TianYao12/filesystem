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

void mainLoop(int s) {
    struct sockaddr_in cli;
    int32 len;
    int s2;
    char *ip;
    int16 port;
    Client *client;
    pid_t pid;

    s2 = accept(s, (struct sockaddr *)&cli, (unsigned int*)&len);
    if (s2 < 0) {
        sleep(1);
        return;
    }
    port = (int16)htons((int)cli.sin_port);
    ip = inet_ntoa(cli.sin_addr);
    printf("Connection from %s:%d\n", ip, port);

    client = (Client *)malloc(sizeof(struct s_client));
    assert(client);

    zero((int8 *)client, sizeof(struct s_client));
    client->s = s;
    client->port = port;
    strncpy(client->ip, ip, 15);

    pid = fork();
    if (pid) {
        free(client);
        return;
    } else {
        dprintf(s2, "100 Connected to Cache22 server\n");
        ccontinuation = true;
        while (ccontinuation) {
            childLoop(client);
        }
        close(s2);
        free(client);
        return;
    }
    return;
}

int initServer(int16 port) {
    struct sockaddr_in sock;
    int s;

    sock.sin_family = AF_INET;
    sock.sin_port = htons((int)port);
    sock.sin_addr.s_addr = inet_addr(HOST);

    s = socket(AF_INET, SOCK_STREAM, 0);
    assert(s >= 0);

    if (bind(s, (struct sockaddr *)&sock, sizeof(sock))) {
        perror("Bind failed");
        assert(false);
    }
    if (listen(s, 20)) {
        perror("Listen failed");
        assert(false);
    }
    printf("Server listening on %s:%d\n", HOST, port);
    fflush(stdout);
    return s;
}

int main(int argc, char *argv[]) {
    char *sport;

    if (argc < 2) {
        sport = PORT;
    } else {
        sport = argv[1];
    }
    int s = initServer((int16)atoi(sport));

    scontinuation = true;
    while (scontinuation) {
        mainLoop(s);
    }
    printf("Shutting down");
    close(s);
    return 0;
}