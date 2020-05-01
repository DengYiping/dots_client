FROM openwrtorg/rootfs:x86-64
RUN mkdir -p /var/lock \
  && mkdir -p /var/run \
  && mkdir -p /code \
  && opkg update \
  && opkg install \
    openssl-util \
    git \
    gcc \
    make \
    autoconf \
    automake \
    libtool-bin \
    pkgconf \
    grep \
    curl \
  && cp /usr/bin/libtoolize /usr/bin/libtool
  && cp /usr/bin/pkgconf /usr/bin/pkg-config

COPY ../ /code/

EXPOSE 80 443 22

CMD ["/sbin/init"]
