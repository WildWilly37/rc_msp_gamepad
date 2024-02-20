INCLUDES = -I.

CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99 $(INCLUDES)
LDFLAGS =
TARGET = msp_gamepad
SRCS = msp_gamepad.c  # Add more .c files if you have multiple source files
OBJS = $(SRCS:.c=.o)
DEPS = $(SRCS:.c=.d)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

%.o: %.c %.d
	$(CC) $(CFLAGS) -c $< -o $@

%.d: %.c
	$(CC) $(CFLAGS) -MM -MF $@ -MT $(@:.d=.o) $<

clean:
	$(RM) $(TARGET) $(OBJS) $(DEPS)

run:
	make && ./msp_gamepad

-include $(DEPS)
