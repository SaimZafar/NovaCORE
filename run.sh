#!/bin/bash
echo "========================================"
echo "     NovaCORE Simulator — Build & Run   "
echo "========================================"
gcc -Wall -std=c99 -o novacore \
    main.c scheduler.c memory.c ipc.c process_state.c deadlock.c \
    -lpthread
if [ $? -ne 0 ]; then echo "Build FAILED."; exit 1; fi
echo "Build successful."
echo ""
./novacore
