CC      = gcc
CFLAGS  = -Wall -std=c99
LIBS    = -lpthread
TARGET  = novacore
SRCS    = main.c scheduler.c memory.c ipc.c process_state.c deadlock.c

all:
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS) $(LIBS)
	@echo "NovaCORE simulator built. Run: ./novacore"

clean:
	rm -f $(TARGET)
