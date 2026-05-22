#ifndef SCHEDULER_H
#define SCHEDULER_H

#define MAX_PROCESSES 10

typedef struct {
    int pid;
    char name[16];
    int arrival_time;
    int burst_time;
    int priority;
    int remaining_time;
    int completion_time;
    int waiting_time;
    int turnaround_time;
    int started;
} Process;

void scheduler_menu();

#endif
