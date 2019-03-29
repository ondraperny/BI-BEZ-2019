#include <stdio.h>
#include <string.h>
#include <openssl/evp.h>
#include <limits.h>
#include <iostream>


int main(int argc, char *argv[]){
    unsigned i;
    int res;
    char text[10];
    char hashFunction[] = "sha256";  // zvolena hashovaci funkce ("sha1", "md5" ...)

    EVP_MD_CTX *ctx;  // struktura kontextu
    const EVP_MD *type; // typ pouzite hashovaci funkce
    unsigned char hash[EVP_MAX_MD_SIZE]; // char pole pro hash - 64 bytu (max pro sha 512)
    unsigned length;  // vysledna delka hashe

    /* Inicializace OpenSSL hash funkci */
    OpenSSL_add_all_digests();
    /* Zjisteni, jaka hashovaci funkce ma byt pouzita */
    type = EVP_get_digestbyname(hashFunction);

    /* Pokud predchozi prirazeni vratilo -1, tak nebyla zadana spravne hashovaci funkce */
    if(!type) {
        printf("Hash %s neexistuje.\n", hashFunction);
        exit(1);
    }

    srand(time(NULL));
    ctx = EVP_MD_CTX_create(); // create context for hashing
    if(ctx == NULL) exit(2);

    while ( true ) {
        const char * tmp = (std::to_string(std::rand()%1000000000)).c_str();
        strncpy(text, tmp, strlen(tmp));

        /* Hash the text */
        res = EVP_DigestInit_ex(ctx, type, NULL); // context setup for our hash type
        if (res != 1) exit(3);
        res = EVP_DigestUpdate(ctx, text, strlen(text)); // feed the message in
        if (res != 1) exit(4);
        res = EVP_DigestFinal_ex(ctx, hash, (unsigned int *) &length); // get the hash
        if (res != 1) exit(5);

        if( hash[0] == 0xAA && hash[1] == 0xBB )
            break;
    }

    EVP_MD_CTX_destroy(ctx); // destroy the context

    /* Vypsani vysledneho hashe */
    printf("Hash textu \"%s\" je: ", text);
    for(i = 0; i < length; i++){
        printf("%02x", hash[i]);
    }
    printf("\n");

    exit(0);
}
