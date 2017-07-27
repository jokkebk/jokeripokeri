#FILES = button.hex blink.hex stop.hex

CC = g++
CFLAGS = -O3 --std=c++11

all: test.exe jokeri.exe jokgen.exe

test.exe: test.o util.o genhand.o
	$(CC) $(CFLAGS) $^ -o $@

jokeri.exe: jokeri.o util.o
	$(CC) $(CFLAGS) $^ -o $@

jokgen.exe: jokgen.o util.o
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.cc
	$(CC) $(CFLAGS) -c $< -o $@
