#FILES = button.hex blink.hex stop.hex

CC = g++
CFLAGS = -O3 --std=c++11 -I\koodi\lib\mingw-std-threads -Wall

all: test.exe testpre.exe jokeri.exe jokgen.exe

clean:
	$(RM) *.o *.exe

# testpre.exe: testpre.o single.o util.o genhand.o
	# $(CC) $(CFLAGS) $^ -o $@

#test.exe: test.o util.o genhand.o
#	$(CC) $(CFLAGS) $^ -o $@
#
#jokeri.exe: jokeri.o util.o
#	$(CC) $(CFLAGS) $^ -o $@
#
#jokgen.exe: jokgen.o util.o single.o
#	$(CC) $(CFLAGS) $^ -o $@

%.exe: %.o util.o genhand.o single.o
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.cc
	$(CC) $(CFLAGS) -c $< -o $@
