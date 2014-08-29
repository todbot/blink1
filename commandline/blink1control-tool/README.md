
blink1control-tool 
==================

## `blink1-tool`-like program for use with Blink1Control


### Use

Usage is just like `blink1-tool`.

Except that currently only the setting of colors is supported.

The url it uses defaults to `http://localhost:8934/`.  If you are
controlling a Blink1Control on another system or urn the Blink1Server
on another port, change it with `--baseurl <url>`.

### Prerequisites:

- libcurl

### Compiling:

On Mac OS X:
```
% wget http://curl.haxx.se/download/curl-7.37.1.tar.gz
% tar xvzf curl-7.37.1.tar.gz
% cd curl-7.37.1
% ./configure --prefix `pwd`/../curl-mac --disable-shared --disable-ldap --without-zlib --without-libssh2 --without-ssl --disable-crypto-auth
% make && make install
% cd ..
% make
```

 On Windows:
 ```
% wget http://curl.haxx.se/download/curl-7.37.1.tar.gz 
% tar xvzf curl-7.37.1.tar.gz
% cd curl-7.37.1
# In the file libcurl.pc.in add -DCURL_STATICLIB to Cflags. 
% ./configure --prefix `pwd`/../curl-win --disable-shared --disable-ldap --without-zlib --without-libssh2 
% make && make install
% cd ..
% gcc -static -o curl-simple curl-simple.c `curl-win2/bin/curl-config --cflags` `curl-win2/bin/curl-config --static-libs`
```
