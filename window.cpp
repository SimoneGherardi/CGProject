#include "window.h"

GLFWwindow* initializeWindow(std::string const& title, const uint32_t width, const uint32_t height)
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    return glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
}