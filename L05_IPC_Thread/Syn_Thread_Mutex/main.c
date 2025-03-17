    #include <stdio.h>
    #include <pthread.h>
    #include <unistd.h>
    #include <stdlib.h>

    int count = 0;
    pthread_mutex_t key;

    void *thread_function(void *arg) 
    {
        while (1)
        {
            pthread_mutex_lock(&key);
            if (count < 1000000)
            {
                count++;
                printf("Count at: %d\n", count);
                pthread_mutex_unlock(&key);
            }
            else
            {
                pthread_mutex_unlock(&key);
                break;
            }
        }
        return NULL;
    }

    int main() 
    {
        pthread_t thread_1;
        pthread_t thread_2;
        pthread_t thread_3;

        pthread_mutex_init(&key, NULL);

        pthread_create(&thread_1, NULL, thread_function, NULL);
        pthread_create(&thread_2, NULL, thread_function, NULL);
        pthread_create(&thread_3, NULL, thread_function, NULL);

        pthread_join(thread_1, NULL);
        pthread_join(thread_2, NULL);
        pthread_join(thread_3, NULL);
        
        return 0;
    }