#!/usr/bin/env bash
echo "Listening to heartbeat for testing ..."
coap-server -p 12341 -A 127.0.0.1 -v 9 -c certs/server-combined.pem -C certs/ca-cert.pem