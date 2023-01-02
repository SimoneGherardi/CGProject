#pragma once
#include "defines.h"

extern VkShaderModule initializeShaderModule(const VkDevice device, const std::vector<char> code);