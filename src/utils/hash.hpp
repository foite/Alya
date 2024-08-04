#pragma once

#include <array>
#include <string>
#include <algorithm>
#include <openssl/evp.h>

namespace utils
{
    std::string sha256(const std::string& input)
    {
        std::string hash;
        unsigned char digest[EVP_MAX_MD_SIZE];
        unsigned int digest_len;
        EVP_MD_CTX* ctx = EVP_MD_CTX_new();
        EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr);
        EVP_DigestUpdate(ctx, input.c_str(), input.length());
        EVP_DigestFinal_ex(ctx, digest, &digest_len);
        EVP_MD_CTX_free(ctx);

        char sha256string[65];
        for (int i = 0; i < 32; i++)
        {
            sprintf(&sha256string[i * 2], "%02x", (unsigned int)digest[i]);
        }
        sha256string[64] = '\0';

        hash = std::string(sha256string);
        std::transform(hash.begin(), hash.end(), hash.begin(), ::tolower);
        return hash;
    }

    std::string md5(const std::string& input)
    {
        unsigned char digest[EVP_MAX_MD_SIZE];
        unsigned int digest_len;
        EVP_MD_CTX* ctx = EVP_MD_CTX_new();
        EVP_DigestInit_ex(ctx, EVP_md5(), nullptr);
        EVP_DigestUpdate(ctx, input.c_str(), input.length());
        EVP_DigestFinal_ex(ctx, digest, &digest_len);
        EVP_MD_CTX_free(ctx);

        char md5string[33];
        for (int i = 0; i < 16; i++)
        {
            sprintf(&md5string[i * 2], "%02x", (unsigned int)digest[i]);
        }
        md5string[32] = '\0';

        std::string hash = std::string(md5string);
        std::transform(hash.begin(), hash.end(), hash.begin(), ::toupper);
        return hash;
    }
}