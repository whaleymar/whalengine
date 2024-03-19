#pragma once

#include <format>
#include <fstream>
#include <sstream>
#include <string>

#include "Util/Expected.h"

namespace whal {

Expected<std::string> readFile(const char* filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return Expected<std::string>::error(std::format("FileNotFound: {}\n", filePath));
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

}  // namespace whal
