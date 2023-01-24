#pragma once
#include "defines.h"

extern GLFWwindow* initializeWindow(
    std::string const& title,
    const uint32_t width,
    const uint32_t height,
    void* pointer,
    const GLFWframebuffersizefun resizeCallback
);

extern void cleanupWindow(GLFWwindow* window);