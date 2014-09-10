

# On Mac: 
# % wget http://curl.haxx.se/download/curl-7.37.1.tar.gz
# % tar xvzf curl-7.37.1.tar.gz
# % cd curl-7.37.1
# % ./configure --prefix `pwd`/../curl-mac --disable-shared --disable-ldap --without-zlib --without-libssh2 --without-ssl --disable-crypto-auth
# % make && make install
# % cd ..
# % gcc -static -o curl-simple curl-simple.c `curl-mac/bin/curl-config --cflags` `curl-mac/bin/curl-config --static-libs`
# 
# On Win:
# % wget http://curl.haxx.se/download/curl-7.37.1.tar.gz
# % tar xvzf curl-7.37.1.tar.gz
# % cd curl-7.37.1
# # In the file libcurl.pc.in add -DCURL_STATICLIB to Cflags. 
# % ./configure --prefix `pwd`/../curl-win --disable-shared --disable-ldap --without-zlib --without-libssh2 
# % make && make install
# % cd ..
# % gcc -static -o curl-simple curl-simple.c `curl-win2/bin/curl-config --cflags` `curl-win2/bin/curl-config --static-libs`



# try to do some autodetecting
UNAME := $(shell uname -s)

ifeq "$(UNAME)" "Darwin"
	OS=macosx
endif

ifeq "$(OS)" "Windows_NT"
	OS=windows
endif

ifeq "$(UNAME)" "Linux"
	OS=linux
endif

ifeq "$(UNAME)" "FreeBSD"
	OS=freebsd
endif

CC=gcc

#################  Mac OS X  ##################################################
ifeq "$(OS)" "macosx"

CFLAGS += `curl-$(OS)/bin/curl-config --static-libs` `curl-$(OS)/bin/curl-config --cflags`

EXE=

endif

#################  Mac OS X  ##################################################
ifeq "$(OS)" "windows"

CFLAGS += `curl-$(OS)/bin/curl-config --static-libs` `curl-$(OS)/bin/curl-config --cflags`

EXE=.exe

endif

#################  Linux  ####################################################
ifeq "$(OS)" "linux"

CFLAGS += `curl-config --static-libs` `curl-config --cflags`


EXE=

#INSTALL = install -D
#EXELOCATION ?= /usr/local/bin
#LIBLOCATION ?= /usr/local/lib
#INCLOCATION ?= /usr/local/include

endif


#####################  Common  ###############################################

CFLAGS += -Wall
#CFLAGS += -Werror
CFLAGS += -std=gnu99
CFLAGS += -g
CFLAGS += -DBLINK1_VERSION=\"$(BLINK1_VERSION)\"


#CFLAGS += -I jsmn-example 
#JSFILES=jsmn-example/jsmn.c jsmn-example/json.c jsmn-example/buf.c jsmn-example/log.c

CFLAGS +=  -I json-parser
JSFILES = json-parser/json.c

#$(OBJS): %.o: %.c
#	$(CC) $(CFLAGS) -c $< -o $@

#all: bjsmn blink1control-tool
all: msg blink1control-tool

msg:
	@echo "Be sure to 'make curl-setup' if you have not already"
	@echo "Building for OS=$(OS) BLINK1_VERSION=$(BLINK1_VERSION)"

curl-setup:
	@echo "setting up curl..."
	#wget http://curl.haxx.se/download/curl-7.37.1.tar.gz
	tar xvzf curl-7.37.1.tar.gz
	cd curl-7.37.1 && ./configure --prefix=`pwd`/../curl-$(OS) --disable-shared --disable-ldap --without-zlib --without-libssh2 --without-ssl --disable-crypto-auth && make && make install

#blink1control-tool: $(OBJS) blink1control-tool.o
blink1control-tool: blink1control-tool.c
	$(CC) -o blink1control-tool$(EXE)  blink1control-tool.c $(JSFILES) $(CFLAGS) 


clean: 
	rm -f $(OBJS)
	rm -f *.o
	rm -f blink1control-tool$(EXE)
	#pushd jsmn-example &
	#pushd jsmn && make clean && popd



