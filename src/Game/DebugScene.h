#pragma once

#include <optional>

#include "Util/Expected.h"

std::optional<Error> loadDebugScene();
std::optional<Error> loadTestMap();
