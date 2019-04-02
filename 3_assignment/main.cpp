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

void encryption() {
//    int res;
//    unsigned char ot1[1024] = "abcdefghijklmnopqrstuvwxyz0123";  // open text
//    unsigned char ot2[1024] = "Druha zpravaDruha zprava-konec";  // open text
//    unsigned char st1[1024];  // sifrovany text
//    unsigned char st2[1024];  // sifrovany text 2
//    unsigned char key[EVP_MAX_KEY_LENGTH] = "Muj klic";  // klic pro sifrovani
//    unsigned char iv[EVP_MAX_IV_LENGTH] = "inicial. vektor";  // inicializacni vektor
//    const char cipherName[] = "RC4";
//    const EVP_CIPHER *cipher;
//
//    OpenSSL_add_all_ciphers();
//    /* sifry i hashe by se nahraly pomoci OpenSSL_add_all_algorithms() */
//    cipher = EVP_get_cipherbyname(cipherName);
//    if (!cipher) {
//        printf("Sifra %s neexistuje.\n", cipherName);
//        exit(1);
//    }
//
//    int otLength = strlen((const char *) ot1);
//    int stLength = 0;
//    int tmpLength = 0;
//
//    EVP_CIPHER_CTX *ctx; // context structure
//    ctx = EVP_CIPHER_CTX_new();
//    if (ctx == NULL) exit(2);
//
//    printf("OT : %s\n", ot1);
//    /* Sifrovani */
//    res = EVP_EncryptInit_ex(ctx, cipher, NULL, key, iv);  // context init - set cipher, key, init vector
//    if (res != 1) exit(3);
//    res = EVP_EncryptUpdate(ctx, st1, &tmpLength, ot1, otLength);  // encryption of pt
//
//    if (res != 1) exit(4);
//    stLength += tmpLength;
//    res = EVP_EncryptFinal_ex(ctx, st1 + stLength, &tmpLength);  // get the remaining ct
//    if (res != 1) exit(5);
//    stLength += tmpLength;
//
//    std::cout << "ST1: ";
//    for (int i = 0; i < stLength; i++)
//        std::cout  << std::setw(2) << std::setfill('0') << std::hex << (unsigned int) st1[i];
//    std::cout << std::endl;
//
//    otLength = strlen((const char *) ot2);
//    stLength = 0;
//    tmpLength = 0;
//
//    /* Sifrovani */
//    res = EVP_EncryptInit_ex(ctx, cipher, NULL, key, iv);  // context init - set cipher, key, init vector
//    if (res != 1) exit(3);
//    res = EVP_EncryptUpdate(ctx, st2, &tmpLength, ot2, otLength);  // encryption of pt
//    if (res != 1) exit(4);
//    stLength += tmpLength;
//    res = EVP_EncryptFinal_ex(ctx, st2 + stLength, &tmpLength);  // get the remaining ct
//    if (res != 1) exit(5);
//    stLength += tmpLength;
//
//    std::cout << "ST2: ";
//    for (int i = 0; i < stLength; i++)
//        std::cout  << std::setw(2) << std::setfill('0') << std::hex << (unsigned int) st2[i];
//    std::cout << std::endl;
//
//    /* Clean up */
//    EVP_CIPHER_CTX_free(ctx);
}

int main(int argc, char *argv[]) {
//    if ((argc != 3) || ((std::string(argv[1]) != "-e") && ((std::string(argv[1]) != "-d")))) {
//        std::cout << "Spatne volani zvolte jeden prepinac (-e nebo -d)" << std::endl;
//        return 0;
//    }
//    if ((std::string(argv[2]) != "ecb") && ((std::string(argv[2]) != "cbc"))) {
//        std::cout << "Spatne volani zvolte jeden mod (ebc nebo cbc)" << std::endl;
//        return 0;
//    }
//
//    if (std::string(argv[1]) == "-e")
//        encryption();
//    if (std::string(argv[1]) == "-d")
//        decryption();

    std::string filename = "C:\\Users\\ondra\\Desktop\\BEZ\\3_assignment\\Mad_scientist.bmp";
    // open the file:
    std::ifstream file(filename, std::ios::binary);

//    std::cout << file.fail() << std::endl;

    // Stop eating new lines in binary mode!!!
    file.unsetf(std::ios::skipws);

    // get its size:
    std::streampos fileSize;

    file.seekg(0, std::ios::end);
    fileSize = file.tellg();
    file.seekg(0, std::ios::beg);



//////////////////////////////////////////////////////////////////////////////

//    // reserve capacity
    std::vector<unsigned char> vec;
    vec.reserve(fileSize);
    // read the data:
    vec.insert(vec.begin(),
               std::istream_iterator<unsigned char>(file),
               std::istream_iterator<unsigned char>());


//    std::cout << "Obsah souboru: " << std::endl;
//    int cnt = 0;
//    for(const char & character : vec) {
////        std::cout << std::hex << (unsigned int)character << " ";
//        std::cout << std::setw(10) << std::setfill(' ') << (unsigned int)character << " ";
//        if ((cnt % 10) == 0)
//            std::cout << std::endl;
//        cnt++;
//    }


    std::string tmp;

    tmp.push_back(vec[2]);
    tmp.push_back(vec[3]);
    tmp.push_back(vec[4]);
    tmp.push_back(vec[5]);


//    std::cout << "Size: ";
//    for(size_t i = 3; i <= 0; i++) {
//        std::cout << std::hex << (unsigned int)tmp[i] << " ";
//    }

//    std::cout << std::endl;

    std::ofstream out("output.bmp");

    std::cout << vec.size() << " " << fileSize << std::endl;
    for(size_t i = 0; i < vec.size(); i++) {
        out << vec[i];
    }

    out.close();

    long size = long((unsigned char)(vec[2]) << 24 |
                     (unsigned char)(vec[3]) << 16 |
                     (unsigned char)(vec[4]) << 8 |
                     (unsigned char)(vec[5]));

    long begin = long((unsigned char)(vec[10]) << 24 |
                      (unsigned char)(vec[11]) << 16 |
                      (unsigned char)(vec[12]) << 8 |
                      (unsigned char)(vec[13]));

//    std::cout << "Size: ";
//    for(size_t i = 0; i <= 3; i++) {
//        std::cout << std::hex << (unsigned int)size[i] << " ";
//    }
//    std::cout << std::endl;

    std::cout << "Size : " << std::dec << size << std::endl;
    std::cout << "Begin: " << std::dec << begin<< std::endl;


    std::cout << std::endl;


    return 0;
}
