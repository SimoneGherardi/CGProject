#pragma once
#include "defines.h"

typedef std::vector<const char*> ValidationLayers;
extern bool checkValidationLayersSupport(const ValidationLayers validationLayers);