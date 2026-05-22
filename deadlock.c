// deadlock.c — Banker's Algorithm (Safety Check) + Resource Allocation Graph
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "deadlock.h"

#define MAX_P 6   // max processes
#define MAX_R 4   // max resource types

static int n_proc, n_res;
static int allocation[MAX_P][MAX_R]; // currently allocated
static int maximum[MAX_P][MAX_R];    // max demand
static int need[MAX_P][MAX_R];       // maximum - allocation
static int available[MAX_R];         // available resources

// ── Compute Need Matrix ───────────────────────────────────────────────────────

static void compute_need() {
    for (int i = 0; i < n_proc; i++)
        for (int j = 0; j < n_res; j++)
            need[i][j] = maximum[i][j] - allocation[i][j];
}

// ── Print Matrices ────────────────────────────────────────────────────────────

static void print_matrices() {
    printf("\n  ── Resource State ───────────────────────────────\n");

    // Available
    printf("  Available : [ ");
    for (int j = 0; j < n_res; j++) printf("R%d:%-2d ", j, available[j]);
    printf("]\n\n");

    // Header
    printf("  %-6s", "Proc");
    printf("  %-*s", n_res * 6, "Allocation");
    printf("  %-*s", n_res * 6, "Maximum");
    printf("  %-*s\n", n_res * 6, "Need");
    printf("  ──────────────────────────────────────────────────\n");

    for (int i = 0; i < n_proc; i++) {
        printf("  P%-5d", i);
        printf("  ");
        for (int j = 0; j < n_res; j++) printf("%-6d", allocation[i][j]);
        printf("  ");
        for (int j = 0; j < n_res; j++) printf("%-6d", maximum[i][j]);
        printf("  ");
        for (int j = 0; j < n_res; j++) printf("%-6d", need[i][j]);
        printf("\n");
    }
    printf("  ──────────────────────────────────────────────────\n\n");
}

// ── Banker's Safety Algorithm ─────────────────────────────────────────────────

static void bankers_algorithm() {
    int work[MAX_R];
    int finish[MAX_P];
    int safe_seq[MAX_P];
    int count = 0;

    for (int j = 0; j < n_res; j++) work[j] = available[j];
    memset(finish, 0, sizeof(finish));

    printf("  ── Banker's Safety Algorithm ─────────────────────\n");
    printf("  Checking safe sequence...\n\n");

    while (count < n_proc) {
        int found = 0;
        for (int i = 0; i < n_proc; i++) {
            if (finish[i]) continue;

            // Check if need[i] <= work
            int can = 1;
            for (int j = 0; j < n_res; j++)
                if (need[i][j] > work[j]) { can = 0; break; }

            if (can) {
                printf("  P%d can proceed  → releasing resources\n", i);
                for (int j = 0; j < n_res; j++)
                    work[j] += allocation[i][j];
                finish[i] = 1;
                safe_seq[count++] = i;
                found = 1;
            }
        }
        if (!found) break; // No process could proceed — deadlock
    }

    printf("\n");
    if (count == n_proc) {
        printf("  ✔  SAFE STATE DETECTED\n");
        printf("  Safe Sequence : ");
        for (int i = 0; i < n_proc; i++) {
            printf("P%d", safe_seq[i]);
            if (i < n_proc - 1) printf(" → ");
        }
        printf("\n");
        printf("  No deadlock will occur.\n");
    } else {
        printf("  ✘  UNSAFE STATE — DEADLOCK DETECTED!\n");
        printf("  Processes that cannot proceed: ");
        for (int i = 0; i < n_proc; i++)
            if (!finish[i]) printf("P%d ", i);
        printf("\n");
        printf("  These processes are in deadlock.\n");
    }
    printf("  ──────────────────────────────────────────────────\n\n");
}

// ── Resource Allocation Graph (text-based) ────────────────────────────────────

static void print_rag() {
    printf("\n  ── Resource Allocation Graph ─────────────────────\n");
    printf("  Format: P→R means P requests R\n");
    printf("          R→P means R is allocated to P\n\n");

    for (int i = 0; i < n_proc; i++) {
        for (int j = 0; j < n_res; j++) {
            if (need[i][j] > 0)
                printf("  P%d ──requests──► R%d  (needs %d)\n", i, j, need[i][j]);
        }
    }
    printf("\n");
    for (int i = 0; i < n_proc; i++) {
        for (int j = 0; j < n_res; j++) {
            if (allocation[i][j] > 0)
                printf("  R%d ──allocated─► P%d  (holds %d)\n", j, i, allocation[i][j]);
        }
    }
    printf("  ──────────────────────────────────────────────────\n\n");
}

// ── Input ─────────────────────────────────────────────────────────────────────

static int input_data() {
    printf("\n  Enter number of processes (max %d): ", MAX_P);
    scanf("%d", &n_proc);
    if (n_proc < 1 || n_proc > MAX_P) { printf("  Invalid.\n"); return 0; }

    printf("  Enter number of resource types (max %d): ", MAX_R);
    scanf("%d", &n_res);
    if (n_res < 1 || n_res > MAX_R) { printf("  Invalid.\n"); return 0; }

    printf("\n  Enter total instances of each resource (Available):\n  ");
    for (int j = 0; j < n_res; j++) {
        printf("R%d: ", j);
        scanf("%d", &available[j]);
    }

    printf("\n  Enter Allocation matrix (processes × resources):\n");
    for (int i = 0; i < n_proc; i++) {
        printf("  P%d allocation [ ", i);
        for (int j = 0; j < n_res; j++) {
            printf("R%d: ", j);
            scanf("%d", &allocation[i][j]);
            available[j] -= allocation[i][j]; // reduce available by allocated
        }
        printf("]\n");
    }

    printf("\n  Enter Maximum demand matrix:\n");
    for (int i = 0; i < n_proc; i++) {
        printf("  P%d maximum    [ ", i);
        for (int j = 0; j < n_res; j++) {
            printf("R%d: ", j);
            scanf("%d", &maximum[i][j]);
        }
        printf("]\n");
    }

    // Validate: allocation must not exceed maximum
    for (int i = 0; i < n_proc; i++)
        for (int j = 0; j < n_res; j++)
            if (allocation[i][j] > maximum[i][j]) {
                printf("  Error: P%d allocation > maximum for R%d.\n", i, j);
                return 0;
            }

    compute_need();
    return 1;
}

// ── Preset Example ────────────────────────────────────────────────────────────

static void load_preset() {
    // Classic Banker's example: 5 processes, 3 resources
    n_proc = 5; n_res = 3;
    int alloc[5][3] = {{0,1,0},{2,0,0},{3,0,2},{2,1,1},{0,0,2}};
    int maxm[5][3]  = {{7,5,3},{3,2,2},{9,0,2},{2,2,2},{4,3,3}};
    int avail[3]    = {3,3,2};

    for (int j = 0; j < n_res; j++) available[j] = avail[j];
    for (int i = 0; i < n_proc; i++)
        for (int j = 0; j < n_res; j++) {
            allocation[i][j] = alloc[i][j];
            maximum[i][j]    = maxm[i][j];
        }
    compute_need();
    printf("\n  Preset loaded: 5 processes, 3 resource types.\n");
}

// ── Menu ──────────────────────────────────────────────────────────────────────

void deadlock_menu() {
    int choice;
    printf("\n  ╔══════════════════════════════╗\n");
    printf("  ║   DEADLOCK DETECTION         ║\n");
    printf("  ║   Banker's Algorithm         ║\n");
    printf("  ╚══════════════════════════════╝\n");
    printf("  1. Enter custom data\n");
    printf("  2. Load preset example\n");
    printf("  0. Back\n");
    printf("  Choose: ");
    scanf("%d", &choice);
    if (choice == 0) return;

    if (choice == 1) {
        if (!input_data()) return;
    } else if (choice == 2) {
        load_preset();
    } else {
        printf("  Invalid.\n"); return;
    }

    print_matrices();
    print_rag();
    bankers_algorithm();
}
