CC = gcc
CFLAGS += -std=gnu11 -Wall -Werror -g3 -DNDEBUG

bandwidth: bandwidth.o
	$(CC) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
