# Packages to install for openwrt
Install command to use: `opkg install ...`

And the packages that needs to install:
```
openssl-util
git
gcc
autoconf
automake
curl
```

We need to change the types.h file.

To compile libcoap, we do:
```
./autogen.sh
CFLAGS=-fPIC LDFLAGS=-fPIC ./configure --with-tinydtls --with-gnu-ld --disable-documentation
chmod +x install-sh
make
make install
```
