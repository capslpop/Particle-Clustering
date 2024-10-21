#pragma once

#include "../main.h"

// these are just helpful functions for using IO for files

std::string readFile(const char* filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        std::cout << "failed to open file!" << std::endl;
    }

    size_t fileSize = (size_t)file.tellg();
    std::string buffer(fileSize, ' ');

    file.seekg(0);
    file.read(&buffer[0], fileSize);

    file.close();

    return buffer;
}
