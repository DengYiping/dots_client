# dots_client
A C client for DDoS Open Threat Signaling (DOTS) protocol

To install, we need to install first libcoap:
```
$ git clone https://github.com/obgm/libcoap.git
$ cd libcoap
$ git checkout 6fc3a7315f6629d804cc7928004cddeb4a84443c
$ ./autogen.sh
$ ./configure --disable-documentation --with-openssl
$ make
$ sudo make install
```

Then install libcbor from src following [here](https://github.com/PJK/libcbor)

To compile, do the following:
```
mkdir build
cd build
cmake ..
make
```
