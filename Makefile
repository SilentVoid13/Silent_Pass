CC=gcc
WC=i686-w64-mingw32-gcc
#CFLAGS = -Wall -Wextra -Wno-pointer-sign -fPIC
#CFLAGS = -m32
CFLAGS_SECRET = $(shell pkg-config --cflags --libs libsecret-1)
LDFLAGS=-Wl,-rpath,/usr/lib/nss -Wl,-rpath,/usr/lib/nspr
INCLUDE= -I. -Isrc -Ilib -Iincludes -I/usr/include/nss -I/usr/include/nspr 

#SRC_FILES := $(wildcard lib/argp/*.c)
#OBJ_FILES := $(patsubst lib/argp/%.c,./%.o,$(SRC_FILES))

LINUX_LIBS= -lssl -lcrypto -l nspr4 -l nss3 -l iniparser -ldl -lm
WINDOWS_LIBS = -lcrypt32 -lws2_32 -lgdi32 -lm

NORMAL_FILES := $(wildcard src/*.c) $(wildcard lib/*.c) $(wildcard src/utilities/*.c)
LINUX_FILES := $(wildcard src/linux/*.c)
WINDOWS_FILES := $(wildcard src/win32/*.c)

OPENSSL_LIBS := $(wildcard lib/*.a)

print-%  : ; @echo $* = $($*)

linux:
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDE) $(CFLAGS_SECRET) $(NORMAL_FILES) $(LINUX_FILES) $(LINUX_LIBS) -pthread -o bin/Silent_Pass 
win: 
	$(WC) $(CFLAGS) $(INCLUDE) $(NORMAL_FILES) $(WINDOWS_FILES) $(OPENSSL_LIBS) $(WINDOWS_LIBS) -o bin/Silent_Pass
clean:
	rm *.o
