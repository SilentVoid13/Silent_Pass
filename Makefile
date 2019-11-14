CC=gcc
WC=i686-w64-mingw32-gcc
CFLAGS = -Wall -Wextra -Wno-pointer-sign -fPIC
CFLAGS_SECRET = $(shell pkg-config --cflags --libs libsecret-1)
LDFLAGS=-Wl,-rpath,/usr/lib/nss -Wl,-rpath,/usr/lib/nspr
DEPS = main.h firefox.h chrome.h lib/cJSON.h
INCLUDE= -I. -Isrc -Ilib -Iincludes -I/usr/include/nss -I/usr/include/nspr 

SRC_FILES := $(wildcard lib/argp/*.c)
OBJ_FILES := $(patsubst lib/argp/%.c,./%.o,$(SRC_FILES))

LINUX_LIBS= -lssl -lcrypto -l nspr4 -l nss3 -l iniparser -ldl 
WINDOWS_LIBS = -lcrypt32 -lws2_32 -lgdi32 

NORMAL_FILES := $(wildcard src/*.c) $(wildcard lib/*.c) $(wildcard src/utilities/*.c)
LINUX_FILES := $(wildcard src/linux/*.c)
WINDOWS_FILES := $(wildcard src/win32/*.c)

OPENSSL_LIBS := $(wildcard lib/*.a)

print-%  : ; @echo $* = $($*)

all: silentpass

win: 
	$(WC) $(CFLAGS) $(INCLUDE) $(NORMAL_FILES) $(WINDOWS_FILES) $(OPENSSL_LIBS) $(WINDOWS_LIBS) -o Silent_Pass

silentpass:
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDE) $(NORMAL_FILES) $(LINUX_FILES) $(CFLAGS_SECRET) $(LINUX_LIBS) -pthread -o Silent_Pass 

#silentpass:
#	$(CC) $(CFLAGS) $(CFLAGS_SECRET) $(LDFLAGS) $(LIBS) $(INCLUDE) -o Silent_Pass src/main.c src/firefox/firefox.c src/chrome/chrome.c lib/cJSON.c
#silentpass: firefox.o main.o chrome.o cJSON.o 
#	$(CC) $(CFLAGS) $(CFLAGS_SECRET) $(LDFLAGS) -lssl -lcrypto -l nspr4 -l nss3 -l iniparser -l sqlite3 -o Silent_Pass firefox.o main.o cJSON.o chrome.o
#
#firefox.o: 
#	$(CC) $(CFLAGS) $(LDFLAGS) -I. -Isrc -I/usr/include/nss -I/usr/include/nspr -c firefox/firefox.c
#
#main.o: 
#	$(CC) $(CFLAGS) -Isrc -I/usr/include/nss -I/usr/include/nspr -c main.c
#
#cJSON.o: 
#	$(CC) $(CFLAGS) -c lib/cJSON.c
#
#chrome.o: 
#	$(CC) $(CFLAGS) $(CFLAGS_SECRET) -Isrc -c chrome/chrome.c

clean:
	rm *.o
