#pragma once

#include <string>

#include "Util/Expected.h"

namespace whal {

Expected<std::string> readFile(const char* filePath);
bool isExist(const char* filePath);

}  // namespace whal
