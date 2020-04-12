#!/usr/bin/env bash
SERVER_ADDR=127.0.0.1
SERVER_PORT=12342
make clean
make
bin/main -s 127.0.0.1 -p 12342 --debug --cert_file=certs/ca-cert.pem --client_cert_file=certs/client-cert.pem --client_key_file=certs/client-key.pem