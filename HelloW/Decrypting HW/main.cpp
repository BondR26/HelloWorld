#include <string>
#include <vector>
#include <fstream>
#include <exception>
#include <iostream>
#include <memory>

#include "openssl/evp.h"
#include <openssl/aes.h>
#include "openssl/sha.h"
#include "openssl/objects.h"

#include "Decrypt.h"

int main()
{
    std::string pass = "pass";
    setlocale(LC_ALL, "Russian");
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    OpenSSL_add_all_algorithms();

    decrypt::Decrypt decrypt(L"cipher_text");

    try
    {
        decrypt.PasswordToKey(pass);
        decrypt.DecryptCryptedFile();
    }
    catch (const std::runtime_error& ex)
    {
        std::cerr << ex.what();
    }

    decrypt.WriteEncryptedFile("plain_text.txt", decrypt.getPlainText());
}