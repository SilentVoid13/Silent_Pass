CC=gcc
CFLAGS = -Wall -Wextra -Wno-pointer-sign#-Wno-discarded-qualifiers
CFLAGS_SECRET = $(shell pkg-config --cflags --libs libsecret-1)
LDFLAGS=-Wl,-rpath,/usr/lib/nss -Wl,-rpath,/usr/lib/nspr
DEPS = main.h firefox.h chrome.h lib/cJSON.h

all: silentpass

silentpass: firefox.o main.o chrome.o cJSON.o 
	$(CC) $(CFLAGS) $(CFLAGS_SECRET) $(LDFLAGS) -lssl -lcrypto -l nspr4 -l nss3 -l iniparser -l sqlite3 -o Silent_Pass firefox.o main.o cJSON.o chrome.o
firefox.o: firefox.c firefox.h lib/cJSON.h
	$(CC) $(CFLAGS) $(LDFLAGS) -I /usr/include/nss -I /usr/include/nspr -c firefox.c
main.o: main.c main.h firefox.h 
	$(CC) $(CFLAGS) -I /usr/include/nss -I /usr/include/nspr -c main.c
cJSON.o: lib/cJSON.c lib/cJSON.h
	$(CC) $(CFLAGS) -c lib/cJSON.c
chrome.o: chrome.c chrome.h
	$(CC) $(CFLAGS) $(CFLAGS_SECRET) -c chrome.c
clean:
	rm *.o
