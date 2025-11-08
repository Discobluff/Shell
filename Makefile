CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = main

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
       src/cmd/myexe.c

OBJS = $(SRCS:.c=.o)


all: $(TARGET)

$(TARGET): $(OBJS)
	@$(CC) $(CFLAGS) -o $@ $^


%.o: %.c
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@rm -f $(OBJS) $(TARGET)

run: rebuild 
	@./$(TARGET)

rebuild: clean all

.PHONY: all clean rebuild run