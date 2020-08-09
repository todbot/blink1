Controlling blink(1) devices on FreeBSD
=====================================
In FreeBSD blink(1) will work by default as the root user, however if you wish to allow other users access to control the blink(1) you'll need to install a custom devd rule (similar to Linux's udev) to perform extra commands when the blink(1) is detected: 
1. Choose whether you would like to allow all users to have access to the blink(1) device:

    [blink1.conf](./blink1.conf) - Allows only users of the "blink1" group (and root), if you choose this config you may need to create a new group.  The following command creates the group ```blink1``` and adds the current user to that group *(remember to log out and log back in)*.
    
      ```
      beastie% id
      uid=1001(me) gid=1001(me) groups=1001(me)
      beastie% sudo pw groupadd blink1 && sudo pw groupmod blink1 -m ${USER}
      Password:
      beastie% id 
      uid=1001(me) gid=1001(me) groups=1001(me)
      beastie% logout
      Connection to beastie closed.
      laptop% ssh me@beastie
      beastie% id
      uid=1001(me) gid=1001(me) groups=1001(me),1004(blink1)
      beastie%
      ```

    [blink1-open.conf](./blink1-open.conf) - Allows *all* users the ability to access blink1

2. Copy the selected config file into ```/usr/local/etc/devd```:

  ```beastie% sudo cp <blink1file>.conf /usr/local/etc/dev/```

3. Restart the devd daemon:
  
  ```
  beastie% sudo service devd restart
  Password:
  Stopping devd.
  Waiting for PIDS: 9689.
  Starting devd.
  beastie%
  ```
4. [Re]plug-in the blink(1)

5. Success
  
  ```
  beastie% id 
  uid=1001(me) gid=1001(me) groups=1001(me),1004(blink1)
  beastie% blink1-tool --on
  set dev:0:0 to rgb:0xff,0xff,0xff over 300 msec
  beastie%
  ```

## Finding the USB vendor and product
The default  vendor and product are set in the [blink1.conf](./blink1.conf) and [blink1-open.conf](./blink1-open.conf) files however should you need to validate the values you can run the following command once you've inserted the blink(1).  

```
% sudo usbconfig | grep blink1 | cut -d: -f1 | xargs -I{} sudo usbconfig -d {} dump_all_desc | grep -i -e idvendor -e idprod
Password:
  idVendor = 0x27b8
  idProduct = 0x01ed
%
```

## Command-line tool "blink1-tool"

To build the commamnd-line tool, you should be able to do, in a local checkout of the [blink1-tool repo](https://github.com/todbot/blink1-tool), as well as a checkout of the [hidapi repo](https://github.com/libusb/hidapi/tree/083223e77952e1ef57e6b77796536a3359c1b2a3) in the respective subdir.

```
beastie% sudo pkg install -y gcc git gmake libiconv
beastie% cd /tmp && git clone https://github.com/todbot/blink1-tool && cd blink1-tool
beastie% git clone https://github.com/libusb/hidapi/tree/083223e77952e1ef57e6b77796536a3359c1b2a3 # Correct as of 2020-08-08
beastie% sudo gmake install
```

NOTE: There is a pending issue ( todbot/blink1#654 ) which it will cause the install to fail to be able to install the "blink1-tool" executable,

Then to make the blink(1) do things, do something like:

```
/usr/local/bin/blink1-tool --rgb 255,0,255
```

Type "./blink1-tool" by itself to get a help screen.
## Credits
https://www.ccammack.com/posts/display-server-notifications-using-a-blink1/
https://www.freebsd.org/cgi/man.cgi?query=devd.conf
https://www.freebsd.org/cgi/man.cgi?query=devfs.rules


-eof-

