CC = clang
CFLAGS = -std=c99 -Wall -Wextra -pedantic -O0 -ggdb
LDFLAGS = 

run: main
	@./main

main: main.o
	@$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -f main *.o