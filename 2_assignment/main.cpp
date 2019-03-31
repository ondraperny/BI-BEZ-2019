#include <stdlib.h>
#include <openssl/evp.h>
#include <string.h>
#include <cctype>
#include <iostream>
#include <iomanip>
#include <algorithm>

// return true if hexChar is valid hexadecimal character
bool isHexChar(const char & hexChar) {
    if ((hexChar >= 'a' && hexChar <= 'f') ||
        (hexChar >= 'A' && hexChar <= 'F') ||
        (hexChar >= '0' && hexChar <= '9'))
        return true;
    return false;
}

void isHexInputCorrect(const std::string & input) {
    for(size_t i = 0; i < input.length(); i++) {
        if(!isHexChar(input[i])) {
            std::cout << "Spatny vstup, ukoncuji program" << std::endl;
            exit(1);
        }
    }
}

std::string hexToBytes(std::string input){
    std::string output, tmp;
    for (size_t i = 0; i < input.length(); i += 2)
        output += std::stoi(input.substr(i, 2), nullptr, 16);
    return output;
}

void decryption() {
    std::string st1In, st2In, otIn, st1Hex, st2Hex, otOut;

    // reading required inputs
    std::cout << "Zadej ST1: ";
    std::getline(std::cin, st1In);
    isHexInputCorrect(st1In);
    std::cout << "Zadej ST2: ";
    std::getline(std::cin, st2In);
    isHexInputCorrect(st2In);
    std::cout << "Zadej OT : ";
    std::getline(std::cin, otIn);

    // converting Hex characters to bytes
    st1Hex = hexToBytes(st1In);
    st2Hex = hexToBytes(st2In);

    // exploiting weakness of using same key by XORing ST1, ST2 and one of OTs (that belong to ST1 or ST2)
    for(size_t i = 0; i < (std::min(st1In.length(), st2In.length()) / 2 ); i++) {
        otOut += (otIn[i] ^ st1Hex[i] ^ st2Hex[i]);
    }
    std::cout << "Tajny OT : " << otOut << std::endl;
}

void encryption() {
    int res;
    unsigned char ot1[1024] = "abcdefghijklmnopqrstuvwxyz0123";  // open text
    unsigned char ot2[1024] = "Druha zpravaDruha zprava-konec";  // open text
    unsigned char st1[1024];  // sifrovany text
    unsigned char st2[1024];  // sifrovany text 2
    unsigned char key[EVP_MAX_KEY_LENGTH] = "Muj klic";  // klic pro sifrovani
    unsigned char iv[EVP_MAX_IV_LENGTH] = "inicial. vektor";  // inicializacni vektor
    const char cipherName[] = "RC4";
    const EVP_CIPHER *cipher;

    OpenSSL_add_all_ciphers();
    /* sifry i hashe by se nahraly pomoci OpenSSL_add_all_algorithms() */
    cipher = EVP_get_cipherbyname(cipherName);
    if (!cipher) {
        printf("Sifra %s neexistuje.\n", cipherName);
        exit(1);
    }

    int otLength = strlen((const char *) ot1);
    int stLength = 0;
    int tmpLength = 0;

    EVP_CIPHER_CTX *ctx; // context structure
    ctx = EVP_CIPHER_CTX_new();
    if (ctx == NULL) exit(2);

    printf("OT : %s\n", ot1);
    /* Sifrovani */
    res = EVP_EncryptInit_ex(ctx, cipher, NULL, key, iv);  // context init - set cipher, key, init vector
    if (res != 1) exit(3);
    res = EVP_EncryptUpdate(ctx, st1, &tmpLength, ot1, otLength);  // encryption of pt

    if (res != 1) exit(4);
    stLength += tmpLength;
    res = EVP_EncryptFinal_ex(ctx, st1 + stLength, &tmpLength);  // get the remaining ct
    if (res != 1) exit(5);
    stLength += tmpLength;

    std::cout << "ST1: ";
    for (int i = 0; i < stLength; i++)
        std::cout  << std::setw(2) << std::setfill('0') << std::hex << (unsigned int) st1[i];
    std::cout << std::endl;

    otLength = strlen((const char *) ot2);
    stLength = 0;
    tmpLength = 0;

    /* Sifrovani */
    res = EVP_EncryptInit_ex(ctx, cipher, NULL, key, iv);  // context init - set cipher, key, init vector
    if (res != 1) exit(3);
    res = EVP_EncryptUpdate(ctx, st2, &tmpLength, ot2, otLength);  // encryption of pt
    if (res != 1) exit(4);
    stLength += tmpLength;
    res = EVP_EncryptFinal_ex(ctx, st2 + stLength, &tmpLength);  // get the remaining ct
    if (res != 1) exit(5);
    stLength += tmpLength;

    std::cout << "ST2: ";
    for (int i = 0; i < stLength; i++)
        std::cout  << std::setw(2) << std::setfill('0') << std::hex << (unsigned int) st2[i];
    std::cout << std::endl;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);
}

int main(int argc, char *argv[]) {
    if ((argc != 2) || ((std::string(argv[1]) != "-e") && ((std::string(argv[1]) != "-d")))) {
        std::cout << "Spatne volani zvolte jeden prepinac (-e nebo -d)" << std::endl;
        return 0;
    }

    if (std::string(argv[1]) == "-e")
        encryption();
    if (std::string(argv[1]) == "-d")
        decryption();

    return 0;
}
