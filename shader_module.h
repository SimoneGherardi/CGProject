#pragma once
#include "defines.h"

extern VkShaderModule initializeShaderModule(const VkDevice device, const std::vector<char> code);
extern void cleanupShaderModule(const VkDevice device, const VkShaderModule module);