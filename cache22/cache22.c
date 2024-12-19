#include "cache22.h"

bool scontinuation;
bool ccontinuation;

CommandHandler handlers[] = {
    { (int8 *)"hello", handleHello }
};

Callback getCommand(int8 *cmd) {
    Callback cb;
    int16 n, arrLen;
    if (sizeof(handlers) < 16) return 0;
    arrLen = (sizeof(handlers) / 16);
    cb = 0;

    for (n = 0; n < arrLen; n++) {
        if (!strcmp((char *)cmd, (char *)handlers[n].cmd)) {
            cb = handlers[n].handler;
            break;
        }
    }
    return cb;
}

int32 handleHello(Client *cli, int8 *folder, int8 *args) {
  dprintf(cli->s, "hello %s\n", folder);
  return 0;  
}

void zero(int8* buf, int16 size) {
    int8 *p;
    int16 n; 

    for (n = 0, p = buf; n < size; ++n, ++p) {
        *p = 0;
    }
    return;
}

void childLoop(Client *cli) {
    int8 buf[256];
    int8 cmd[256], folder[256], args[256];
    int8 *p, *f;

    zero(buf, 256);
    zero(cmd, 256);
    zero(folder, 256);
    zero(args, 256);

    read(cli->s, buf, 255);
    int16 n = (int16)strlen((char *)buf);

    for (p = buf; (*p) && (n--) && (*p != ' ') && (*p != '\n') && (*p != '\r'); p++);
    strncpy((char *)cmd, (char *)buf, p - buf);

    // Skip whitespace
    while ((*p == ' ') || (*p == '\n') || (*p == '\r')) {
        *p++ = 0;
        n--;
    }

    // Extract folder
    f = p;
    for (; (*p) && (n--) && (*p != ' ') && (*p != '\n') && (*p != '\r'); p++);
    strncpy((char *)folder, (char *)f, p - f);

    // Skip whitespace
    while ((*p == ' ') || (*p == '\n') || (*p == '\r')) {
        *p++ = 0;
    }

    if (*p) {
    strncpy((char *)args, (char *)p, 256);
    // Truncate at newline or carriage return
    char *end = args;
    while (*end && *end != '\n' && *end != '\r') {
        end++;
    }
    *end = '\0';
    }

    dprintf(cli->s, "cmd:\t%s\n", cmd);
    dprintf(cli->s, "folder:\t%s\n", folder);
    dprintf(cli->s, "args:\t%s\n", args);
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

    client->s = client_socket;
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

    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)); // Allow reuse of address

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
    Callback x = getCommand((int8 *)"hello");
    printf("%p\n", x);
    x = getCommand((int8 *)"efwefwefwef");
    printf("%p\n", x);

    return 0;
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