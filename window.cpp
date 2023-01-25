#include "window.h"

void initializeWindow(
    std::string const& title,
    const uint32_t width,
    const uint32_t height,
    void* pointer,
    const GLFWframebuffersizefun resizeCallback,
    GLFWwindow* window
)
{
    if (window != nullptr)
    {
        throw std::runtime_error("initialize window can be called only once; window is not a nullptr");
    }
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

    glfwSetWindowUserPointer(window, pointer);
    glfwSetFramebufferSizeCallback(window, resizeCallback);
}

void cleanupWindow(GLFWwindow* window)
{
    glfwDestroyWindow(window);
    glfwTerminate();
}