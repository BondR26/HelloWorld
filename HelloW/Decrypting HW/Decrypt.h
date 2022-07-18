#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <exception>
#include <iostream>
#include <memory>
#include <type_traits>
#include <typeinfo>
#include <Windows.h>

#include "openssl/evp.h"
#include <openssl/aes.h>
#include "openssl/sha.h" 
#include "openssl/objects.h"

namespace decrypt
{

    class Decrypt
    {
    public:

        Decrypt(const std::wstring& cipherText);

        void PasswordToKey(std::string& password);

        void DecryptCryptedFile();

        bool DecryptAes(std::vector<unsigned char>& plainText, const std::vector<unsigned char>& chipherText);

        void ReadEncryptedFile(const std::string& filePath, std::vector<unsigned char>& buf);

        void WriteEncryptedFile(const std::string& filePath, const std::vector<unsigned char>& buf);
        
        std::vector<unsigned char>& getPlainText()
        {
            return m_plainText;
        }

    private:

        unsigned char m_key[EVP_MAX_KEY_LENGTH];

        unsigned char m_iv[EVP_MAX_IV_LENGTH];

        std::vector<unsigned char> m_chiperText;

        std::vector<unsigned char> m_plainText;
    };
}