#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_ITERATIONS 10

int data;                     // Biến toàn cục lưu dữ liệu
pthread_mutex_t mutex;        // Mutex để bảo vệ vùng nhớ dùng chung
pthread_cond_t cond;          // Condition variable để đồng bộ hóa
int data_available = 0;       // Cờ kiểm tra dữ liệu có sẵn không

void* producer(void* arg) {
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        usleep(rand() % 500000); // Giả lập độ trễ của producer

        pthread_mutex_lock(&mutex);
        data = rand() % 10 + 1;  // Create a random number form 1 to 10
        data_available = 1;
        pthread_cond_signal(&cond);  // Gửi tín hiệu cho consumer
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void* consumer(void* arg) {
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        pthread_mutex_lock(&mutex);
        while (!data_available) { // Skip this loop if consumer run first because data_available = 0
            pthread_cond_wait(&cond, &mutex); // If consumer run after producer, it will wait for signal
        }
        printf("Consumer: %d\n", data);
        data_available = 0;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main() {
    pthread_t producer_thread, consumer_thread;

    srand(time(NULL));
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    pthread_create(&producer_thread, NULL, producer, NULL);
    pthread_create(&consumer_thread, NULL, consumer, NULL);

    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;
}
