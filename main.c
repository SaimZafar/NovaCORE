// main.c — NovaCORE Entry Point
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scheduler.h"
#include "memory.h"
#include "ipc.h"
#include "process_state.h"
#include "deadlock.h"

void print_banner() {
    printf("\n");
    printf("  ╔══════════════════════════════════════════╗\n");
    printf("  ║                NovaCORE                  ║\n");
    printf("  ║    CPU · OS · Resources · Execution      ║\n");
    printf("  ║   Process | Memory | Synchronization     ║\n");
    printf("  ╚══════════════════════════════════════════╝\n");
    printf("\n");
}

void print_menu() {
    printf("  ┌─────────────────────────────────────┐\n");
    printf("  │          NovaCORE  MENU             │\n");
    printf("  ├─────────────────────────────────────┤\n");
    printf("  │  1. Process Scheduler               │\n");
    printf("  │  2. Memory Manager                  │\n");
    printf("  │  3. IPC  – Producer Consumer        │\n");
    printf("  │  4. Process State Simulation        │\n");
    printf("  │  5. Deadlock Detection              │\n");
    printf("  │  6. System Info                     │\n");
    printf("  │  0. Exit                            │\n");
    printf("  └─────────────────────────────────────┘\n");
    printf("  Choose: ");
}

void system_info() {
    printf("\n  ── NovaCORE System Info ─────────────────\n");
    printf("  OS Simulator     : NovaCORE v1.0\n");
    printf("  Scheduler Algos  : Round Robin, SJF, Priority\n");
    printf("  Page Replacement : FIFO, LRU, Optimal\n");
    printf("  IPC Model        : Producer-Consumer (semaphore + mutex)\n");
    printf("  Process States   : New->Ready->Running->Waiting->Terminated\n");
    printf("  Deadlock         : Banker's Algorithm + RAG\n");
    printf("  Built with       : C (GCC), pthreads, POSIX semaphores\n");
    printf("  Platform         : Ubuntu Linux\n");
    printf("  ─────────────────────────────────────────\n\n");
}

int main() {
    int choice;
    print_banner();

    while (1) {
        print_menu();
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            printf("  Invalid input.\n\n");
            continue;
        }
        while (getchar() != '\n');

        switch (choice) {
            case 1: scheduler_menu();      break;
            case 2: memory_menu();         break;
            case 3: ipc_menu();            break;
            case 4: process_state_menu();  break;
            case 5: deadlock_menu();       break;
            case 6: system_info();         break;
            case 0:
                printf("\n  Shutting down NovaCORE. Goodbye.\n\n");
                return 0;
            default:
                printf("  Invalid option. Try again.\n\n");
        }
    }
    return 0;
}
