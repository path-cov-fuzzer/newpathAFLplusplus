#include <iostream>
#include <openssl/sha.h>
#include <unordered_map>
#include <cassert>
#include <cstdint>

std::unordered_map<std::string, bool> hashPool;

bool hashcompare(unsigned char trace_hash[SHA256_DIGEST_LENGTH]) {
    // indicate whether this hash is unique
    bool interesting = false;

    // convert hash to C++ string
    char hash[2 * SHA256_DIGEST_LENGTH + 1];
    int total_len = 0;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        int written_bytes = snprintf((char *)(&hash[0]) + total_len, 2 * SHA256_DIGEST_LENGTH + 1 - total_len, "%02x", trace_hash[i]);
        total_len += written_bytes;
    }
    hash[2 * SHA256_DIGEST_LENGTH] = '\0';
    std::string thehashstr(hash);

    // judge whether thehashstr is in hashPool
    // if thehashstr does not exist in hashPool, then it is interesting
    if (hashPool.find(thehashstr) == hashPool.end()) {
        interesting = true;
        hashPool[thehashstr] = true;
    }

    // unique hash, return true. duplicate hash, return false
    return interesting;

}

extern "C" {

#include <stdbool.h>

// interface to C language
bool c_hashcompare(unsigned char trace_hash[SHA256_DIGEST_LENGTH]) {
    return hashcompare(trace_hash);
}

}


