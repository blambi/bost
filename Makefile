TARGET = bost
SRCS = bost.c

CC = gcc
LD = gcc

CFLAGS = -std=gnu99 -Wall -pedantic -D_GNU_SOURCE -g #-DDEBUG
LDFLAGS = -g -lm

OBJS = $(SRCS:.c=.o)

all:$(TARGET)

$(TARGET):$(OBJS)
	$(LD) -o $@ $^ $(LDFLAGS)

%.o:%.c
	$(CC) -c $< $(CFLAGS)

clean:
	rm -f $(TARGET) *.o *~ *.bak
