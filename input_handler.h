#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "scene_elements.h"
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

extern void KeyboardInput(GLFWwindow* window, Camera camera, float delta_time);
extern void MouseInput(GLFWwindow* window, Camera camera);