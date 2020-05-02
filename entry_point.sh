#!/usr/bin/env bash
SERVER_ADDR=192.168.1.224
SERVER_PORT=4646
build/dots_client -s $SERVER_ADDR -p $SERVER_PORT --debug --cert_file=certs/ca-cert.pem --client_cert_file=certs/client-cert.pem --client_key_file=certs/client-key.pem
