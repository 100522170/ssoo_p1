#Description of a Makefile

# Executable name
TARGET = test_exec

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror -O2

# Source files
SRCS = src/test.c
OBJS = $(SRCS:.c=.o)

# Default target (compile all)
all: $(TARGET)

# Compile target
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compile objects (fix for src/)
src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile specific binary mannualy
test: 
	$(CC) $(CFLAGS) src/test.c -o test_exec_manual
	

# Clean objects and bins
clean:
	rm -f $(OBJS) $(TARGET) test_exec_manual
