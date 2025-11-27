#!/bin/bash
# Create Self-Signed Code Signing Certificate
# Usage: ./create-cert.sh

CERT_NAME="Dotenv C++ Library"
CERT_PASSWORD="dotenv123"
PFX_FILE="certificate.pfx"
KEY_FILE="certificate.key"
CRT_FILE="certificate.crt"

echo "Creating self-signed code signing certificate..."

# Generate private key
openssl genrsa -out "$KEY_FILE" 2048

# Generate self-signed certificate
openssl req -new -x509 -key "$KEY_FILE" -out "$CRT_FILE" -days 1825 \
    -subj "/CN=$CERT_NAME/O=Development/C=US"

# Export to PFX format
openssl pkcs12 -export -out "$PFX_FILE" -inkey "$KEY_FILE" -in "$CRT_FILE" \
    -password pass:"$CERT_PASSWORD"

# Clean up intermediate files
rm "$KEY_FILE" "$CRT_FILE"

echo ""
echo "Certificate created successfully!"
echo "Certificate file: $PFX_FILE"
echo "Password: $CERT_PASSWORD"
echo ""
echo "WARNING: This is a self-signed certificate for TESTING ONLY."
echo "It will trigger security warnings when the DLL is used."
echo ""
echo "To sign the DLL, run:"
echo "  make sign"

# Make the script executable
chmod +x create-cert.sh
