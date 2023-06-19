#include "tls.h"
#include "tcp.h"
#include <unistd.h>

int tsl_is_init = 0;
int tls_init(){
    //if(tsl_is_init == 1) return 0;
    SSL_library_init();
    SSLeay_add_ssl_algorithms();
    SSL_load_error_strings();
    tsl_is_init = 1;
}

SSL* tls_connect(Ipv4Addr addr, size_t timeout){
    int sock = tcp_connect(addr, timeout);
    if(sock == -1){return NULL;}
    const SSL_METHOD *meth = TLS_client_method();
    SSL_CTX *ctx = SSL_CTX_new(meth);
    if(ctx == NULL){
        close(sock);
        return NULL;
    }
    SSL* ssl = SSL_new(ctx);
    if(!ssl){ 
        close(sock);
        SSL_free(ssl);
        return NULL;
    }
    SSL_set_fd(ssl, sock);
    int err = SSL_connect(ssl);
    if(err != 1){
        SSL_get_error(ssl, err);
        ERR_print_errors_fp(stderr); //High probability this doesn't do anything
        fprintf(stderr, "SSL_connect failed with SSL_get_error code %d\n", err);
        close(sock);
        SSL_free(ssl);
        return NULL;
    }
    return ssl;
}

int tls_send(SSL* ssl, void* buf, int num){
    return SSL_write(ssl, buf, num);
}

int tls_recv(SSL* ssl, void* buf, int num){
    return SSL_read(ssl, buf, num);
}

int tls_close(SSL* ssl){
    close(SSL_get_fd(ssl));
    SSL_free(ssl);
    return 0;
}