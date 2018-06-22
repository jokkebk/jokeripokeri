CC = g++
CFLAGS = -O3 --std=c++11 -I\koodi\lib\mingw-std-threads -Wall

all: test.exe jokeri.exe jokgen.exe

clean:
	$(RM) *.o *.exe

jokgen.exe: jokgen.o util.o fastone.o
	$(CC) $(CFLAGS) $^ -o $@

test.exe: test.o util.o genhand.o
	$(CC) $(CFLAGS) $^ -o $@

%.exe: %.o util.o
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.cc
	$(CC) $(CFLAGS) -c $< -o $@
