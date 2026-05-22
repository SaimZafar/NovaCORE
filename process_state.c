// process_state.c — Process State Simulation
// States: NEW → READY → RUNNING → WAITING → TERMINATED
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "process_state.h"

#define MAX_PROC     6
#define STATE_NEW        0
#define STATE_READY      1
#define STATE_RUNNING    2
#define STATE_WAITING    3
#define STATE_TERMINATED 4

static const char *state_bars[] = {
    "[  NEW  ]",
    "[ READY ]",
    "[RUNNING]",
    "[WAITING]",
    "[ DONE  ]"
};

typedef struct {
    int   pid;
    char  name[12];
    int   state;
    int   burst_remaining;
    int   io_wait;         // cycles to wait in WAITING state
} PCB;                     // Process Control Block

static PCB procs[MAX_PROC];
static int n;

// ── Display ───────────────────────────────────────────────────────────────────

static void print_state_table(int cycle) {
    printf("\n  ── Cycle %-3d ─────────────────────────────────────\n", cycle);
    printf("  %-6s %-10s %-10s %-6s\n", "PID", "Name", "State", "Burst Left");
    printf("  ────────────────────────────────────────────────\n");
    for (int i = 0; i < n; i++) {
        printf("  %-6d %-10s %-10s %-6d\n",
               procs[i].pid,
               procs[i].name,
               state_bars[procs[i].state],
               procs[i].burst_remaining);
    }
    printf("  ────────────────────────────────────────────────\n");
}

static void print_state_diagram() {
    printf("\n  Process State Diagram:\n\n");
    printf("   ┌─────┐    admit    ┌───────┐  dispatch  ┌─────────┐\n");
    printf("   │ NEW │ ─────────► │ READY │ ─────────► │ RUNNING │\n");
    printf("   └─────┘            └───────┘            └─────────┘\n");
    printf("                          ▲                  │       │\n");
    printf("                          │    I/O complete  │       │\n");
    printf("                          │                  ▼       │\n");
    printf("                          │              ┌─────────┐ │\n");
    printf("                          └───────────── │ WAITING │ │\n");
    printf("                                         └─────────┘ │\n");
    printf("                                                      │ exit\n");
    printf("                                                      ▼\n");
    printf("                                              ┌────────────┐\n");
    printf("                                              │ TERMINATED │\n");
    printf("                                              └────────────┘\n\n");
}

// ── Simulation ────────────────────────────────────────────────────────────────

static void simulate() {
    int cycle = 0;
    int all_done = 0;

    // All start as NEW
    for (int i = 0; i < n; i++)
        procs[i].state = STATE_NEW;

    printf("\n  Starting simulation...\n");
    printf("  (Each cycle = 1 time unit)\n");

    while (!all_done) {
        // NEW → READY (admit all new processes)
        for (int i = 0; i < n; i++)
            if (procs[i].state == STATE_NEW)
                procs[i].state = STATE_READY;

        // Find one READY process → RUNNING (simple round: first ready)
        int running = -1;
        for (int i = 0; i < n; i++)
            if (procs[i].state == STATE_RUNNING) { running = i; break; }

        if (running == -1) {
            // No process running, dispatch first READY one
            for (int i = 0; i < n; i++)
                if (procs[i].state == STATE_READY) {
                    procs[i].state = STATE_RUNNING;
                    running = i;
                    break;
                }
        }

        // RUNNING process executes one unit
        if (running != -1) {
            procs[running].burst_remaining--;

            // Simulate I/O wait: every 3 burst units, go to WAITING
            if (procs[running].burst_remaining > 0 &&
                procs[running].burst_remaining % 3 == 0) {
                procs[running].state    = STATE_WAITING;
                procs[running].io_wait  = 2; // wait 2 cycles
            } else if (procs[running].burst_remaining <= 0) {
                procs[running].state            = STATE_TERMINATED;
                procs[running].burst_remaining  = 0;
            }
        }

        // WAITING → READY after io_wait cycles
        for (int i = 0; i < n; i++) {
            if (procs[i].state == STATE_WAITING) {
                procs[i].io_wait--;
                if (procs[i].io_wait <= 0)
                    procs[i].state = STATE_READY;
            }
        }

        print_state_table(cycle);
        usleep(400000); // 0.4s pause so user can watch

        // Check if all terminated
        all_done = 1;
        for (int i = 0; i < n; i++)
            if (procs[i].state != STATE_TERMINATED) { all_done = 0; break; }

        cycle++;

        // Safety cap
        if (cycle > 60) {
            printf("\n  [Simulation capped at 60 cycles]\n");
            break;
        }
    }

    printf("\n  All processes terminated. Simulation complete.\n");
    printf("  Total cycles: %d\n\n", cycle);
}

// ── Menu ──────────────────────────────────────────────────────────────────────

void process_state_menu() {
    printf("\n  ╔══════════════════════════════╗\n");
    printf("  ║   PROCESS STATE SIMULATION   ║\n");
    printf("  ╚══════════════════════════════╝\n");

    print_state_diagram();

    printf("  Enter number of processes (max %d): ", MAX_PROC);
    scanf("%d", &n);
    if (n < 1 || n > MAX_PROC) { printf("  Invalid.\n"); return; }

    for (int i = 0; i < n; i++) {
        procs[i].pid = i + 1;
        snprintf(procs[i].name, sizeof(procs[i].name), "P%d", i + 1);
        printf("  P%d Burst Time: ", i + 1);
        scanf("%d", &procs[i].burst_remaining);
        if (procs[i].burst_remaining < 1) procs[i].burst_remaining = 1;
        procs[i].io_wait = 0;
    }

    simulate();
}
