// scheduler.c — Round Robin, SJF (Non-Preemptive), Priority (Non-Preemptive)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "scheduler.h"

// ── Input ────────────────────────────────────────────────────────────────────

static int n;
static Process procs[MAX_PROCESSES];

static void input_processes() {
    printf("\n  Enter number of processes (max %d): ", MAX_PROCESSES);
    scanf("%d", &n);
    if (n < 1 || n > MAX_PROCESSES) { printf("  Invalid count.\n"); n = 0; return; }

    int need_priority;
    printf("  Enter priority for each process? (1=Yes, 0=No): ");
    scanf("%d", &need_priority);

    for (int i = 0; i < n; i++) {
        procs[i].pid = i + 1;
        snprintf(procs[i].name, sizeof(procs[i].name), "P%d", i + 1);
        printf("\n  Process P%d\n", i + 1);
        printf("    Arrival Time : "); scanf("%d", &procs[i].arrival_time);
        printf("    Burst Time   : "); scanf("%d", &procs[i].burst_time);
        if (need_priority) {
            printf("    Priority     : "); scanf("%d", &procs[i].priority);
        } else {
            procs[i].priority = 0;
        }
        procs[i].remaining_time = procs[i].burst_time;
        procs[i].completion_time = 0;
        procs[i].waiting_time = 0;
        procs[i].turnaround_time = 0;
        procs[i].started = 0;
    }
}

// ── Print Results ─────────────────────────────────────────────────────────────

static void print_results(const char *algo) {
    printf("\n  ── Results: %s ─────────────────────────────\n", algo);
    printf("  %-6s %-6s %-8s %-10s %-12s %-10s\n",
           "PID", "AT", "BT", "CT", "TAT", "WT");
    printf("  ──────────────────────────────────────────────────\n");

    float total_tat = 0, total_wt = 0;
    for (int i = 0; i < n; i++) {
        printf("  %-6s %-6d %-8d %-10d %-12d %-10d\n",
               procs[i].name,
               procs[i].arrival_time,
               procs[i].burst_time,
               procs[i].completion_time,
               procs[i].turnaround_time,
               procs[i].waiting_time);
        total_tat += procs[i].turnaround_time;
        total_wt  += procs[i].waiting_time;
    }

    printf("  ──────────────────────────────────────────────────\n");
    printf("  Average Turnaround Time : %.2f\n", total_tat / n);
    printf("  Average Waiting Time    : %.2f\n\n", total_wt / n);
}

// ── Reset remaining_time ──────────────────────────────────────────────────────

static void reset_procs() {
    for (int i = 0; i < n; i++) {
        procs[i].remaining_time  = procs[i].burst_time;
        procs[i].completion_time = 0;
        procs[i].waiting_time    = 0;
        procs[i].turnaround_time = 0;
        procs[i].started         = 0;
    }
}

// ── Round Robin ───────────────────────────────────────────────────────────────

static void round_robin(int quantum) {
    reset_procs();
    int time = 0, completed = 0;
    int queue[MAX_PROCESSES * 100];
    int front = 0, rear = 0;
    int in_queue[MAX_PROCESSES];
    memset(in_queue, 0, sizeof(in_queue));

    // Sort by arrival time
    for (int i = 0; i < n - 1; i++)
        for (int j = i + 1; j < n; j++)
            if (procs[j].arrival_time < procs[i].arrival_time) {
                Process tmp = procs[i]; procs[i] = procs[j]; procs[j] = tmp;
            }

    // Enqueue first process
    queue[rear++] = 0;
    in_queue[0] = 1;

    while (completed < n) {
        if (front == rear) {
            // CPU idle — advance to next arrival
            int next = INT_MAX;
            for (int i = 0; i < n; i++)
                if (!in_queue[i] && procs[i].remaining_time > 0 && procs[i].arrival_time < next)
                    next = procs[i].arrival_time;
            time = next;
            for (int i = 0; i < n; i++)
                if (!in_queue[i] && procs[i].arrival_time <= time && procs[i].remaining_time > 0) {
                    queue[rear++] = i; in_queue[i] = 1;
                }
        }

        int i = queue[front++];
        int run = (procs[i].remaining_time < quantum) ? procs[i].remaining_time : quantum;
        procs[i].remaining_time -= run;
        time += run;

        // Enqueue newly arrived
        for (int j = 0; j < n; j++)
            if (!in_queue[j] && procs[j].arrival_time <= time && procs[j].remaining_time > 0) {
                queue[rear++] = j; in_queue[j] = 1;
            }

        if (procs[i].remaining_time == 0) {
            procs[i].completion_time  = time;
            procs[i].turnaround_time  = time - procs[i].arrival_time;
            procs[i].waiting_time     = procs[i].turnaround_time - procs[i].burst_time;
            completed++;
        } else {
            queue[rear++] = i;
        }
    }
    print_results("Round Robin");
}

// ── SJF Non-Preemptive ────────────────────────────────────────────────────────

static void sjf() {
    reset_procs();
    int time = 0, completed = 0;
    int done[MAX_PROCESSES];
    memset(done, 0, sizeof(done));

    while (completed < n) {
        int idx = -1, min_bt = INT_MAX;
        for (int i = 0; i < n; i++)
            if (!done[i] && procs[i].arrival_time <= time && procs[i].burst_time < min_bt) {
                min_bt = procs[i].burst_time; idx = i;
            }
        if (idx == -1) { time++; continue; }

        time += procs[idx].burst_time;
        procs[idx].completion_time  = time;
        procs[idx].turnaround_time  = time - procs[idx].arrival_time;
        procs[idx].waiting_time     = procs[idx].turnaround_time - procs[idx].burst_time;
        done[idx] = 1;
        completed++;
    }
    print_results("SJF (Non-Preemptive)");
}

// ── Priority Non-Preemptive (lower number = higher priority) ──────────────────

static void priority_scheduling() {
    reset_procs();
    int time = 0, completed = 0;
    int done[MAX_PROCESSES];
    memset(done, 0, sizeof(done));

    while (completed < n) {
        int idx = -1, best = INT_MAX;
        for (int i = 0; i < n; i++)
            if (!done[i] && procs[i].arrival_time <= time && procs[i].priority < best) {
                best = procs[i].priority; idx = i;
            }
        if (idx == -1) { time++; continue; }

        time += procs[idx].burst_time;
        procs[idx].completion_time  = time;
        procs[idx].turnaround_time  = time - procs[idx].arrival_time;
        procs[idx].waiting_time     = procs[idx].turnaround_time - procs[idx].burst_time;
        done[idx] = 1;
        completed++;
    }
    print_results("Priority (Non-Preemptive)");
}

// ── Scheduler Menu ────────────────────────────────────────────────────────────

void scheduler_menu() {
    int choice;
    printf("\n  ╔══════════════════════════════╗\n");
    printf("  ║     PROCESS SCHEDULER        ║\n");
    printf("  ╚══════════════════════════════╝\n");
    printf("  1. Round Robin\n");
    printf("  2. SJF (Non-Preemptive)\n");
    printf("  3. Priority (Non-Preemptive)\n");
    printf("  0. Back\n");
    printf("  Choose: ");
    scanf("%d", &choice);
    if (choice == 0) return;

    input_processes();
    if (n == 0) return;

    switch (choice) {
        case 1: {
            int q;
            printf("\n  Enter Time Quantum: ");
            scanf("%d", &q);
            round_robin(q);
            break;
        }
        case 2: sjf();               break;
        case 3: priority_scheduling(); break;
        default: printf("  Invalid.\n");
    }
}
