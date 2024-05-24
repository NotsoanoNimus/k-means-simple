.PHONY: default multi clean

CC = gcc
CFLAGS = -Wall -O3

SRCS = main.c kmeans.c
OBJS = main.o kmeans.o

SRCS_MULTI = multidimensional.c kmeans.c
OBJS_MULTI = multidimensional.o kmeans.o

TARGET = kmeans
TARGET_MULTI = multidimensional


default:
	$(MAKE) clean
	$(MAKE) $(TARGET)

multi:
	$(MAKE) clean
	$(MAKE) $(TARGET_MULTI)

clean:
	-rm -f $(OBJS) $(TARGET)
	-rm -f $(OBJS_MULTI) $(TARGET_MULTI)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ -lm

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ -lm

$(TARGET_MULTI): $(OBJS_MULTI) 
	$(CC) $(CFLAGS) $^ -o $@ -lm

