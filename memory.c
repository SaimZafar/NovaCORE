// memory.c — Page Replacement: FIFO, LRU, Optimal
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "memory.h"

static int frames, page_count;
static int pages[MAX_PAGES];

// ── Input ─────────────────────────────────────────────────────────────────────

static int input_memory() {
    printf("\n  Enter number of frames (max %d): ", MAX_FRAMES);
    scanf("%d", &frames);
    if (frames < 1 || frames > MAX_FRAMES) { printf("  Invalid.\n"); return 0; }

    printf("  Enter number of pages in reference string (max %d): ", MAX_PAGES);
    scanf("%d", &page_count);
    if (page_count < 1 || page_count > MAX_PAGES) { printf("  Invalid.\n"); return 0; }

    printf("  Enter page reference string:\n  ");
    for (int i = 0; i < page_count; i++) scanf("%d", &pages[i]);
    return 1;
}

// ── Helper: is page in frame? ─────────────────────────────────────────────────

static int in_frames(int *frame, int size, int page) {
    for (int i = 0; i < size; i++)
        if (frame[i] == page) return 1;
    return 0;
}

// ── Print frame state ─────────────────────────────────────────────────────────

static void print_frame_state(int *frame, int size, int hit) {
    printf("  [ ");
    for (int i = 0; i < size; i++) {
        if (frame[i] == -1) printf(" -");
        else printf("%2d", frame[i]);
        if (i < size - 1) printf(" |");
    }
    printf(" ]  %s\n", hit ? "HIT " : "MISS");
}

// ── FIFO ──────────────────────────────────────────────────────────────────────

static void fifo() {
    int frame[MAX_FRAMES];
    memset(frame, -1, sizeof(frame));
    int pointer = 0, hits = 0, misses = 0;

    printf("\n  ── FIFO ─────────────────────────────────\n");
    printf("  Page  Frames                    Result\n");
    printf("  ─────────────────────────────────────────\n");

    for (int i = 0; i < page_count; i++) {
        printf("  %3d   ", pages[i]);
        if (in_frames(frame, frames, pages[i])) {
            hits++;
            print_frame_state(frame, frames, 1);
        } else {
            frame[pointer] = pages[i];
            pointer = (pointer + 1) % frames;
            misses++;
            print_frame_state(frame, frames, 0);
        }
    }
    printf("  ─────────────────────────────────────────\n");
    printf("  Hits: %d  |  Misses (Faults): %d  |  Hit Rate: %.1f%%\n\n",
           hits, misses, (float)hits / page_count * 100);
}

// ── LRU ───────────────────────────────────────────────────────────────────────

static void lru() {
    int frame[MAX_FRAMES];
    int last_used[MAX_FRAMES];
    memset(frame, -1, sizeof(frame));
    memset(last_used, -1, sizeof(last_used));
    int count = 0, hits = 0, misses = 0;

    printf("\n  ── LRU ──────────────────────────────────\n");
    printf("  Page  Frames                    Result\n");
    printf("  ─────────────────────────────────────────\n");

    for (int i = 0; i < page_count; i++) {
        printf("  %3d   ", pages[i]);
        int found = -1;
        for (int j = 0; j < frames; j++)
            if (frame[j] == pages[i]) { found = j; break; }

        if (found != -1) {
            last_used[found] = count++;
            hits++;
            print_frame_state(frame, frames, 1);
        } else {
            // Find empty slot or LRU slot
            int replace = -1;
            for (int j = 0; j < frames; j++)
                if (frame[j] == -1) { replace = j; break; }

            if (replace == -1) {
                int lru_time = INT_MAX;
                for (int j = 0; j < frames; j++)
                    if (last_used[j] < lru_time) { lru_time = last_used[j]; replace = j; }
            }
            frame[replace] = pages[i];
            last_used[replace] = count++;
            misses++;
            print_frame_state(frame, frames, 0);
        }
    }
    printf("  ─────────────────────────────────────────\n");
    printf("  Hits: %d  |  Misses (Faults): %d  |  Hit Rate: %.1f%%\n\n",
           hits, misses, (float)hits / page_count * 100);
}

// ── Optimal ───────────────────────────────────────────────────────────────────

static void optimal() {
    int frame[MAX_FRAMES];
    memset(frame, -1, sizeof(frame));
    int hits = 0, misses = 0;

    printf("\n  ── Optimal ──────────────────────────────\n");
    printf("  Page  Frames                    Result\n");
    printf("  ─────────────────────────────────────────\n");

    for (int i = 0; i < page_count; i++) {
        printf("  %3d   ", pages[i]);
        if (in_frames(frame, frames, pages[i])) {
            hits++;
            print_frame_state(frame, frames, 1);
            continue;
        }

        // Find empty slot
        int replace = -1;
        for (int j = 0; j < frames; j++)
            if (frame[j] == -1) { replace = j; break; }

        if (replace == -1) {
            // Find page used farthest in future (or never used again)
            int farthest = -1, idx = -1;
            for (int j = 0; j < frames; j++) {
                int next_use = INT_MAX;
                for (int k = i + 1; k < page_count; k++)
                    if (pages[k] == frame[j]) { next_use = k; break; }
                if (next_use > farthest) { farthest = next_use; idx = j; }
            }
            replace = idx;
        }

        frame[replace] = pages[i];
        misses++;
        print_frame_state(frame, frames, 0);
    }
    printf("  ─────────────────────────────────────────\n");
    printf("  Hits: %d  |  Misses (Faults): %d  |  Hit Rate: %.1f%%\n\n",
           hits, misses, (float)hits / page_count * 100);
}

// ── Memory Menu ───────────────────────────────────────────────────────────────

void memory_menu() {
    int choice;
    printf("\n  ╔══════════════════════════════╗\n");
    printf("  ║     MEMORY MANAGER           ║\n");
    printf("  ╚══════════════════════════════╝\n");
    printf("  1. FIFO Page Replacement\n");
    printf("  2. LRU Page Replacement\n");
    printf("  3. Optimal Page Replacement\n");
    printf("  4. Compare All Three\n");
    printf("  0. Back\n");
    printf("  Choose: ");
    scanf("%d", &choice);
    if (choice == 0) return;
    if (!input_memory()) return;

    switch (choice) {
        case 1: fifo();    break;
        case 2: lru();     break;
        case 3: optimal(); break;
        case 4:
            fifo();
            lru();
            optimal();
            break;
        default: printf("  Invalid.\n");
    }
}
