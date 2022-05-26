#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>
#include "new_stack.h"

void* test_push(void* str){
    sleep(0.5);
    char ans[6];
    strcpy(ans,(char*)str);
    ans[5]='\0';
    PUSH(ans);
    printf("%s\n", TOP());
    pthread_exit(NULL);
}

int main(int argc, char const *argv[]){
    pthread_t threadId[26];
    char str[6] = "testX";
    str[5]='\0';
    printf("push and top test:\n");
    //pushing and checking the top everytime in function test_push
    for(int i = 65; i <91 ; i++){
        str[4] = i;
        pthread_create(&threadId[i], NULL, &test_push, (void*)str);
        pthread_join(threadId[i],NULL);
    }
    printf("\npop test:\n");
    //checking if the pop is in the order of the push
    for(int i = 90; i >=65 ; i--){
        char* ans = POP(); 
        str[4] = i;
        ans[5]='\0';
        printf("%s\n",ans);
        assert(!strcmp(str, ans));
    }
}