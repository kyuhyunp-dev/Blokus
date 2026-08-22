# Certificates Directory

Local TLS certificates are generated dynamically for local development.

To generate new self-signed certificates, run:

```bash
openssl req -x509 -nodes -days 365 -newkey rsa:2048 \
  -keyout certs/server.key \
  -out certs/server.crt \
  -subj "/CN=localhost" \
  -addext "subjectAltName=DNS:localhost"