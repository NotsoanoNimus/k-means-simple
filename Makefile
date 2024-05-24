.PHONY: default clean all

CC = gcc
CFLAGS = -Wall -O3

SRCS = main.c kmeans.c
OBJS = $(SRCS:.c=.o)

TARGET = kmeans


default:
	$(MAKE) clean
	$(MAKE) $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ -lm

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ -lm

clean:
	rm -f $(OBJS) $(TARGET)
