#include <stdlib.h>
//#include <openssl/evp.h>
#include <string.h>
#include <cctype>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <iterator>
#include <vector>
#include <bitset>

void decryption() {

}



class File {
    std::ifstream input;
    std::ofstream output;
    long start, size;
    std::string inputName, outputName, actionType, cipherType;

public:
    void inicialize(const std::string & action, const std::string & cipher){
        input.open(inputName, std::ios::binary);
        if (input.fail()){
            std::cout << "Nepodarilo se otevrit vstupni soubor" << std::endl;
            exit(1);
        }
        output.open(outputName, std::ios::binary );
        start = read4bytes(10);
        size = read4bytes(2);
        this->actionType = action;
        this->cipherType = cipher;
        std::cout << "Start: " << std::dec << start << "...Size: " << std::dec << size << std::endl;

    }

    File() {
        inputName = "Mad_scientist.bmp";
        outputName = "Mad_scientist_ebc.bmp";
    }

    ~File() {
        input.close();
        output.close();
    }

    void copyHeader() {
        input.seekg(0);
        char * header = new char[this->start];
        input.read(header, start);
        output.write(header, start);
    }

    void copyBody() {
       char * body = new char[this->size - this->start];
       input.seekg(start);
       input.read(body, this->size - this->start);
       output.write(body, this->size - this->start);
   }

    long read4bytes(unsigned int from) {
        char * tmp = new char[4];
        input.seekg(from);
        input.read(tmp, 4);
        std::stringstream iss;
        for (int i = 3; i >= 0; --i) {
            iss << std::setw(2) << std::setfill('0') << std::hex << (unsigned int)(unsigned char)tmp[i] << std::flush;
            std::cout << std::setw(2) << std::setfill('0') << std::hex << (unsigned int)(unsigned char)tmp[i] << " " <<std::flush;
        }
        std::cout << iss.str() << std::endl;
        return std::stol(iss.str(), nullptr, 16);
    }

    void encryption() {
        unsigned char key[EVP_MAX_KEY_LENGTH] = "Muj tajny klic";  // klic pro sifrovani
        unsigned char iv[EVP_MAX_IV_LENGTH] = "inicial. vektor";  // inicializacni vektor

        OpenSSL_add_all_ciphers();
        int stLength = 0;
        int tmpLength = 0;

        EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();

        if(std::string{argv[2]} == "ecb"){
            EVP_EncryptInit(ctx, EVP_aes_128_ecb(), key, iv);
        } else {
            EVP_EncryptInit(ctx, EVP_aes_128_cbc(), key, iv);
        }

        EVP_EncryptUpdate(ctx,  out_img, &stLength, img, length);  // sifrovani ot1
        free(img);

        EVP_EncryptFinal(ctx, out_img + stLength, &tmpLength);  // dokonceni (ziskani zbytku z kontextu)
        stLength += tmpLength;

        EVP_CIPHER_CTX_free(ctx);
    }

};

int main(int argc, char *argv[]) {
    File file;
    file.inicialize("enc", "cbc");
    file.copyHeader();
    file.copyBody();



    return 0;
}
