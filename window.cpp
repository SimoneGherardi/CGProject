#include "window.h"

GLFWwindow* initializeWindow(
    std::string const& title,
    const uint32_t width,
    const uint32_t height,
    void* pointer,
    const GLFWframebuffersizefun resizeCallback
)
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    auto window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

    glfwSetWindowUserPointer(window, pointer);
    glfwSetFramebufferSizeCallback(window, resizeCallback);

    return window;
}

void cleanupWindow(GLFWwindow* window)
{
    glfwDestroyWindow(window);
    glfwTerminate();
}