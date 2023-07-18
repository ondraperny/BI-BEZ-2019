# C++ security challenges
---
Every task is written in C++ and contains makefile.\
Cryptographic functions were provided by [OpenSSL library.](https://www.openssl.org/)

 - 1_assignment - looking for random hash(SHA-256) that begin with 0xAABB
 - 2_assignment - encrypting two messages with stream cipher(RS4) and exploiting
                  weakness of using same key twice
 - 3_assignment - encrypting and decrypting .bmp pictures
 - 4_assignment - asymetric cryptography - RSA is used to exchange key for synchronous cipher that is used to encrypt file
 - 5/6_assignment - verifying certificate between client and server
 
 
Run program
---
Navigate to folder of program you want to run (assignment 1 - 5/6)

    make
    ./main
Additional parameters might be required based on which program is chosen (required parameters will be listed when attempt to run program without them).
