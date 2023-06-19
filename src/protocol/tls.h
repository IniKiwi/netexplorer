#pragma once

#include "../networktask.h"

#include <openssl/ssl.h>
#include <openssl/err.h>

int tls_init();
SSL* tls_connect(Ipv4Addr addr, size_t timeout);
int tls_send(SSL* ssl, void* buf, int num);
int tls_recv(SSL* ssl, void* buf, int num);
int tls_close(SSL* ssl);