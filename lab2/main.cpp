#include "stdlib.h"
#include "pthread.h"
#include <iostream>
#include <atomic>
#include <time.h>
#include <chrono>
#include <unistd.h>
#define COUNT 1

struct input{
    int out;
    int op_count;
};

pthread_mutex_t m;
input s;

void *check(void* ptr){
    input* a = (input*)ptr;
    int temp = 0;
    drand48_data buf;
    srand48_r(time(0), &buf);
    for (int k = 0; k < a->op_count; k++)
    {
        long card1;
        lrand48_r(&buf, &card1);
        card1 = card1 % 52;
        long card2;
        lrand48_r(&buf, &card2);
        card2 = card2 % 52;

        if (card1 % 14 == card2 % 14){
            temp++;
        }
    } 
    pthread_mutex_lock(&m);
    a->out += temp;
    pthread_mutex_unlock(&m);
    return NULL;
}


int main(int argsc, char** args){
    int count = atoi(args[1]);
    int count_threads = atoi(args[2]);
    srand(time(NULL));
    pthread_t* t = (pthread_t*)malloc(count_threads * sizeof(pthread_t));
    if (t == NULL){
        exit(EXIT_FAILURE);
    }
   
    s.out = 0;
    s.op_count = count / count_threads;
    
    auto start_time = std::chrono::steady_clock::now();  
    int status;
    status = pthread_mutex_init(&m, NULL);
    if (status != 0){
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < count_threads; i++){
        status = pthread_create(&t[i], NULL, check, &s);
        if (status != 0){
            exit(EXIT_FAILURE);
        }
    }
    
    for (int i = 0; i < count_threads; i++){
        status = pthread_join(t[i], NULL);     
        if (status != 0){
            exit(EXIT_FAILURE);
        }
    }

    pthread_mutex_destroy(&m);
    std::cout << (double)s.out / (double)count << '\n';
    std::chrono::steady_clock::time_point current_time = std::chrono::steady_clock::now();
    std::cout << "Program has been running for " << std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count() << " miliseconds" << std::endl;
    free(t);
    return 0;
}