#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_ITERATIONS 100

int exit_flag = 0;
int even_count = 0;
int odd_count = 0;
int i = 0;
pthread_mutex_t mutex;
char arr[NUM_ITERATIONS];

void* even(void* arg) {
    while (1) {
        pthread_mutex_lock(&mutex);

        if (i >= NUM_ITERATIONS) { // Kiểm tra giới hạn
            pthread_mutex_unlock(&mutex);
            return NULL;
        }

        if (arr[i] % 2 == 0) {
            even_count++;
        }
        i++;

        pthread_mutex_unlock(&mutex);
        usleep(100); // Giảm CPU usage
    }
    return NULL;
}

void* odd(void* arg) {
    while (1) {
        pthread_mutex_lock(&mutex);

        if (i >= NUM_ITERATIONS) { // Kiểm tra giới hạn
            pthread_mutex_unlock(&mutex);
            return NULL;
        }

        if (arr[i] % 2 != 0) {
            odd_count++;
        }
        i++;

        pthread_mutex_unlock(&mutex);
        usleep(100);
    }
    return NULL;
}

int main() {
    pthread_t even_thread, odd_thread;

    for (int i = 0; i < NUM_ITERATIONS; i++) {
        arr[i] = i + 1;
    }

    pthread_mutex_init(&mutex, NULL);

    pthread_create(&even_thread, NULL, even, NULL);
    pthread_create(&odd_thread, NULL, odd, NULL);

    pthread_join(even_thread, NULL);
    pthread_join(odd_thread, NULL);

    printf("Số chẵn: %d, Số lẻ: %d\n", even_count, odd_count);

    pthread_mutex_destroy(&mutex);

    return 0;
}
