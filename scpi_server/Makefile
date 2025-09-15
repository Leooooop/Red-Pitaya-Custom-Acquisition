# Makefile — build simple SCPI server (5010)

CC      := gcc
CFLAGS  := -std=gnu11 -O2 -Wall
LDFLAGS := 
TARGET  := scpi_server
SRCS    := scpi_main.c scpi_commands.c rp.c
OBJS    := $(SRCS:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(TARGET) $(OBJS)
