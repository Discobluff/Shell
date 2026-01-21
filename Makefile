CC = gcc
CFLAGS = -Wall -Wextra
TARGET = main

MAKEFLAGS = --no-print-directory

SRCS = src/main.c \
       src/parser/parser.c \
       src/runner.c \
       src/cmd/cmd.c \
       src/cmd/generic.c \
       src/cmd/myps.c \
       src/cmd/myenv.c \
       src/cmd/mypstree.c \
       src/cmd/mynetstat.c \
       src/cmd/myarp.c \
       src/cmd/myexe.c \
       src/cmd/job.c \
       src/cmd/myinfo.c \
       src/cmd/mymaps.c \
       src/cmd/mydump.c \
       src/cmd/mylof.c \
       src/cmd/cd.c \
       src/cmd/fin.c

OBJS = $(SRCS:.c=.o)


build: $(TARGET)

$(TARGET): $(OBJS)
	@$(CC) $(CFLAGS) -o $@ $^ -lreadline


%.o: %.c
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

farce: build
	@./$(TARGET) --farce

clean:
	@rm -f $(OBJS) $(TARGET) demo/demo1 demo/demo2

run: build 
	@./$(TARGET)

test: rebuild
	@./$(TARGET) --debug

test_unit: tests/test_job.c src/cmd/job.c
	@$(CC) $(CFLAGS) -o tests/runner_unit $^
	@./tests/runner_unit
	@rm tests/runner_unit

test_integration: build
	@python3 tests/test_integration.py

rebuild: clean build

demo%: demo/demo%.c
	@$(CC) $(CFLAGS) -Wno-unused-result -o demo/$@ $<

.PHONY: build clean rebuild run test