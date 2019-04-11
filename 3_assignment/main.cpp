#include <stdlib.h>
#include <openssl/evp.h>
#include <string.h>
#include <cctype>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <iterator>
#include <vector>
#include <bitset>
#include <string.h>
#include <stdio.h>


class File {
    std::ifstream input;
    std::ofstream output;
    long start, size;
    long actualSize;
    std::string inputName, outputName, actionType, cipherType;

    char *header;
    unsigned char *body;
    unsigned char *bodyOut;

    // crypto variables
    int res;
    EVP_CIPHER_CTX * ctx;
    const EVP_CIPHER * evpCipher;
    unsigned char key[EVP_MAX_KEY_LENGTH];  // klic pro sifrovani
    unsigned char iv[EVP_MAX_IV_LENGTH];  // inicializacni vektor

    int numberOfBlocks, rest;
    int otLength, stLength;

    // copy bmp header
    void copyHeader() {
        header = new char[this->start];
        input.seekg(0);
        input.read(header, start);
        output.write(header, start);
    }

    // read 4 bytes from 'from' position and transform them to long int
    long read4bytes(unsigned int from) {
        char *tmp = new char[4];
        input.seekg(from);
        input.read(tmp, 4);
        std::stringstream iss;
        for (int i = 3; i >= 0; --i) {
            iss << std::setw(2) << std::setfill('0') << std::hex << (unsigned int) (unsigned char) tmp[i] << std::flush;
//            std::cout << std::setw(2) << std::setfill('0') << std::hex << (unsigned int) (unsigned char) tmp[i] << " "
//                      << std::flush;
        }
//        std::cout << iss.str() << std::endl;
        return std::stol(iss.str(), nullptr, 16);
    }

    // inicialize all class variables
    void inicialize(const std::string &action, const std::string &cipher, const std::string &name) {
        const char * cipherKey = "Muj tajny klic";
        const char * cipherIV = "inicial. vektor";
        memcpy(key, cipherKey, sizeof(cipherKey));
        memcpy(iv, cipherIV, sizeof(cipherIV));

        inputName = name + ".bmp";
        if (action == "-e")
            if (cipher == "ecb")
                outputName = name + "_ecb.bmp";
            else
                outputName = name + "_cbc.bmp";
        else
            outputName = name + "_dec.bmp";

        input.open(inputName, std::ios::binary);
        start = read4bytes(10);
        size = read4bytes(2);
        input.seekg(0,std::ios_base::end);
        actualSize = (long)input.tellg();
        this->actionType = action;
        this->cipherType = cipher;

        // check if input is ok - otherwise warn user
        isInputCorrect(input.fail());

        output.open(outputName, std::ios::binary);

        // length of copied block - works for any value (here is only place where it needs to be changed)
        otLength = 1024;

        input.seekg(start);
        body = new unsigned char[otLength];
        bodyOut = new unsigned char[otLength];
        numberOfBlocks = (this->actualSize - this->start) / otLength;
        rest = (this->actualSize - this->start) % otLength;

        // inicialize crypto variables
        ctx = EVP_CIPHER_CTX_new();
        OpenSSL_add_all_ciphers();
    }

    // cipher can have wither value "ecb" or "cbc"
    void encryption(const std::string & cipher) {
        evpCipher = (cipher == "ecb") ? EVP_des_ecb() : EVP_des_cbc();
        EVP_EncryptInit(ctx, evpCipher, key, iv);
        if (!res) exit(2);

        for(size_t i = 0; i < numberOfBlocks ; ++i) {
            input.read((char *) &body[0], otLength);
            EVP_EncryptUpdate(ctx, bodyOut, &stLength, body, otLength);  // sifrovani ot1
            if (!res) exit(2);
            output.write((char *) &bodyOut[0], stLength);
        }
        input.read((char *) &body[0], rest);
        EVP_EncryptUpdate(ctx, bodyOut, &stLength, body, rest);  // sifrovani ot1
        if (!res) exit(2);
        output.write((char *) &bodyOut[0], stLength);

        EVP_EncryptFinal(ctx, bodyOut, &stLength);  // dokonceni (ziskani zbytku z kontextu)
        if (!res) exit(2);
        output.write((char *) &bodyOut[0],stLength);
    }

    // cipher can have wither value "ecb" or "cbc"
    void decryption(const std::string & cipher) {
        evpCipher = (cipher == "ecb") ? EVP_des_ecb() : EVP_des_cbc();
        res = EVP_DecryptInit(ctx, evpCipher, key, iv);
        if (!res) exit(2);

        for(size_t i = 0; i < numberOfBlocks ; ++i) {
            input.read((char *) &body[0], otLength);

            res = EVP_DecryptUpdate(ctx, bodyOut, &stLength, body, otLength);
            if (!res) exit(2);
            output.write((char *) &bodyOut[0], stLength);
        }
        input.read((char *) &body[0], rest);
        res = EVP_DecryptUpdate(ctx, bodyOut, &stLength, body, rest);
        if (!res) exit(2);
        output.write((char *) &bodyOut[0], stLength);

        res = EVP_DecryptFinal(ctx, bodyOut, &stLength);
        output.write((char *) &bodyOut[0], stLength);
    }

    void isInputCorrect(bool FileNotOpen) {
        if ( start < 14 || start >= size || actualSize <= 14 || (actionType == "-e" && size != actualSize) || FileNotOpen ) {
            std::cout << "Vstupni soubor neni validni .bmp soubor (program ukoncen)" << std::endl;
            exit(3);
        }
    }

public:
    File() {
    }

    ~File() {
        input.close();
        output.close();

        delete body;
        delete bodyOut;
        delete header;

        EVP_CIPHER_CTX_free(ctx);
    }

    void solve(const std::string & action, const std::string & cipher, const std::string & name) {
        inicialize(action, cipher, name);
        copyHeader();

        if (actionType == "-e") {
            encryption(cipher);
            std::cout << "Vytvoreny zasifrovany soubor: " << outputName << std::endl;
        } else {
            decryption(cipher);
            std::cout << "Vytvoreny desifrovany soubor: " << outputName << std::endl;
        }
    }
};


int main(int argc, char *argv[]) {
    if ((argc != 4) || ((std::string(argv[1]) != "-e") && ((std::string(argv[1]) != "-d")))
        || ((std::string(argv[2]) != "ecb") && (std::string(argv[2]) != "cbc"))) {
        std::cout << "Spatne volani, format: [ -e | -d ] [ ecb | cbc ] NazevVstupnihoSouboru (pozn. bez koncovky .bmp)" << std::endl;
        return 1;
    }

    File file;
    file.solve(argv[1], argv[2], argv[3]);

    return 0;
}
