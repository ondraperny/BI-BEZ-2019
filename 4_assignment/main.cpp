#include <stdio.h>
#include <string.h>
#include <iostream>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rand.h>

class File {
private:
    std::string inputName;
    std::string outputName;
    std::string keyName;
    std::string cipherName;
    std::string actionType;
    unsigned blocksize;
    int res;
    unsigned char *inBlock, *outBlock;
    unsigned char *keyArray;
    int outSize;
    int keySize;
    int read;

    FILE *input;
    FILE *output;
    FILE *keyCFile;
    EVP_CIPHER_CTX *ctx;
    EVP_PKEY *keyC;
    unsigned char iv[EVP_MAX_IV_LENGTH];

    void isInputCorrect(bool FileNotOpen) {
        if (!FileNotOpen) {
            std::cout << "Vstupni soubor se nepodarilo otevrit." << std::endl;
            exit(3);
        }
    }

public:
    File(std::string actionT, std::string keyN, std::string inputN,
               std::string outputN, std::string cipherN) {

        if (RAND_load_file("/dev/random", 32) != 32) {
            puts("Cannot seed the random generator!");
            exit(1);
        }

        actionType = actionT;
        inputName = inputN;
        outputName = outputN;
        keyName = keyN;
        cipherName = cipherN;

        blocksize = 1024;

        inBlock = new unsigned char[blocksize];
        outBlock = new unsigned char[blocksize + 1000];

        keyCFile = fopen(keyName.c_str(), "r");
        isInputCorrect(keyCFile);

        ctx = EVP_CIPHER_CTX_new();
        OpenSSL_add_all_ciphers();

        input = fopen(inputName.c_str(), "r");
        isInputCorrect(input);

        output = fopen(outputName.c_str(), "w");
        isInputCorrect(output);

    }

    ~File() {
        fclose(input);
        fclose(output);
        fclose(keyCFile);

        delete[] inBlock;
        delete[] outBlock;
        delete[] keyArray;
    }


    void encrypt() {
        keyC = PEM_read_PUBKEY(keyCFile, NULL, NULL, NULL);
        keyArray = new unsigned char[EVP_PKEY_size(keyC)];

        const EVP_CIPHER *cipher = EVP_get_cipherbyname(cipherName.c_str());
        if (!cipher) {
            std::cout << "Spatne jmeno sifry." << std::endl;
            exit(4);
        }

        res = EVP_SealInit(ctx, cipher, &keyArray, &keySize, iv, &keyC, 1);
        if (!res) exit(2);

        fprintf(output, "%s %d \n", cipherName.c_str(), EVP_MAX_IV_LENGTH);
        fwrite(iv, sizeof(unsigned char), EVP_MAX_IV_LENGTH, output);
        fwrite(keyArray, sizeof(unsigned char), keySize, output);

        while ((read = fread(inBlock, sizeof(unsigned char), blocksize, input)) > 0) {
            res = EVP_SealUpdate(ctx, outBlock, &outSize, inBlock, read);
            if (!res) exit(2);
            fwrite(outBlock, sizeof(unsigned char), outSize, output);
        }
        res = EVP_SealFinal(ctx, outBlock, &outSize);
        if (!res) exit(2);
        fwrite(outBlock, sizeof(unsigned char), outSize, output);
    }

    void decrypt() {
        keyC = PEM_read_PrivateKey(keyCFile, NULL, NULL, NULL);

        keySize = EVP_PKEY_size(keyC);
        keyArray = new unsigned char[keySize];

        char cipherNameChar[blocksize];
        int ivSize;
        fscanf(input, "%s %d \n", cipherNameChar, &ivSize);
        fread(iv, sizeof(unsigned char), ivSize, input);
        fread(keyArray, sizeof(unsigned char), keySize, input);

        const EVP_CIPHER *cipher = EVP_get_cipherbyname(cipherNameChar);
        if (!cipher) {
            std::cout << "Spatne jmeno sifry." << std::endl;
            exit(4);
        }

        res = EVP_OpenInit(ctx, cipher, keyArray, keySize, iv, keyC);
        if (!res) exit(2);
        while ((read = fread(inBlock, sizeof(unsigned char), blocksize, input)) > 0) {
            res = EVP_OpenUpdate(ctx, outBlock, &outSize, inBlock, read);
            if (!res) exit(2);
            fwrite(outBlock, sizeof(unsigned char), outSize, output);
        }
        res = EVP_OpenFinal(ctx, outBlock, &outSize);
        if (!res) exit(2);
        fwrite(outBlock, sizeof(unsigned char), outSize, output);
    }

    void solve() {
        if (actionType == "-e") {
            encrypt();
            std::cout << "Vytvoreny zasifrovany soubor: " << outputName << std::endl;
        } else {
            decrypt();
            std::cout << "Vytvoreny desifrovany soubor: " << outputName << std::endl;
        }
    }
};

int main(int argc, char *argv[]) {
    if (!((strcmp(argv[1], "-e") == 0) && (argc == 6)) &&
        !((strcmp(argv[1], "-d") == 0) && (argc == 5))) {
        std::cout << "Spatne volani, format: [ -e | -d ] soubor_s_verejnym/soukromym_klicem "
                     "vstupni_soubor vystupni_soubor typ_sifry" << std::endl;
        return 1;
    }

    std::string cipherName = (argc == 6) ? argv[5] : "none";
    File cipher(argv[1], argv[2], argv[3], argv[4], cipherName);
    cipher.solve();
}
