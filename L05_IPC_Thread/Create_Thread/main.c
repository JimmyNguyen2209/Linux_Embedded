#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

void *thread_function(void *arg) 
{
    int thread_id = (int)(long)arg;
    printf("Thread %d: Hello from thread\n", thread_id);
    return NULL;
}

int main() 
{
    pthread_t thread_1;
    pthread_t thread_2;
    
    int id1 = 1;
    int id2 = 2;

    pthread_create(&thread_1, NULL, thread_function, (void*)1);
    pthread_create(&thread_2, NULL, thread_function, (void*)2);

    pthread_join(thread_1, NULL);
    pthread_join(thread_2, NULL);
    
    return 0;
}