/*
** server.c -- a stream socket server demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/signal.h>
#include <features.h>
#include <sys/msg.h>
#include <pthread.h>
//#include "main.cpp"
#include <stddef.h>
#include "new_stack.h"

#define PORT "3490"  // the port users will be connecting to

#define BACKLOG 10   // how many pending connections queue will hold

//struct Stack *stack = createStack(1000000);

static void *handle_client(void *new_fd) {
    int* ptr = (int*) new_fd;
    int sock = *ptr;
    char str[MAXDATASIZE];
    char get[MAXDATASIZE];
    char *splitted[MAXDATASIZE];
    //struct Stack *stack = createStack(100);

    while (1) {
        recv(sock,str,MAXDATASIZE,0);
        //printf("Enter Command: ");
        //fgets(str, MAX_CHARS, stdin);
        splitCommand(str, splitted);
        if (!strcmp(splitted[0], "PUSH")) {
            int size = strlen(splitted[1]);
            splitted[1][size - 1] = 0;
            //sleep(1);
            //for (int i = 0; i < 500000; i++) {
                //printf("%d ", i);
                PUSH(splitted[1]);
            //}
        }

        if (!strcmp(splitted[0], "POP\n")) { //TODO: remove \n
            //char str[MAX_CHARS];
            if(POP() != NULL) {
                printf("popped from stack\n");
            }
        }

        if (!strcmp(splitted[0], "TOP\n")) { //TODO: remove \n
            sleep(1);
            send(sock, TOP(), MAXDATASIZE, 0);
        }
        if (!(strcmp(str, "q\n"))) {
            break;
        }

    }
    //clear(stack);
    //free(stack);

    return NULL;
}

void sigchld_handler(int s) {
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while (waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *) sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *) sa)->sin6_addr);
}

int main(void) {
    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes = 1;
    char s[INET6_ADDRSTRLEN];
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                       sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL) {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    printf("server: waiting for connections...\n");

    while (1) {  // main accept() loop
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *) &their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }
        inet_ntop(their_addr.ss_family,
                  get_in_addr((struct sockaddr *) &their_addr),
                  s, sizeof s);
        printf("server: got connection from %s\n", s);

        pthread_t thread_id;
        int *ptr = &new_fd;
        if (pthread_create(&thread_id, NULL, handle_client,  (void*) ptr) != 0) {
            perror("start thread");
            close(new_fd);
        }

    }
    pthread_exit(NULL);
    return 0;
}
