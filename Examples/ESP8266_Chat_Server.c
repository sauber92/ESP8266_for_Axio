#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#define BUFF_SIZE 1024

int Sockfd;

void CloseServer() {
    close(Sockfd);
    exit(0);
}

main() {
    int newSockfd, cliAddrLen, n;
    struct sockaddr_in  cliAddr, servAddr;
    int option;
    pid_t pid;

    char buffRcv[BUFF_SIZE + 5];
    char buffSnd[BUFF_SIZE + 5];

    signal(SIGINT, CloseServer);

    if ((Sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)  {
        perror("socket");
        exit(1);
    }

    option = 1;
    setsockopt(Sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    bzero((char *)&servAddr, sizeof(servAddr));
    servAddr.sin_family = PF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(4000);

    if (bind(Sockfd, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)  {
        perror("bind");
        exit(1);
    }

    listen(Sockfd, 5);

    printf("TCP Server started.....\n");

    cliAddrLen = sizeof(cliAddr);

    newSockfd = accept(Sockfd, (struct sockaddr *) &cliAddr, &cliAddrLen);
    if (newSockfd < 0)  {
        perror("accept");
        exit(1);
    }

    while (1)  {
        if((pid = fork()) < 0) {
            perror("fork");
            exit(1);
        }

        else if(pid == 0) {
            // child = read
            read(newSockfd, buffRcv, BUFF_SIZE);

            fputs(buffRcv, stdout);
            fputs("\n", stdout);
            sprintf(buffSnd, "%s", buffRcv);
            usleep(100);
        }

        else {
            // parent = write
            fgets(buffSnd, sizeof(buffSnd), stdin);
            if (write(newSockfd, buffSnd, strlen(buffSnd)) < 0) {
                perror("wrtie");
                exit(1);
            }
            usleep(100);
        }
    }

    close(newSockfd);
}
