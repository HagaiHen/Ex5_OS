//
// Based on: https://www.scaler.com/topics/c/stack-using-linked-list-in-c/
//


using namespace std;

#define PORT "3490"  // the port users will be connecting to

#define BACKLOG 10   // how many pending connections queue will hold

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>

#include<sys/ipc.h>
#include<sys/shm.h>
#include <fcntl.h>

#define MAXDATASIZE 1024 // max number of bytes we can get at once
#define PAGE_SIZE 4096 

int fd;
struct flock lock;

// Structure to create a node with data and next pointer
struct Node {
    char data[MAXDATASIZE];
    struct Node *next;
};


Node* top = (Node*) mmap (NULL, sizeof(Node) * 10000, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
int* addr = (int*) mmap (NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);


void splitCommand(char *str, char **splittedWord) {
    for (int i = 0; i < MAXDATASIZE; i++) {
        splittedWord[i] = strsep(&str, " ");
        if (splittedWord[i] == NULL) {
            break;
        }
    }
}

// Push() operation on a  stack
void PUSH(char data[MAXDATASIZE]) {
    fcntl(fd, F_SETLKW, &lock);

    struct Node *newNode;
    newNode = (top + (*addr));
    strcpy(newNode->data, data);
    if (top == NULL) {
        newNode->next = NULL;
    } else {
        newNode->next = top; // Make the node as top
    }
    printf("Node is Inserted\n");
    (*addr)++;

    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLKW, &lock);
}

char* POP() {

    fcntl(fd, F_SETLKW, &lock);

    if (*addr > 0) {
        (*addr)--;
        return (top+(*addr))->data;
    }

    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLKW, &lock);
    return NULL;
}

void display() {

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
    
    fcntl(fd, F_SETLKW, &lock);

    // Display the elements of the stack
    if (top == NULL) {
        printf("\nStack Underflow\n");
    } else {
        lock.l_type = F_UNLCK;
        fcntl(fd, F_SETLKW, &lock);
        return (top+(*addr)-1)->data;
    }

    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLKW, &lock);
    return NULL;
}
