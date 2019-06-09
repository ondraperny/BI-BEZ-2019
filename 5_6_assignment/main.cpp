#include <cstring>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>

int main(int argc, char const *argv[]) {
    char buffer[512];

    struct sockaddr_in address;
    socklen_t sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    address.sin_family = AF_INET;
    address.sin_port = htons(443);
    address.sin_addr.s_addr = inet_addr("147.32.232.248"); // ip addresa fit cvutu
    if (connect(sock, (struct sockaddr *) &address, sizeof(address)) != 0)
        return 1;

    SSL_library_init();
    SSL_CTX *ssl_ctx = SSL_CTX_new(SSLv23_client_method());
    if (!ssl_ctx)
        return 1;
    if (!SSL_CTX_set_default_verify_paths(ssl_ctx))
        return 1;

    SSL_CTX_set_options(ssl_ctx, SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1);
    SSL *ssl = SSL_new(ssl_ctx);
    if (!ssl)
        return 1;
    if (!SSL_set_fd(ssl, sock))
        return 1;

    // ECDHE-RSA elipticke krivky pri vymene klice diffie-hellmanem,
    // AES256-GCM symetricka blokova sifra s delkou bloku 256 bitu a gcm operacnim modem
    // SHA384 hashovaci funkce pro dany certifikat
    SSL_set_cipher_list(ssl, "ALL:!ECDHE-RSA-AES256-GCM-SHA384");

    if (SSL_connect(ssl) <= 0)
        return 1;
    if (SSL_get_verify_result(ssl) == X509_V_OK) {
        std::cout << "Certifikat je platny" << std::endl;
    } else {
        std::cout << "Certifikat neplatny" << std::endl;
        return 1;
    }

    const SSL_CIPHER *cipher = SSL_get_current_cipher(ssl);
    std::cout << "Sifra: " << SSL_CIPHER_get_name(cipher) << std::endl;

    std::cout << "Dostupne sifry clienta serazene dle priority:" << std::endl;
    const char *availableCipher = nullptr;

    int index = 0;
    while (true) {
        availableCipher = SSL_get_cipher_list(ssl, index);
        if (availableCipher == nullptr)
            break;
        std::cout << availableCipher << std::endl;
        index++;
    }
    std::cout << "---------------------------------" << std::endl;

    X509 *cert = SSL_get_peer_certificate(ssl);
    if (!cert)
        return 1;

    FILE *certificateFile = fopen("certifikat.pem", "w");
    if (!certificateFile)
        return 1;
    if (!PEM_write_X509(certificateFile, cert))
        return 1;
    fclose(certificateFile);

    snprintf(buffer, 512, "GET /student/odkazy HTTP/1.1\r\nHost: fit.cvut.cz\r\n\r\n");
    if (SSL_write(ssl, buffer, strlen(buffer) + 1) <= 0) {
        printf("SSL_write error\n");
        return 1;
    }

    FILE *out = fopen("stazenaStranka", "w");
    if (!out)
        return 1;

    int readLen;
    while (true) {
        readLen = (int) SSL_read(ssl, buffer, 512);
        if (readLen <= 0)
            break;
        fwrite(buffer, sizeof(buffer[0]), readLen, out);
    }
    fclose(out);
    SSL_shutdown(ssl);
    close(sock);
    SSL_CTX_free(ssl_ctx);
    SSL_free(ssl);
    return 0;
}
