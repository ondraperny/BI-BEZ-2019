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
    int length;
    int tmpLength;

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
        actualSize = input.tellg();
        this->actionType = action;
        this->cipherType = cipher;

        // check if input is ok - otherwise warn user
        isInputCorrect(input.fail());

        output.open(outputName, std::ios::binary);
        body = new unsigned char[this->actualSize - this->start];
        input.seekg(start);
        input.read((char *) &body[0], this->actualSize - this->start);
        bodyOut = new unsigned char[this->actualSize - this->start]();

        // inicialize crypto variables
        evpCipher = (cipher == "ecb") ? EVP_des_ecb() : EVP_des_cbc();
        ctx = EVP_CIPHER_CTX_new();
        OpenSSL_add_all_ciphers();
        length = 0;
        tmpLength = 0;
    }

    // encryption and decription for both ecb and cbc (based on how were inicialized class variables)
    void encryption() {
        EVP_EncryptInit(ctx, evpCipher, key, iv);
        if (!res) exit(2);
        EVP_EncryptUpdate(ctx, bodyOut, &length, body, size - start);  // sifrovani ot1
        if (!res) exit(2);
        EVP_EncryptFinal(ctx, body + length, &tmpLength);  // dokonceni (ziskani zbytku z kontextu)
        if (!res) exit(2);
        output.write((char *) &bodyOut[0], length + tmpLength);
    }

    void decryption() {
        res = EVP_DecryptInit(ctx, evpCipher, key, iv);
        if (!res) exit(2);
        res = EVP_DecryptUpdate(ctx, bodyOut, &length,  body, actualSize - start);
        if (!res) exit(2);
        output.write((char *) &bodyOut[0], length);
        res = EVP_DecryptFinal(ctx, bodyOut, &length);
        output.write((char *) &bodyOut[0], length);
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

    void solve(std::string a, std::string b, std::string c) {
        inicialize(a, b, c);
        copyHeader();

        if (actionType == "-e") {
            encryption();
            std::cout << "Vytvoreny zasifrovany soubor: " << outputName << std::endl;
        } else {
            decryption();
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
