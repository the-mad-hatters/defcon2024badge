#include "util.h"

std::string xorCrypt(const std::string &input, const std::string &key) {
    std::string output(input.size(), 0);
    for (size_t i = 0; i < input.size(); i++) {
        output[i] = input[i] ^ key[i % key.size()];
    }
    return output;
}

std::string hexToBin(const std::string &hex) {
    std::string bin;
    for (size_t i = 0; i < hex.size(); i += 2) {
        bin.push_back(std::stoi(hex.substr(i, 2), nullptr, 16));
    }
    return bin;
}