//
// Based on: https://www.scaler.com/topics/c/stack-using-linked-list-in-c/
//


#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define MAXDATASIZE 1024 // max number of bytes we can get at once
pthread_mutex_t lock;


// Structure to create a node with data and next pointer
struct Node {
    char* data;
    struct Node *next;
};
Node* top = NULL;


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
    pthread_mutex_lock(&lock);
    struct Node *newNode;
    newNode = (struct Node *)malloc(sizeof(struct Node));
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
    printf("Node is Inserted\n\n");
}

char* POP() {
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
    pthread_mutex_lock(&lock);
    // Display the elements of the stack
    if (top == NULL) {
        printf("\nStack Underflow\n");
    } else {
        //printf("The stack is \n");
        struct Node *temp = top;
        //printf("OUTPUT: %s\n", temp->data);
        pthread_mutex_unlock(&lock);
        return top->data;
    }
    pthread_mutex_unlock(&lock);
    return NULL;
}

//int main() {
//    PUSH("hey");
//    PUSH("hello");
//    display();
//    POP();
//    display();
//    TOP();
//}



//#endif //EX4_OS_NEW_STACK_H
