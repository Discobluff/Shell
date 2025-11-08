CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = main

SRCS = main.c \
       parser/parser.c \
       runner/runner.c \
	   runner/cmd/cmd.c \
       runner/cmd/generic.c \
       runner/cmd/myps.c \
	   runner/cmd/myenv.c \
	   runner/cmd/mypstree.c \
	   runner/cmd/mynetstat.c \
	   runner/cmd/myarp.c \
	   runner/cmd/myexe.c

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