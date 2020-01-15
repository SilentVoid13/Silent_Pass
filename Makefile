CC=gcc
WC=i686-w64-mingw32-gcc

#CFLAGS = -Wall -Wextra -Wno-pointer-sign -fPIC
LDFLAGS=-Wl,-rpath,/usr/lib/nss -Wl,-rpath,/usr/lib/nspr
INCLUDE= -I. -Isrc -Ilib -Iincludes -I/usr/include/nss -I/usr/include/nspr -Llib
32BITS = -m32

LINUX_LIBS= -lssl -lcrypto -lnspr4 -lnss3 -liniparser_s -lsqlite3_s -largtable3_s -lcJSON_s -ldl -lm $(SECRET_LIBS)
SECRET_LIBS = $(shell i686-pc-linux-gnu-pkg-config --cflags --libs libsecret-1)

WINDOWS_LIBS = -lcrypto_s -lcrypt32 -lws2_32 -lgdi32 -lm
#OPENSSL_LIBS := $(wildcard lib/*.a)

NORMAL_FILES := $(wildcard src/*.c) $(wildcard src/utilities/*.c) #$(wildcard lib/*.c)
LINUX_FILES := $(wildcard src/linux/*.c)
WINDOWS_FILES := $(wildcard src/win32/*.c)

print-%  : ; @echo $* = $($*)

linux:
	$(CC) $(32BITS) $(CFLAGS) $(LDFLAGS) $(INCLUDE) $(NORMAL_FILES) $(LINUX_FILES) $(LINUX_LIBS) -pthread -o bin/Silent_Pass
win:
	$(WC) $(CFLAGS) $(INCLUDE) $(NORMAL_FILES) $(WINDOWS_FILES) $(WINDOWS_LIBS) -o bin/Silent_Pass
clean:
	rm *.o
