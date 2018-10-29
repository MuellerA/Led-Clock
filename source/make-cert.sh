#/bin/sh

# Key Length in bits: 512 or 1024
KeyLen=1024

# replace by something meaningful
CommonName=Clock

openssl req -x509 -nodes -newkey rsa:$KeyLen -subj /CN=$CommonName -keyform DER -keyout key.pem -out cert.pem -days 9999
openssl rsa  -in key.pem  -outform DER -out key.der
openssl x509 -in cert.pem -outform DER -out cert.der
