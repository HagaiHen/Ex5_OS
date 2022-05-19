/*
** server.c -- a stream socket server demo
*/

#include <stdio.h>
#include <sys/mman.h>
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
// #include "new_stack.h"

#define PORT "3490"  // the port users will be connecting to

#define BACKLOG 10   // how many pending connections queue will hold

//struct Stack *stack = createStack(1000000);

//Node* shared = get_top();

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>

#include<sys/ipc.h>
#include<sys/shm.h>

#define MAXDATASIZE 1024 // max number of bytes we can get at once
#define PAGE_SIZE 4096 
#define SHM_KEY 0x1234

pthread_mutex_t lock;


// Structure to create a node with data and next pointer
struct Node {
    char* data;
    struct Node *next;
};



void init(Node * top) {
    int shmid;
    shmid = shmget(SHM_KEY, PAGE_SIZE, 0644|IPC_CREAT);
    if (shmid == -1) {
        perror("Shared memory");
        return;
    }
    top = (Node*) shmat(shmid, NULL, 0);
    if (top == (void *) -1) {
        perror("Shared memory attach");
        return;
    }
}



Node* top = NULL;

Node* get_top() {
    
    return top;
}



void splitCommand(char *str, char **splittedWord) {
    for (int i = 0; i < MAXDATASIZE; i++) {
        splittedWord[i] = strsep(&str, " ");

        if (splittedWord[i] == NULL) {
            break;
        }
    }
}

// Push() operation on a  stack
void PUSH(char* data) {

    // init(top);

    pthread_mutex_lock(&lock);
    struct Node *newNode;
    newNode = (struct Node *) malloc(sizeof(struct Node));
    // newNode = top++;
    char * cpy = (char*) malloc (sizeof(char)* (strlen(data)+1));
    strcpy(cpy, data);
    newNode->data = cpy; // assign value to the node
    if (top == NULL) {
        newNode->next = NULL;
    } else {
        newNode->next = top; // Make the node as top
    }
    top = newNode; // top always points to the newly created node
    pthread_mutex_unlock(&lock);
    printf("Node is Inserted\n%p\n", newNode);
}

char* POP() {

    // int shmid;
    // shmid = shmget(SHM_KEY, PAGE_SIZE, 0644|IPC_CREAT);
    // if (shmid == -1) {
    //     perror("Shared memory");
    //     return "error";
    // }
    // Node * top = (Node*) shmat(shmid, NULL, 0);
    // if (top == (void *) -1) {
    //     perror("Shared memory attach");
    //     return "error";
    // }

    pthread_mutex_lock(&lock);
    if (top == NULL) {
        printf("\nStack Underflow\n");
        pthread_mutex_unlock(&lock);
        return NULL;
    } else {
        struct Node *temp = top;
        char * temp_data = top->data;
        top = top->next;
        free(temp);
        pthread_mutex_unlock(&lock);
        return temp_data;
    }
    //return -1;
}

void display() {
    // int shmid;
    // shmid = shmget(SHM_KEY, PAGE_SIZE, 0644|IPC_CREAT);
    // if (shmid == -1) {
    //     perror("Shared memory");
    //     return;
    // }
    // Node * top = (Node*) shmat(shmid, NULL, 0);
    // if (top == (void *) -1) {
    //     perror("Shared memory attach");
    //     return;
    // }

    // Display the elements of the stack
    if (top == NULL) {
        printf("\nStack Underflow\n");
    } else {
        printf("The stack is \n");
        struct Node *temp = top;
        while (temp->next != NULL) {
            printf("%s--->", temp->data);
            temp = temp->next;
        }
        printf("%s--->NULL\n\n", temp->data);
    }
}

char* TOP() {
    // int shmid;
    // shmid = shmget(SHM_KEY, PAGE_SIZE, 0644|IPC_CREAT);
    // if (shmid == -1) {
    //     perror("Shared memory");
    //     return "error";
    // }
    // Node * top = (Node*) shmat(shmid, NULL, 0);
    // if (top == (void *) -1) {
    //     perror("Shared memory attach");
    //     return "error";
    // }

    pthread_mutex_lock(&lock);
    // Display the elements of the stack
    if (top == NULL) {
        printf("\nStack Underflow\n");
    } else {
        //printf("The stack is \n");
        //struct Node *temp = top;
        //printf("OUTPUT: %s\n", temp->data);
        pthread_mutex_unlock(&lock);
        return top->data;
    }
    pthread_mutex_unlock(&lock);
    return NULL;
}


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
    top = (Node*) mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
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

    // printf("shared %p\n", top);

    printf("server: waiting for connections...\n");

    while(1) {  // main accept() loop
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family,
            get_in_addr((struct sockaddr *)&their_addr),
            s, sizeof s);
        printf("server: got connection from %s\n", s);
        
        if (!fork()) { // this is the child process
            // Node * shared = get_top();
            handle_client(&new_fd);
            // printf("%p", (void*) shared); 
            // printf("hey");           
        }
        // handle_client(&new_fd);
        // Node * shared = get_top();
        // shared = (Node*) mmap(NULL, 1024, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
        // printf("hey");
        printf("shared %p\n", top);
        // close(new_fd);  // parent doesn't need this
    }

    return 0;
}
