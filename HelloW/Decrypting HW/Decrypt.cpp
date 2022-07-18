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

#include "Decrypt.h"

decrypt::Decrypt::Decrypt(const std::wstring& cipherText)
{
    m_chiperText.resize(cipherText.size());
    std::copy(cipherText.begin(), cipherText.end(), m_chiperText.begin());
}

void decrypt::Decrypt::PasswordToKey(std::string& password)
{
    EVP_MD* dgst = const_cast<EVP_MD*>(EVP_get_digestbyname("MD5"));
    if (!dgst)
    {
        throw std::runtime_error("no such digest");
    }

    const unsigned char* salt = NULL;
    if (!EVP_BytesToKey(EVP_aes_128_cbc(), EVP_md5(), salt,
        reinterpret_cast<unsigned char*>(&password[0]),
        password.size(), 1, m_key, m_iv))
    {
        throw std::runtime_error("EVP_BytesToKey failed");
    }
}


bool decrypt::Decrypt::DecryptAes(std::vector<unsigned char>& m_plainText, const std::vector<unsigned char>& chipherText)
{
    bool ret = false;

    if (!chipherText.empty())
    {
        EVP_CIPHER_CTX* ctx(EVP_CIPHER_CTX_new());

        if (EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, m_key, m_iv))
        {
            m_plainText.resize(chipherText.size()); // Recovered text contracts up to block size
            int outLenFirst = (int)m_plainText.size();

            if (EVP_DecryptUpdate(ctx, (uint8_t*)&m_plainText[0], &outLenFirst, (const uint8_t*)&chipherText[0], (int)chipherText.size()))
            {
                int outLenSecond = (int)m_plainText.size() - outLenFirst;
                if (EVP_DecryptFinal_ex(ctx, (uint8_t*)&m_plainText[0] + outLenFirst, &outLenSecond))
                {
                    m_plainText.resize(outLenFirst + outLenSecond); // Set recovered text size now that we know it
                    ret = true;
                }
            }
        }
    }
    return ret;
}

void decrypt::Decrypt::DecryptCryptedFile()
{
    ReadEncryptedFile("chipher_text", m_chiperText);

    try
    {
        DecryptAes(m_plainText, m_chiperText);
    }
    catch (const std::runtime_error& ex)
    {
        std::cerr << ex.what();
    }
}

void decrypt::Decrypt::ReadEncryptedFile(const std::string& filePath, std::vector<unsigned char>& buf)
{
    std::basic_fstream<unsigned char> fileStream(filePath, std::ios::binary | std::fstream::in);
    if (!fileStream.is_open())
    {
        throw std::runtime_error("Can not open file " + filePath);
    }

    buf.clear();
    buf.insert(buf.begin(), std::istreambuf_iterator<unsigned char>(fileStream), std::istreambuf_iterator<unsigned char>());

    fileStream.close();
}

void decrypt::Decrypt::WriteEncryptedFile(const std::string& filePath, const std::vector<unsigned char>& buf)
{
    std::basic_ofstream<unsigned char> fileStream(filePath, std::ios::binary);
    fileStream.write(&buf[0], buf.size());
    fileStream.close();
}