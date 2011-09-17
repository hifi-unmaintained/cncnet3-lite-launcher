CC=i586-mingw32msvc-gcc
CFLAGS=-pedantic -Wall -Os -s -Wall
WINDRES=i586-mingw32msvc-windres
DLLTOOL=i586-mingw32msvc-dlltool
LIBS=-Wl,--file-alignment,512 -Wl,--gc-sections -lwininet -lcomctl32
REV=$(shell sh -c 'git rev-parse --short @{0}')

all: cncnet

cncnet.rc.o: res/cncnet.rc.in
	sed 's/__REV__/$(REV)/g' res/cncnet.rc.in | $(WINDRES) -o cncnet.rc.o

cncnet: cncnet.rc.o
	$(CC) $(CFLAGS) -mwindows -o cncnet.exe src/main.c src/http.c src/config.c src/register.c cncnet.rc.o $(LIBS)

clean:
	rm -f cncnet.exe cncnet.rc.o
