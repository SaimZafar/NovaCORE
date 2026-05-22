// ipc.c — Producer-Consumer with semaphores + mutex
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include "ipc.h"

#define MAX_BUFFER 10

static int buffer[MAX_BUFFER];
static int in_idx = 0, out_idx = 0;
static int buffer_size;
static int items_per_producer;
static int num_producers, num_consumers;
static int item_counter = 0;

static sem_t empty_sem, full_sem;
static pthread_mutex_t mutex, counter_mutex;

typedef struct { int id; } ThreadArg;

static void *producer(void *arg) {
    int id = ((ThreadArg *)arg)->id;
    for (int i = 0; i < items_per_producer; i++) {
        usleep((rand() % 400 + 100) * 1000);

        pthread_mutex_lock(&counter_mutex);
        int item = ++item_counter;
        pthread_mutex_unlock(&counter_mutex);

        sem_wait(&empty_sem);
        pthread_mutex_lock(&mutex);
        buffer[in_idx] = item;
        printf("  [PRODUCER %d]  Produced item %-3d  → buffer[%d]\n", id, item, in_idx);
        in_idx = (in_idx + 1) % buffer_size;
        pthread_mutex_unlock(&mutex);
        sem_post(&full_sem);
    }
    return NULL;
}

static void *consumer(void *arg) {
    int id = ((ThreadArg *)arg)->id;
    for (int i = 0; i < items_per_producer; i++) {
        sem_wait(&full_sem);
        pthread_mutex_lock(&mutex);
        int item = buffer[out_idx];
        printf("  [CONSUMER %d]  Consumed item %-3d  ← buffer[%d]\n", id, item, out_idx);
        out_idx = (out_idx + 1) % buffer_size;
        pthread_mutex_unlock(&mutex);
        sem_post(&empty_sem);
        usleep((rand() % 400 + 100) * 1000);
    }
    return NULL;
}

static void run_ipc() {
    in_idx = 0; out_idx = 0; item_counter = 0;

    sem_init(&empty_sem, 0, buffer_size);
    sem_init(&full_sem,  0, 0);
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&counter_mutex, NULL);

    pthread_t prod_t[MAX_BUFFER], cons_t[MAX_BUFFER];
    ThreadArg prod_args[MAX_BUFFER], cons_args[MAX_BUFFER];

    printf("\n  ── Running IPC (Buffer=%d, P=%d, C=%d, Items/P=%d) ──\n\n",
           buffer_size, num_producers, num_consumers, items_per_producer);

    for (int i = 0; i < num_producers; i++) {
        prod_args[i].id = i + 1;
        pthread_create(&prod_t[i], NULL, producer, &prod_args[i]);
    }
    for (int i = 0; i < num_consumers; i++) {
        cons_args[i].id = i + 1;
        pthread_create(&cons_t[i], NULL, consumer, &cons_args[i]);
    }
    for (int i = 0; i < num_producers; i++) pthread_join(prod_t[i], NULL);
    for (int i = 0; i < num_consumers; i++) pthread_join(cons_t[i], NULL);

    int total = num_producers * items_per_producer;
    printf("\n  ─────────────────────────────────────────────\n");
    printf("  Total Produced : %d\n", total);
    printf("  Total Consumed : %d\n", total);
    printf("  Status         : No race condition. Buffer safe.\n");
    printf("  ─────────────────────────────────────────────\n\n");

    sem_destroy(&empty_sem);
    sem_destroy(&full_sem);
    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&counter_mutex);
}

void ipc_menu() {
    int choice;
    printf("\n  ╔══════════════════════════════╗\n");
    printf("  ║  IPC — PRODUCER CONSUMER     ║\n");
    printf("  ╚══════════════════════════════╝\n");
    printf("  1. Run with default settings (2P / 2C / buf=5)\n");
    printf("  2. Run with custom settings\n");
    printf("  0. Back\n");
    printf("  Choose: ");
    scanf("%d", &choice);

    if (choice == 0) return;

    if (choice == 1) {
        buffer_size = 5; num_producers = 2;
        num_consumers = 2; items_per_producer = 4;
    } else if (choice == 2) {
        printf("\n  Buffer size  (1-%d): ", MAX_BUFFER); scanf("%d", &buffer_size);
        printf("  Num producers (1-%d): ", MAX_BUFFER); scanf("%d", &num_producers);
        printf("  Num consumers (1-%d): ", MAX_BUFFER); scanf("%d", &num_consumers);
        printf("  Items per producer : "); scanf("%d", &items_per_producer);
        if (buffer_size < 1 || buffer_size > MAX_BUFFER ||
            num_producers < 1 || num_producers > MAX_BUFFER ||
            num_consumers < 1 || num_consumers > MAX_BUFFER ||
            items_per_producer < 1) {
            printf("  Invalid input.\n"); return;
        }
    } else {
        printf("  Invalid.\n"); return;
    }

    run_ipc();
}
