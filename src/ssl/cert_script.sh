#!/bin/bash

CERTS_DIR="./certs"

CERTA_CRT="ca-crt.pem"
CERTA_KEY="ca-key.pem"
# RADIO_CRT="radio-crt.pem"
# RADIO_KEY="radio-key.pem"
# RADIO_CSR="radio-csr.pem"
RADIO_CRT="sslserver-crt.pem"
RADIO_KEY="sslserver-key.pem"
RADIO_CSR="sslserver-csr.pem"

RADIO_ID="localhost"

# This will create a new certificate authority. Only need to do this once really.
# echo "Generating CA from the ca.cnf file"
openssl req -new -x509 -days 9999 -config ca.cnf -keyout ca-key.pem -out ca-crt.pem
# This will generate a private key for the server
echo "Generating private key for the server"
openssl genrsa -out $CERTS_DIR/$RADIO_KEY 4096 

# This will generate the certificate signing request
echo "Generating certificate signing request from the server.cnf"
#openssl req -new -config server.cnf -key $CERTS_DIR/$RADIO_KEY -out $RADIO_CSR 
# This ill generate a radio specific certificate signing request
openssl req -new -key $CERTS_DIR/$RADIO_KEY -out $RADIO_CSR -subj "/C=US/ST=MD/L=MA/O=G3/CN=$RADIO_ID"

# This will sign the request
echo "Signing the certificate request"
openssl x509 -req -days 999 -passin "pass:radio" -in $RADIO_CSR -CA $CERTA_CRT -CAkey $CERTA_KEY -CAcreateserial -out $CERTS_DIR/$RADIO_CRT

