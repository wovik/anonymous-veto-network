#pragma once

#include <openssl/bio.h>>

int createSocket()
{
    BIO *cbio, *out;
    int len;
    char tmpbuf[1024];

    cbio = BIO_new_connect("localhost:http");
    out = BIO_new_fp(stdout, BIO_NOCLOSE);
    if (BIO_do_connect(cbio) <= 0) {
        fprintf(stderr, "Error connecting to server\n");
        // ERR_print_errors_fp(stderr);
        exit(1);
    }
    BIO_puts(cbio, "GET / HTTP/1.0\n\n");
    for ( ; ; ) {
        len = BIO_read(cbio, tmpbuf, 1024);
        if (len <= 0)
            break;
        BIO_write(out, tmpbuf, len);
    }
    BIO_free(cbio);
    BIO_free(out);
}