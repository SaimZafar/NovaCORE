# NovaCORE — OS Concepts Simulator

> An interactive terminal-based Operating Systems simulator written in C, with a real xv6 kernel extension.

Built for **CSC320 — Operating Systems** at **Bahria University Islamabad**
**Section:** BSIT 5A | **Semester:** Spring 2026

---

##  Group Members

| # | Name | Enrollment No. | Module |
|---|------|----------------|--------|
| 1 | Muhammad Ibrahim | 01-135241-035 | Process Scheduler & Main Menu |
| 2 | Manzer Bibi | 01-135241-026 | Memory Manager |
| 3 | Abdul Wahab | 01-135241-001 | Deadlock & Process State |
| 4 | Ateeq Ur Rehman | 01-135241-011 | IPC Producer-Consumer |
| 5 | Saim Zafar | 01-135241-045 | xv6 Kernel Extension |

---

##  What is NovaCORE?

**NovaCORE** is a hybrid OS project with two parts:

1. **Linux Simulator** — A C program that runs on any Linux machine with 6 interactive modules covering core OS concepts.
2. **xv6 Kernel Extension** — A real system call (`getmeminfo`, syscall #22) added to the MIT xv6 teaching kernel, running inside QEMU.

---

##  Modules

### 1.  Process Scheduler
- **Algorithms:** Round Robin, SJF (Non-Preemptive), Priority (Non-Preemptive)
- Calculates Completion Time, Turnaround Time, and Waiting Time
- Fixed clock initialization bug for correct results when processes arrive after time 0

### 2.  Memory Manager
- **Algorithms:** FIFO, LRU, Optimal Page Replacement
- Frame-by-frame trace showing page hits and misses
- Compare All Three mode on the same reference string

### 3.  IPC Producer-Consumer
- Uses POSIX threads, semaphores, and mutexes
- Bounded buffer with race-condition-free synchronization
- Supports custom number of producers, consumers, and buffer size

### 4.  Process State Simulation
- Simulates all 5 states: NEW → READY → RUNNING → WAITING → TERMINATED
- Cycle-by-cycle output with I/O wait triggers
- Safety cap of 60 cycles to prevent infinite loops

### 5.  Deadlock Detection
- Implements Banker's Algorithm
- Computes Need matrix, runs safety algorithm, finds safe sequence
- Generates text-based Resource Allocation Graph (RAG)

### 6.  xv6 Kernel Extension
- Adds `getmeminfo()` syscall to the xv6 kernel
- Walks kernel free page linked list to report live physical memory stats
- Includes `meminfo` user utility runnable from the xv6 shell

---

##  Technologies Used

- **Language:** C (C99 standard)
- **Compiler:** GCC 13
- **Threading:** POSIX Threads (pthreads)
- **Kernel:** xv6 (MIT teaching OS)
- **Emulator:** QEMU
- **OS:** Ubuntu 24.04
- **Build:** Make

---

##  How to Run

### Linux Simulator

```bash
# Clone the repository
git clone https://github.com/YOUR_USERNAME/NovaCORE.git
cd NovaCORE

# Build and run
chmod +x run.sh
./run.sh
```

Or manually:
```bash
gcc -Wall -std=c99 -o novacore main.c scheduler.c memory.c ipc.c process_state.c deadlock.c -lpthread
./novacore
```

### xv6 Kernel Extension

```bash
# Setup, patch, and build xv6 (requires QEMU installed)
chmod +x setup_xv6.sh
./setup_xv6.sh

# Boot xv6 in QEMU
cd xv6-public
make qemu-nox

# Inside xv6 shell, run:
meminfo
```

---

##  Project Structure

```
NovaCORE/
├── main.c              # Entry point and main menu
├── scheduler.c         # Round Robin, SJF, Priority scheduling
├── memory.c            # FIFO, LRU, Optimal page replacement
├── ipc.c               # Producer-Consumer with semaphores
├── process_state.c     # Five-state process simulation
├── deadlock.c          # Banker's Algorithm
├── novacore.h          # Shared header file
├── Makefile            # Build configuration
├── run.sh              # One-step build and run script
├── setup_xv6.sh        # xv6 clone, patch, and build script
└── meminfo.c           # xv6 user utility
```

---

##  Sample Output

### Process Scheduler (Round Robin, Quantum=2)
```
PID   AT   BT   CT   TAT   WT
P1     2    3    9     7    4
P2     3    4   11     8    4
P3     4    2    8     4    2

Average TAT : 6.33
Average WT  : 3.33
```

### xv6 meminfo Output
```
=== NovaCORE: Memory Info (xv6 Kernel) ===
  Page Size   : 4 KB
  Total Pages : 57344 (229376 KB)
  Used Pages  : 554   (2216 KB)
  Free Pages  : 56790 (227160 KB)
  Usage       : 0%
===========================================
```

---

##  Requirements

- GCC 13 or later
- Ubuntu 22.04 / 24.04 (or any Linux distro)
- QEMU (for xv6 only): `sudo apt install qemu-system-x86`
- pthreads (included with GCC on Linux)

---

##  License

This project was submitted as an academic project for CSC320 at Bahria University Islamabad, Spring 2026. For educational use only.

---

*NovaCORE — CPU · OS · Resources · Execution | Process | Memory | Synchronization*
