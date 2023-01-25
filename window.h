#pragma once
#include "defines.h"

extern void initializeWindow(
    std::string const& title,
    const uint32_t width,
    const uint32_t height,
    void* pointer,
    const GLFWframebuffersizefun resizeCallback,
    GLFWwindow* window
);

extern void cleanupWindow(GLFWwindow* window);