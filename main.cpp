#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

#include <flecs.h>
#include "test.h"

#include <cstdint> // Necessary for uint32_t
#include <limits> // Necessary for std::numeric_limits
#include <algorithm> // Necessary for std::clamp

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <optional>
#include <set>
#include <array>
#include <map>
#include <unordered_map>
#include <fstream>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <chrono>

#include <reactphysics3d/reactphysics3d.h>
#include <flecs.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NO_INCLUDE_STB_IMAGE
#include <tiny_gltf.h>

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#ifndef _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_NONSTDC_NO_DEPRECATE
#endif

const uint32_t WIDTH = 640;
const uint32_t HEIGHT = 480;

const std::string MODEL_PATH = "resources/models/";
const std::string TEXTURE_PATH = "resources/textures/";
const std::string SHADER_PATH = "resources/shaders/";

const int MAX_FRAMES_IN_FLIGHT = 2;

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

enum ModelType { OBJ, GLTF };

struct Model {
    const char* ObjFile;
    ModelType type;
    const char* TextureFile;
    const glm::vec3 pos;
    const float scale;
};

const std::vector<Model> SceneToLoad = {
    {"Sphere.obj", OBJ, "Plaster.png", {0,0.0, 0.0}, 1.0},
    {"Sphere.obj", OBJ, "Ball15.png", {0,0.0, 0.0}, 1.0},
    {"Sphere.obj", OBJ, "soccer_sph.png", {0,0.0, 0.0}, 1.0},
    {"Sphere.obj", OBJ, "Moon.jpg", {0,0.0, 0.0}, 1.0}
};

struct UniformBufferObject {
    alignas(16) glm::mat4 mvpMat;
    alignas(16) glm::mat4 mMat;
    alignas(16) glm::mat4 nMat;
};

struct GlobalUniformBufferObject {
    alignas(16) glm::vec3 lightDir0;
    alignas(16) glm::vec3 lightColor0;
    alignas(16) glm::vec3 lightDir1;
    alignas(16) glm::vec3 lightColor1;
    alignas(16) glm::vec3 lightDir2;
    alignas(16) glm::vec3 lightColor2;
    alignas(16) glm::vec3 lightDir3;
    alignas(16) glm::vec3 lightColor3;
    alignas(16) glm::vec3 eyePos;
    alignas(16) glm::vec4 selector;
};

struct SkyBoxModel {
    const char* ObjFile;
    ModelType type;
    const char* TextureFile[6];
};

const std::vector<SkyBoxModel> SkyBoxToLoad = {
    {"SkyBoxCube.obj", OBJ, {"sky1/posx.jpg", "sky1/negx.jpg", "sky1/posy.jpg", "sky1/negy.jpg", "sky1/posz.jpg", "sky1/negz.jpg"}},
    {"SkyBoxCube.obj", OBJ, {"sky2/posx.jpg", "sky2/negx.jpg", "sky2/posy.jpg", "sky2/negy.jpg", "sky2/posz.jpg", "sky2/negz.jpg"}},
    {"SkyBoxCube.obj", OBJ, {"sky3/ToonSkybox-2-1.png", "sky3/ToonSkybox-0-1.png", "sky3/ToonSkybox-1-0.png", "sky3/ToonSkybox-1-2.png", "sky3/ToonSkybox-1-1.png", "sky3/ToonSkybox-3-1.png"}},
    {"SkyBoxCube.obj", OBJ, {"sky4/bkg1_right.png", "sky4/bkg1_left.png", "sky4/bkg1_top.png", "sky4/bkg1_bot.png", "sky4/bkg1_front.png", "sky4/bkg1_back.png"}}
};


const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

struct errorcode {
    VkResult resultCode;
    std::string meaning;
}
ErrorCodes[] = {
    { VK_NOT_READY, "Not Ready" },
    { VK_TIMEOUT, "Timeout" },
    { VK_EVENT_SET, "Event Set" },
    { VK_EVENT_RESET, "Event Reset" },
    { VK_INCOMPLETE, "Incomplete" },
    { VK_ERROR_OUT_OF_HOST_MEMORY, "Out of Host Memory" },
    { VK_ERROR_OUT_OF_DEVICE_MEMORY, "Out of Device Memory" },
    { VK_ERROR_INITIALIZATION_FAILED, "Initialization Failed" },
    { VK_ERROR_DEVICE_LOST, "Device Lost" },
    { VK_ERROR_MEMORY_MAP_FAILED, "Memory Map Failed" },
    { VK_ERROR_LAYER_NOT_PRESENT, "Layer Not Present" },
    { VK_ERROR_EXTENSION_NOT_PRESENT, "Extension Not Present" },
    { VK_ERROR_FEATURE_NOT_PRESENT, "Feature Not Present" },
    { VK_ERROR_INCOMPATIBLE_DRIVER, "Incompatible Driver" },
    { VK_ERROR_TOO_MANY_OBJECTS, "Too Many Objects" },
    { VK_ERROR_FORMAT_NOT_SUPPORTED, "Format Not Supported" },
    { VK_ERROR_FRAGMENTED_POOL, "Fragmented Pool" },
    { VK_ERROR_SURFACE_LOST_KHR, "Surface Lost" },
    { VK_ERROR_NATIVE_WINDOW_IN_USE_KHR, "Native Window in Use" },
    { VK_SUBOPTIMAL_KHR, "Suboptimal" },
    { VK_ERROR_OUT_OF_DATE_KHR, "Error Out of Date" },
    { VK_ERROR_INCOMPATIBLE_DISPLAY_KHR, "Incompatible Display" },
    { VK_ERROR_VALIDATION_FAILED_EXT, "Valuidation Failed" },
    { VK_ERROR_INVALID_SHADER_NV, "Invalid Shader" },
    { VK_ERROR_OUT_OF_POOL_MEMORY_KHR, "Out of Pool Memory" },
    { VK_ERROR_INVALID_EXTERNAL_HANDLE, "Invalid External Handle" },

};

void PrintVkError(VkResult result) {
    const int numErrorCodes = sizeof(ErrorCodes) / sizeof(struct errorcode);
    std::string meaning = "";
    for (int i = 0; i < numErrorCodes; i++) {
        if (result == ErrorCodes[i].resultCode) {
            meaning = ErrorCodes[i].meaning;
            break;
        }
    }
    std::cout << "Error: " << result << ", " << meaning << "\n";
}

struct VertexDescriptor {
    bool hasPos;
    bool hasNormal;
    bool hasTexCoord;
    bool hasColor;
    bool hasTangent;

    int deltaPos;
    int deltaNormal;
    int deltaTexCoord;
    int deltaColor;
    int deltaTangent;

    int locPos;
    int locNormal;
    int locTexCoord;
    int locColor;
    int locTangent;

    int size;
    int loc;

    VertexDescriptor(bool hPos, bool hNormal, bool hTexCoord, bool hColor, bool hTangent) {
        size = 0;
        loc = 0;

        hasPos = hPos;
        hasNormal = hNormal;
        hasTexCoord = hTexCoord;
        hasColor = hColor;
        hasTangent = hTangent;

        if (hasPos) { deltaPos = size; size += 3; locPos = loc; loc++; }
        else { deltaPos = -1; locPos = -1; }
        if (hasNormal) { deltaNormal = size; size += 3; locNormal = loc; loc++; }
        else { deltaNormal = -1; locNormal = -1; }
        if (hasTexCoord) { deltaTexCoord = size; size += 2; locTexCoord = loc; loc++; }
        else { deltaTexCoord = -1; locTexCoord = -1; }
        if (hasColor) { deltaColor = size; size += 4; locColor = loc; loc++; }
        else { deltaColor = -1; locColor = -1; }
        if (hasTangent) { deltaTangent = size; size += 4; locTangent = loc; loc++; }
        else { deltaTangent = -1; locTangent = -1; }
    }


    VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = size * sizeof(float);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    std::vector<VkVertexInputAttributeDescription>
        getAttributeDescriptions(int binding = 0) {
        std::vector<VkVertexInputAttributeDescription>
            attributeDescriptions{};
        attributeDescriptions.resize(loc);
        if (hasPos) {
            attributeDescriptions[locPos].binding = binding;
            attributeDescriptions[locPos].location = locPos;
            attributeDescriptions[locPos].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[locPos].offset = deltaPos * sizeof(float);
        }

        if (hasNormal) {
            attributeDescriptions[locNormal].binding = binding;
            attributeDescriptions[locNormal].location = locNormal;
            attributeDescriptions[locNormal].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[locNormal].offset = deltaNormal * sizeof(float);
        }

        if (hasTexCoord) {
            attributeDescriptions[locTexCoord].binding = binding;
            attributeDescriptions[locTexCoord].location = locTexCoord;
            attributeDescriptions[locTexCoord].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[locTexCoord].offset = deltaTexCoord * sizeof(float);
        }

        if (hasColor) {
            attributeDescriptions[locColor].binding = binding;
            attributeDescriptions[locColor].location = locColor;
            attributeDescriptions[locColor].format = VK_FORMAT_R32G32B32A32_SFLOAT;
            attributeDescriptions[locColor].offset = deltaColor * sizeof(float);
        }

        if (hasTangent) {
            attributeDescriptions[locTangent].binding = binding;
            attributeDescriptions[locTangent].location = locTangent;
            attributeDescriptions[locTangent].format = VK_FORMAT_R32G32B32A32_SFLOAT;
            attributeDescriptions[locTangent].offset = deltaTangent * sizeof(float);
        }

        return attributeDescriptions;
    }

    glm::vec3 getPos(float* data, int i) {
        if (hasPos) {
            return glm::vec3(data[i * size + deltaPos], data[i * size + deltaPos + 1], data[i * size + deltaPos + 2]);
        }
        else {
            return glm::vec3(0.0f);
            std::cerr << "Vertex has no position \n";
        }
    }

    void setPos(float* data, int i, glm::vec3 pos) {
        if (hasPos) {
            data[i * size + deltaPos] = pos.x;
            data[i * size + deltaPos + 1] = pos.y;
            data[i * size + deltaPos + 2] = pos.z;
        }
        else {
            std::cerr << "Vertex has no position \n";
        }
    }

    glm::vec3 getNormal(float* data, int i) {
        if (hasPos) {
            return glm::vec3(data[i * size + deltaNormal], data[i * size + deltaNormal + 1], data[i * size + deltaNormal + 2]);
        }
        else {
            return glm::vec3(0.0f);
            std::cerr << "Vertex has no normal \n";
        }
    }

    void setNormal(float* data, int i, glm::vec3 norm) {
        if (hasNormal) {
            data[i * size + deltaNormal] = norm.x;
            data[i * size + deltaNormal + 1] = norm.y;
            data[i * size + deltaNormal + 2] = norm.z;
        }
        else {
            std::cerr << "Vertex has no normal \n";
        }
    }

    glm::vec2 getTexCoord(float* data, int i) {
        if (hasPos) {
            return glm::vec2(data[i * size + deltaTexCoord], data[i * size + deltaTexCoord + 1]);
        }
        else {
            return glm::vec2(0.0f);
            std::cerr << "Vertex has no UV \n";
        }
    }

    void setTexCoord(float* data, int i, glm::vec3 uv) {
        if (hasNormal) {
            data[i * size + deltaTexCoord] = uv.x;
            data[i * size + deltaTexCoord + 1] = uv.y;
        }
        else {
            std::cerr << "Vertex has no UV \n";
        }
    }
};

struct ModelData {
    VertexDescriptor* vertDesc;
    std::vector<float> vertices;
    std::vector<uint32_t> indices;
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;
};

struct TextureData {
    VkImage textureImage;
    VkDeviceMemory textureImageMemory;
    VkImageView textureImageView;
    VkSampler textureSampler;
    uint32_t mipLevels;
};

struct SceneModel {
    // Model data
    ModelData MD;

    // Texture data
    TextureData TD;
};

class CGProject {
public:
    void run() {
        initWindow();
        initVulkan();
        initApp();
        mainLoop();
        cleanup();
    }

private:
    // place all the application variables here. As an example, here the one for the GLFW Window
    // and the Vulkan instances are already defined.

    GLFWwindow* window;
    VkInstance instance;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;
    VkQueue graphicsQueue;

    VkSurfaceKHR surface;
    VkQueue presentQueue;
    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;
    VkSwapchainKHR swapChain;

    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;

    std::vector<VkImageView> swapChainImageViews;
    VkRenderPass renderPass;

    VkImage depthImage;
    VkDeviceMemory depthImageMemory;
    VkImageView depthImageView;

    VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
    VkImage colorImage;
    VkDeviceMemory colorImageMemory;
    VkImageView colorImageView;

    std::vector<VkFramebuffer> swapChainFramebuffers;
    std::vector<VkCommandBuffer> commandBuffers;
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    std::vector<VkFence> imagesInFlight;
    size_t currentFrame = 0;

    VkDescriptorPool descriptorPool;

    VertexDescriptor 	phongAndSkyBoxVertices = VertexDescriptor(true, true, true, false, false);

    // Wireframe pipeline
    VkDescriptorSetLayout WireframeDescriptorSetLayout;
    VkPipelineLayout WireframePipelineLayout;
    VkPipeline WireframePipeline;

    std::vector<VkBuffer> WireframeUniformBuffers;
    std::vector<VkDeviceMemory> WireframeUniformBuffersMemory;

    std::vector<VkBuffer> WireframeGlobalUniformBuffers;
    std::vector<VkDeviceMemory> WireframeGlobalUniformBuffersMemory;

    std::vector<VkDescriptorSet> WireframeDescriptorSets;

    // Phong pipeline
    VkDescriptorSetLayout PhongDescriptorSetLayout;
    VkPipelineLayout PhongPipelineLayout;
    VkPipeline PhongPipeline;
    
    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    
    std::vector<VkBuffer> globalUniformBuffers;
    std::vector<VkDeviceMemory> globalUniformBuffersMemory;
    
    std::vector<VkDescriptorSet> PhongDescriptorSets;

    std::vector<SceneModel> Scene;
    

    //  Skybox pipeline
    VkDescriptorSetLayout SkyBoxDescriptorSetLayout; // for skybox
    VkPipelineLayout SkyBoxPipelineLayout;	// for skybox
    VkPipeline SkyBoxPipeline;

    std::vector<VkBuffer> SkyBoxUniformBuffers;
    std::vector<VkDeviceMemory> SkyBoxUniformBuffersMemory;
    
    std::vector<VkDescriptorSet> SkyBoxDescriptorSets;
    
    std::vector<SceneModel> SkyBox;

    // Camera
    glm::vec3 CamAng = glm::vec3(0.0f);
    glm::vec3 CamPos = glm::vec3(0.0f, 0.5f, 5.0f);

    // Other global variables
    int curText = 0;

    void initWindow() {

        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        window = glfwCreateWindow(WIDTH, HEIGHT, "CG Project", nullptr, nullptr);
    }

    void initVulkan() {
        createInstance();
        createSurface();
        pickPhysicalDevice();
        createLogicalDevice();
        createSwapChain();
        createImageViews();
        createRenderPass();
        createDescriptorSetLayouts();
        createPipelines();
        createCommandPool();
        createColorResources();
        createDepthResources();
        createFramebuffers();
        
        loadModels();
        createUniformBuffers();
        createDescriptorPool();
        createDescriptorSets();

        createCommandBuffers();

        createSyncObjects();

    }
   

    void createInstance() {
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;

        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        createInfo.enabledExtensionCount = glfwExtensionCount;
        createInfo.ppEnabledExtensionNames = glfwExtensions;

        createInfo.enabledLayerCount = 0;

        VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);

        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
            throw std::runtime_error("failed to create instance!");
        }
    }

    void createSurface() {
        if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface!");
        }
    }

    bool isDeviceSuitable(VkPhysicalDevice device) {
        QueueFamilyIndices indices = findQueueFamilies(device);

        bool extensionsSupported = checkDeviceExtensionSupport(device);

        bool swapChainAdequate = false;
        if (extensionsSupported) {
            SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }

        return indices.isComplete() && extensionsSupported && swapChainAdequate;
    }

    bool checkDeviceExtensionSupport(VkPhysicalDevice device) {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

        for (const auto& extension : availableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    void pickPhysicalDevice() {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        if (deviceCount == 0) {
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }

        std::multimap<int, VkPhysicalDevice> candidates;

        for (const auto& device : devices) {
            int score = rateDeviceSuitability(device);
            candidates.insert(std::make_pair(score, device));
        }

        for (const auto& device : devices) {
            if (isDeviceSuitable(device)) {
                physicalDevice = device;
                break;
            }
        }

        if (physicalDevice == VK_NULL_HANDLE) {
            throw std::runtime_error("failed to find a suitable GPU!");
        }
    }

    int rateDeviceSuitability(VkPhysicalDevice device) {
        int score = 0;
        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        // Discrete GPUs have a significant performance advantage
        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            score += 1000;
        }

        // Maximum possible size of textures affects graphics quality
        score += deviceProperties.limits.maxImageDimension2D;

        // Application can't function without geometry shaders
        if (!deviceFeatures.geometryShader) {
            return 0;
        }
        return score;
    }

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const auto& queueFamily : queueFamilies) {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = i;
            }

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

            if (presentSupport) {
                indices.presentFamily = i;
            }

            if (indices.isComplete()) {
                break;
            }

            i++;
        }

        return indices;
    }

    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device) {
        SwapChainSupportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
        }

        return details;
    }

    void createLogicalDevice() {
        QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures{};

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();

        createInfo.pEnabledFeatures = &deviceFeatures;

        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();

        createInfo.enabledLayerCount = 0;

        if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
            throw std::runtime_error("failed to create logical device!");
        }

        vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
        vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
    }

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }

    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return availablePresentMode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        }
        else {
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);

            VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };

            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }

    void createSwapChain() {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = surface;

        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
        uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

        if (indices.graphicsFamily != indices.presentFamily) {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0; // Optional
            createInfo.pQueueFamilyIndices = nullptr; // Optional
        }

        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;

        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;

        createInfo.oldSwapchain = VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
            throw std::runtime_error("failed to create swap chain!");
        }

        vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
        swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

        swapChainImageFormat = surfaceFormat.format;
        swapChainExtent = extent;
    }

    void createImageViews() {
        swapChainImageViews.resize(swapChainImages.size());

        for (size_t i = 0; i < swapChainImages.size(); i++) {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = swapChainImages[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = swapChainImageFormat;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create image views!");
            }
        }
    }

    void createRenderPass() {
        VkAttachmentDescription colorAttachmentResolve{};
        colorAttachmentResolve.format = swapChainImageFormat;
        colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentResolveRef{};
        colorAttachmentResolveRef.attachment = 2;
        colorAttachmentResolveRef.layout =
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription depthAttachment{};
        depthAttachment.format = findDepthFormat();
        depthAttachment.samples = msaaSamples;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout =
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout =
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = swapChainImageFormat;
        colorAttachment.samples = msaaSamples;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout =
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        subpass.pDepthStencilAttachment = &depthAttachmentRef;
        subpass.pResolveAttachments = &colorAttachmentResolveRef;

        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        std::array<VkAttachmentDescription, 3> attachments =
        { colorAttachment, depthAttachment,
         colorAttachmentResolve };

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());;
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        VkResult result = vkCreateRenderPass(device, &renderPassInfo, nullptr,
            &renderPass);
        if (result != VK_SUCCESS) {
            PrintVkError(result);
            throw std::runtime_error("failed to create render pass!");
        }
    }

    VkFormat findSupportedFormat(const std::vector<VkFormat> candidates,
        VkImageTiling tiling, VkFormatFeatureFlags features) {
        for (VkFormat format : candidates) {
            VkFormatProperties props;

            vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);
            if (tiling == VK_IMAGE_TILING_LINEAR &&
                (props.linearTilingFeatures & features) == features) {
                return format;
            }
            else if (tiling == VK_IMAGE_TILING_OPTIMAL &&
                (props.optimalTilingFeatures & features) == features) {
                return format;
            }
        }

        throw std::runtime_error("failed to find supported format!");
    }

    VkFormat findDepthFormat() {
        return findSupportedFormat({ VK_FORMAT_D32_SFLOAT,
                                    VK_FORMAT_D32_SFLOAT_S8_UINT,
                                    VK_FORMAT_D24_UNORM_S8_UINT },
            VK_IMAGE_TILING_OPTIMAL,
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
    }

    void initApp() {
        // This will not be needed in this assignment!
    }

    void mainLoop() {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
        }
    }

    void cleanup() {
        vkDestroyDescriptorSetLayout(device, PhongDescriptorSetLayout, nullptr);
        vkDestroyDescriptorSetLayout(device, SkyBoxDescriptorSetLayout, nullptr);

        vkDestroyCommandPool(device, commandPool, nullptr);

        vkDestroyDevice(device, nullptr);

        vkDestroySurfaceKHR(instance, surface, nullptr);
        vkDestroyInstance(instance, nullptr);

        glfwDestroyWindow(window);

        glfwTerminate();
    }

    void createCommandPool() {
        QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

        if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create command pool!");
        }
    }

    void createCommandBuffers() {
        commandBuffers.resize(swapChainFramebuffers.size());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

        VkResult result = vkAllocateCommandBuffers(device, &allocInfo,
            commandBuffers.data());
        if (result != VK_SUCCESS) {
            PrintVkError(result);
            throw std::runtime_error("failed to allocate command buffers!");
        }

        for (size_t i = 0; i < commandBuffers.size(); i++) {
            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = 0; // Optional
            beginInfo.pInheritanceInfo = nullptr; // Optional

            if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) !=
                VK_SUCCESS) {
                throw std::runtime_error("failed to begin recording command buffer!");
            }

            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = renderPass;
            renderPassInfo.framebuffer = swapChainFramebuffers[i];
            renderPassInfo.renderArea.offset = { 0, 0 };
            renderPassInfo.renderArea.extent = swapChainExtent;

            std::array<VkClearValue, 2> clearValues{};
            clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
            clearValues[1].depthStencil = { 1.0f, 0 };

            renderPassInfo.clearValueCount =
                static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data();

            vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo,
                VK_SUBPASS_CONTENTS_INLINE);

            vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
                PhongPipeline);
            //			for(int j = 0; j < Scene.size(); j++) {
            {int j = curText;
            VkBuffer vertexBuffers[] = { Scene[j].MD.vertexBuffer };
            VkDeviceSize offsets[] = { 0 };
            vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);
            vkCmdBindIndexBuffer(commandBuffers[i], Scene[j].MD.indexBuffer, 0,
                VK_INDEX_TYPE_UINT32);
            vkCmdBindDescriptorSets(commandBuffers[i],
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                PhongPipelineLayout, 0, 1,
                &PhongDescriptorSets[i + j * swapChainImages.size()],
                0, nullptr);

            vkCmdDrawIndexed(commandBuffers[i],
                static_cast<uint32_t>(Scene[j].MD.indices.size()), 1, 0, 0, 0);
            }

            // Draws the Skybox
            vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
                SkyBoxPipeline);
            VkBuffer vertexBuffers[] = { SkyBox[curText].MD.vertexBuffer };
            VkDeviceSize offsets[] = { 0 };
            vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);
            vkCmdBindIndexBuffer(commandBuffers[i], SkyBox[curText].MD.indexBuffer, 0,
                VK_INDEX_TYPE_UINT32);
            vkCmdBindDescriptorSets(commandBuffers[i],
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                SkyBoxPipelineLayout, 0, 1,
                &SkyBoxDescriptorSets[i + curText * swapChainImages.size()],
                0, nullptr);
            vkCmdDrawIndexed(commandBuffers[i],
                static_cast<uint32_t>(SkyBox[curText].MD.indices.size()), 1, 0, 0, 0);

        }
    }

    void createDescriptorSetLayouts() {
        createWireframeDescriptorSetLayout();
        createPhongDescriptorSetLayout();
        createSkyBoxDescriptorSetLayout();
    }

    void createWireframeDescriptorSetLayout() {
        VkDescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        uboLayoutBinding.pImmutableSamplers = nullptr;

        VkDescriptorSetLayoutBinding samplerLayoutBinding{};
        samplerLayoutBinding.binding = 1;
        samplerLayoutBinding.descriptorType =
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerLayoutBinding.descriptorCount = 1;
        samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        samplerLayoutBinding.pImmutableSamplers = nullptr;

        VkDescriptorSetLayoutBinding globalUboLayoutBinding{};
        globalUboLayoutBinding.binding = 2;
        globalUboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        globalUboLayoutBinding.descriptorCount = 1;
        globalUboLayoutBinding.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
        globalUboLayoutBinding.pImmutableSamplers = nullptr;

        std::array<VkDescriptorSetLayoutBinding, 3> bindings =
        { uboLayoutBinding, samplerLayoutBinding, globalUboLayoutBinding };

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        layoutInfo.pBindings = bindings.data();

        VkResult result = vkCreateDescriptorSetLayout(device, &layoutInfo,
            nullptr, &WireframeDescriptorSetLayout);
        if (result != VK_SUCCESS) {
            PrintVkError(result);
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }

    void createPhongDescriptorSetLayout() {
        VkDescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        uboLayoutBinding.pImmutableSamplers = nullptr;

        VkDescriptorSetLayoutBinding samplerLayoutBinding{};
        samplerLayoutBinding.binding = 1;
        samplerLayoutBinding.descriptorType =
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerLayoutBinding.descriptorCount = 1;
        samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        samplerLayoutBinding.pImmutableSamplers = nullptr;

        VkDescriptorSetLayoutBinding globalUboLayoutBinding{};
        globalUboLayoutBinding.binding = 2;
        globalUboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        globalUboLayoutBinding.descriptorCount = 1;
        globalUboLayoutBinding.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
        globalUboLayoutBinding.pImmutableSamplers = nullptr;

        std::array<VkDescriptorSetLayoutBinding, 3> bindings =
        { uboLayoutBinding, samplerLayoutBinding, globalUboLayoutBinding };

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        layoutInfo.pBindings = bindings.data();

        VkResult result = vkCreateDescriptorSetLayout(device, &layoutInfo,
            nullptr, &PhongDescriptorSetLayout);
        if (result != VK_SUCCESS) {
            PrintVkError(result);
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }

    void createSkyBoxDescriptorSetLayout() {
        VkDescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        uboLayoutBinding.pImmutableSamplers = nullptr;

        VkDescriptorSetLayoutBinding samplerLayoutBinding{};
        samplerLayoutBinding.binding = 1;
        samplerLayoutBinding.descriptorType =
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerLayoutBinding.descriptorCount = 1;
        samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        samplerLayoutBinding.pImmutableSamplers = nullptr;

        std::array<VkDescriptorSetLayoutBinding, 2> bindings =
        { uboLayoutBinding, samplerLayoutBinding };

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        layoutInfo.pBindings = bindings.data();

        VkResult result = vkCreateDescriptorSetLayout(device, &layoutInfo,
            nullptr, &SkyBoxDescriptorSetLayout);
        if (result != VK_SUCCESS) {
            PrintVkError(result);
            throw std::runtime_error("failed to create SkyBox descriptor set layout!");
        }
    }

    void createPipelines() {
        createWireframePipeline();
        createPhongPipeline();
        createSkyBoxPipeline();
    }

    void createWireframePipeline() {
        createPipeline("WireframeVert.spv", "WireframeFrag.spv",
            WireframePipelineLayout, WireframePipeline,
            WireframeDescriptorSetLayout, VK_COMPARE_OP_LESS, VK_POLYGON_MODE_LINE,
            VK_CULL_MODE_NONE, false, phongAndSkyBoxVertices);
    }

    void createPhongPipeline() {
        createPipeline("PhongVert.spv", "PhongFrag.spv",
            PhongPipelineLayout, PhongPipeline,
            PhongDescriptorSetLayout, VK_COMPARE_OP_LESS, VK_POLYGON_MODE_FILL,
            VK_CULL_MODE_NONE, false, phongAndSkyBoxVertices);
    }

    void createSkyBoxPipeline() {
        createPipeline("SkyBoxVert.spv", "SkyBoxFrag.spv",
            SkyBoxPipelineLayout, SkyBoxPipeline,
            SkyBoxDescriptorSetLayout, VK_COMPARE_OP_LESS_OR_EQUAL, VK_POLYGON_MODE_FILL,
            VK_CULL_MODE_BACK_BIT, false, phongAndSkyBoxVertices);
    }

    void createPipeline(const char* VertexShaderName, const char* FragShaderName,
                        VkPipelineLayout& PipelineLayout,
                        VkPipeline& Pipeline,
                        VkDescriptorSetLayout& descriptorSetLayout,
                        VkCompareOp compareOp, VkPolygonMode polyModel,
                        VkCullModeFlagBits CM, bool transp,
                        VertexDescriptor& VD) {
        auto vertShaderCode = readFile((SHADER_PATH + VertexShaderName).c_str());
        auto fragShaderCode = readFile((SHADER_PATH + FragShaderName).c_str());

        VkShaderModule vertShaderModule =
            createShaderModule(vertShaderCode);
        VkShaderModule fragShaderModule =
            createShaderModule(fragShaderCode);

        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType =
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = vertShaderModule;
        vertShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType =
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragShaderModule;
        fragShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo shaderStages[] =
        { vertShaderStageInfo, fragShaderStageInfo };

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType =
            VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        auto bindingDescription = VD.getBindingDescription();
        auto attributeDescriptions = VD.getAttributeDescriptions();

        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.vertexAttributeDescriptionCount =
            static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.pVertexAttributeDescriptions =
            attributeDescriptions.data();

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType =
            VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)swapChainExtent.width;
        viewport.height = (float)swapChainExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor{};
        scissor.offset = { 0, 0 };
        scissor.extent = swapChainExtent;

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType =
            VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType =
            VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = polyModel;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = CM;
        rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;
        rasterizer.depthBiasConstantFactor = 0.0f; // Optional
        rasterizer.depthBiasClamp = 0.0f; // Optional
        rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType =
            VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_TRUE;
        multisampling.minSampleShading = .2f;
        multisampling.rasterizationSamples = msaaSamples;
        multisampling.minSampleShading = 1.0f; // Optional
        multisampling.pSampleMask = nullptr; // Optional
        multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
        multisampling.alphaToOneEnable = VK_FALSE; // Optional

        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT |
            VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT |
            VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = transp ? VK_TRUE : VK_FALSE;
        colorBlendAttachment.srcColorBlendFactor =
            transp ? VK_BLEND_FACTOR_SRC_ALPHA : VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstColorBlendFactor =
            transp ? VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA : VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.colorBlendOp =
            VK_BLEND_OP_ADD; // Optional
        colorBlendAttachment.srcAlphaBlendFactor =
            VK_BLEND_FACTOR_ONE; // Optional
        colorBlendAttachment.dstAlphaBlendFactor =
            VK_BLEND_FACTOR_ZERO; // Optional
        colorBlendAttachment.alphaBlendOp =
            VK_BLEND_OP_ADD; // Optional

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType =
            VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f; // Optional
        colorBlending.blendConstants[1] = 0.0f; // Optional
        colorBlending.blendConstants[2] = 0.0f; // Optional
        colorBlending.blendConstants[3] = 0.0f; // Optional

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType =
            VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
        pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
        pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

        VkResult result = vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr,
            &PipelineLayout);
        if (result != VK_SUCCESS) {
            PrintVkError(result);
            throw std::runtime_error("failed to create pipeline layout!");
        }

        VkPipelineDepthStencilStateCreateInfo depthStencil{};
        depthStencil.sType =
            VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.depthTestEnable = VK_TRUE;
        depthStencil.depthWriteEnable = VK_TRUE;
        depthStencil.depthCompareOp = compareOp;
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.minDepthBounds = 0.0f; // Optional
        depthStencil.maxDepthBounds = 1.0f; // Optional
        depthStencil.stencilTestEnable = VK_FALSE;
        depthStencil.front = {}; // Optional
        depthStencil.back = {}; // Optional

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType =
            VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = &depthStencil;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = nullptr; // Optional
        pipelineInfo.layout = PipelineLayout;
        pipelineInfo.renderPass = renderPass;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
        pipelineInfo.basePipelineIndex = -1; // Optional

        result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1,
            &pipelineInfo, nullptr, &Pipeline);
        if (result != VK_SUCCESS) {
            PrintVkError(result);
            throw std::runtime_error("failed to create graphics pipeline!");
        }

        vkDestroyShaderModule(device, fragShaderModule, nullptr);
        vkDestroyShaderModule(device, vertShaderModule, nullptr);
    }

    VkShaderModule createShaderModule(const std::vector<char>& code) {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        VkShaderModule shaderModule;

        VkResult result = vkCreateShaderModule(device, &createInfo, nullptr,
            &shaderModule);
        if (result != VK_SUCCESS) {
            PrintVkError(result);
            throw std::runtime_error("failed to create shader module!");
        }

        return shaderModule;
    }

    static std::vector<char> readFile(const std::string& filename) {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("failed to open file!");
        }

        size_t fileSize = (size_t)file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();

        return buffer;
    }

    void createColorResources() {
        VkFormat colorFormat = swapChainImageFormat;
        createImage(swapChainExtent.width, swapChainExtent.height, 1,
            msaaSamples, colorFormat, VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT |
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            colorImage, colorImageMemory);
        colorImageView = createImageView(colorImage, colorFormat,
            VK_IMAGE_ASPECT_COLOR_BIT, 1,
            VK_IMAGE_VIEW_TYPE_2D, 1);
    }

    void createImage(uint32_t width, uint32_t height, uint32_t mipLevels,
        VkSampleCountFlagBits numSamples, VkFormat format,
        VkImageTiling tiling, VkImageUsageFlags usage,
        VkMemoryPropertyFlags properties, VkImage& image,
        VkDeviceMemory& imageMemory) {
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = width;
        imageInfo.extent.height = height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = mipLevels;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format;
        imageInfo.tiling = tiling;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.samples = numSamples;
        imageInfo.flags = 0; // Optional

        VkResult result = vkCreateImage(device, &imageInfo, nullptr, &image);
        if (result != VK_SUCCESS) {
            PrintVkError(result);
            throw std::runtime_error("failed to create image!");
        }

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(device, image, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits,
            properties);
        if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to allocate image memory!");
        }

        vkBindImageMemory(device, image, imageMemory, 0);
    }

    uint32_t findMemoryType(uint32_t typeFilter,
        VkMemoryPropertyFlags properties) {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) &&
                (memProperties.memoryTypes[i].propertyFlags & properties) ==
                properties) {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }

    VkImageView createImageView(VkImage image, VkFormat format,
        VkImageAspectFlags aspectFlags,
        uint32_t mipLevels, VkImageViewType type, int layerCount) {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = image;
        viewInfo.viewType = type;
        viewInfo.format = format;
        viewInfo.subresourceRange.aspectMask = aspectFlags;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = mipLevels;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = layerCount;
        VkImageView imageView;

        VkResult result = vkCreateImageView(device, &viewInfo, nullptr,
            &imageView);
        if (result != VK_SUCCESS) {
            PrintVkError(result);
            throw std::runtime_error("failed to create image view!");
        }
        return imageView;
    }

    void createDepthResources() {
        VkFormat depthFormat = findDepthFormat();

        createImage(swapChainExtent.width, swapChainExtent.height, 1,
            msaaSamples, depthFormat, VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            depthImage, depthImageMemory);
        depthImageView = createImageView(depthImage, depthFormat,
            VK_IMAGE_ASPECT_DEPTH_BIT, 1,
            VK_IMAGE_VIEW_TYPE_2D, 1);

        transitionImageLayout(depthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1, 1);
    }

    void transitionImageLayout(VkImage image, VkFormat format,
        VkImageLayout oldLayout, VkImageLayout newLayout,
        uint32_t mipLevels, int layersCount) {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands();

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;

        if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

            if (hasStencilComponent(format)) {
                barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
            }
        }
        else {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        }

        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = mipLevels;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = layersCount;

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
            newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
            newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
            newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        }
        else {
            throw std::invalid_argument("unsupported layout transition!");
        }
        vkCmdPipelineBarrier(commandBuffer,
            sourceStage, destinationStage, 0,
            0, nullptr, 0, nullptr, 1, &barrier);

        endSingleTimeCommands(commandBuffer);
    }

    VkCommandBuffer beginSingleTimeCommands() {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = commandPool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        return commandBuffer;
    }

    void endSingleTimeCommands(VkCommandBuffer commandBuffer) {
        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;
        vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(graphicsQueue);

        vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
    }

    bool hasStencilComponent(VkFormat format) {
        return format == VK_FORMAT_D32_SFLOAT_S8_UINT ||
            format == VK_FORMAT_D24_UNORM_S8_UINT;
    }

    void createFramebuffers() {
        swapChainFramebuffers.resize(swapChainImageViews.size());
        for (size_t i = 0; i < swapChainImageViews.size(); i++) {
            std::array<VkImageView, 3> attachments = {
                colorImageView,
                depthImageView,
                swapChainImageViews[i]
            };

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType =
                VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = renderPass;
            framebufferInfo.attachmentCount =
                static_cast<uint32_t>(attachments.size());;
            framebufferInfo.pAttachments = attachments.data();
            framebufferInfo.width = swapChainExtent.width;
            framebufferInfo.height = swapChainExtent.height;
            framebufferInfo.layers = 1;

            VkResult result = vkCreateFramebuffer(device, &framebufferInfo, nullptr,
                &swapChainFramebuffers[i]);
            if (result != VK_SUCCESS) {
                PrintVkError(result);
                throw std::runtime_error("failed to create framebuffer!");
            }
        }
    }

    void loadModels() {
        Scene.resize(SceneToLoad.size());
        int i = 0;

        for (const auto& M : SceneToLoad) {
            loadModelWithTexture(M, i);
            i++;
        }

        loadSkyBox();
    }

    void loadModelWithTexture(const Model& M, int i) {
        loadMesh(M.ObjFile, M.type, Scene[i].MD, phongAndSkyBoxVertices);
        createVertexBuffer(Scene[i].MD);
        createIndexBuffer(Scene[i].MD);

        createTextureImage(M.TextureFile, Scene[i].TD);
        createTextureImageView(Scene[i].TD);
        createTextureSampler(Scene[i].TD);
    }

    void loadMesh(const char* FName, ModelType T, ModelData& MD, VertexDescriptor& VD) {
        switch (T) {
        case OBJ:
            loadObjMesh(FName, MD, VD);
            break;
        case GLTF:
            loadGLTFMesh(FName, MD, VD);
            break;
        }
    }

    void loadObjMesh(const char* FName, ModelData& MD, VertexDescriptor& VD) {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err,
            (MODEL_PATH + FName).c_str())) {
            throw std::runtime_error(warn + err);
        }

        MD.vertDesc = &VD;

        std::cout << FName << "\n";

        std::vector<float> vertex{};
        vertex.resize(VD.size);

        //		std::unordered_map<std::vector<float>, uint32_t> uniqueVertices{};
        for (const auto& shape : shapes) {
            for (const auto& index : shape.mesh.indices) {

                vertex[VD.deltaPos + 0] = attrib.vertices[3 * index.vertex_index + 0];
                vertex[VD.deltaPos + 1] = attrib.vertices[3 * index.vertex_index + 1];
                vertex[VD.deltaPos + 2] = attrib.vertices[3 * index.vertex_index + 2];
                vertex[VD.deltaTexCoord + 0] = attrib.texcoords[2 * index.texcoord_index + 0];
                vertex[VD.deltaTexCoord + 1] = 1 - attrib.texcoords[2 * index.texcoord_index + 1];
                vertex[VD.deltaNormal + 0] = attrib.normals[3 * index.normal_index + 0];
                vertex[VD.deltaNormal + 1] = attrib.normals[3 * index.normal_index + 1];
                vertex[VD.deltaNormal + 2] = attrib.normals[3 * index.normal_index + 2];

                //				if (uniqueVertices.count(vertex) == 0) {
                int j = MD.vertices.size() / VD.size;
                //					uniqueVertices[vertex] =
                //							static_cast<uint32_t>(j);
                int s = MD.vertices.size();
                MD.vertices.resize(s + VD.size);
                for (int k = 0; k < VD.size; k++) {
                    MD.vertices[s + k] = vertex[k];
                }
                /**/				MD.indices.push_back(j);
                //				}

                //				MD.indices.push_back(uniqueVertices[vertex]);
            }
        }

        std::cout << FName << " -> V: " << MD.vertices.size()
            << ", I: " << MD.indices.size() << "\n";
    }

    void loadGLTFMesh(const char* FName, ModelData& MD, VertexDescriptor& VD) {
        tinygltf::Model model;
        tinygltf::TinyGLTF loader;
        std::string warn, err;

        if (!loader.LoadASCIIFromFile(&model, &warn, &err,
            (MODEL_PATH + FName).c_str())) {
            throw std::runtime_error(warn + err);
        }

        for (const auto& mesh : model.meshes) {
            std::cout << "Primitives: " << mesh.primitives.size() << "\n";
            for (const auto& primitive : mesh.primitives) {
                if (primitive.indices < 0) {
                    continue;
                }
                //				for (const auto& attribute :  primitive.attributes) {
                //					std::cout << attribute.first << "\n";
                //					tinygltf::Accessor accessor = model.accessors[attribute.second];
                //					std::cout << accessor.bufferView << "\n";
                //				}

                const float* bufferPos = nullptr;
                const float* bufferNormals = nullptr;
                const float* bufferTangents = nullptr;
                const float* bufferTexCoords = nullptr;

                const tinygltf::Accessor& posAccessor = model.accessors[primitive.attributes.find("POSITION")->second];
                const tinygltf::BufferView& posView = model.bufferViews[posAccessor.bufferView];
                bufferPos = reinterpret_cast<const float*>(&(model.buffers[posView.buffer].data[posAccessor.byteOffset + posView.byteOffset]));

                const tinygltf::Accessor& normAccessor = model.accessors[primitive.attributes.find("NORMAL")->second];
                const tinygltf::BufferView& normView = model.bufferViews[normAccessor.bufferView];
                bufferNormals = reinterpret_cast<const float*>(&(model.buffers[normView.buffer].data[normAccessor.byteOffset + normView.byteOffset]));

                const tinygltf::Accessor& tanAccessor = model.accessors[primitive.attributes.find("TANGENT")->second];
                const tinygltf::BufferView& tanView = model.bufferViews[tanAccessor.bufferView];
                bufferTangents = reinterpret_cast<const float*>(&(model.buffers[tanView.buffer].data[tanAccessor.byteOffset + tanView.byteOffset]));

                const tinygltf::Accessor& uvAccessor = model.accessors[primitive.attributes.find("TEXCOORD_0")->second];
                const tinygltf::BufferView& uvView = model.bufferViews[uvAccessor.bufferView];
                bufferTexCoords = reinterpret_cast<const float*>(&(model.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));

                if ((posAccessor.count != normAccessor.count) || (posAccessor.count != tanAccessor.count) || (posAccessor.count != uvAccessor.count)) {
                    std::cerr << "Attribute counts mismatch" << std::endl;
                    throw std::runtime_error("Error loading GLTF component");
                }

                //				std::cout << posAccessor.count << "\n";
                //				std::cout << posView.byteOffset << " " << posView.byteLength << " " << posView.byteStride << "\n";
                //				std::cout << normView.byteOffset << " " << normView.byteLength << " " << normView.byteStride << "\n";
                //				std::cout << uvView.byteOffset << " " << uvView.byteLength << " " << uvView.byteStride << "\n";

                std::vector<float> vertex{};
                vertex.resize(VD.size);
                for (int i = 0; i < posAccessor.count; i++) {
                    vertex[VD.deltaPos + 0] = bufferPos[3 * i + 0];
                    vertex[VD.deltaPos + 1] = bufferPos[3 * i + 1];
                    vertex[VD.deltaPos + 2] = bufferPos[3 * i + 2];
                    vertex[VD.deltaTexCoord + 0] = bufferTexCoords[2 * i + 0];
                    vertex[VD.deltaTexCoord + 1] = bufferTexCoords[2 * i + 1];
                    vertex[VD.deltaNormal + 0] = bufferNormals[3 * i + 0];
                    vertex[VD.deltaNormal + 1] = bufferNormals[3 * i + 1];
                    vertex[VD.deltaNormal + 2] = bufferNormals[3 * i + 2];

                    int j = MD.vertices.size() / VD.size;
                    int s = MD.vertices.size();
                    MD.vertices.resize(s + VD.size);
                    for (int k = 0; k < VD.size; k++) {
                        MD.vertices[s + k] = vertex[k];
                    }
                }

                const tinygltf::Accessor& accessor = model.accessors[primitive.indices];
                const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
                const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

                switch (accessor.componentType) {
                case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:
                {
                    const uint16_t* bufferIndex = reinterpret_cast<const uint16_t*>(&(buffer.data[accessor.byteOffset + bufferView.byteOffset]));
                    for (int i = 0; i < accessor.count; i++) {
                        MD.indices.push_back(bufferIndex[i]);
                    }
                }
                break;
                case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT:
                {
                    const uint32_t* bufferIndex = reinterpret_cast<const uint32_t*>(&(buffer.data[accessor.byteOffset + bufferView.byteOffset]));
                    for (int i = 0; i < accessor.count; i++) {
                        MD.indices.push_back(bufferIndex[i]);
                    }
                }
                break;
                default:
                    std::cerr << "Index component type " << accessor.componentType << " not supported!" << std::endl;
                    throw std::runtime_error("Error loading GLTF component");
                }
            }
        }

        std::cout << FName << " (GLTF) -> V: " << MD.vertices.size()
            << ", I: " << MD.indices.size() << "\n";
        //		throw std::runtime_error("Now We Stop Here!");			
    }

    void createVertexBuffer(ModelData& Md) {
        VkDeviceSize bufferSize = sizeof(Md.vertices[0]) * Md.vertices.size();

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, Md.vertices.data(), (size_t)bufferSize);
        vkUnmapMemory(device, stagingBufferMemory);

        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT |
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            Md.vertexBuffer, Md.vertexBufferMemory);

        copyBuffer(stagingBuffer, Md.vertexBuffer, bufferSize);

        vkDestroyBuffer(device, stagingBuffer, nullptr);
        vkFreeMemory(device, stagingBufferMemory, nullptr);
    }

    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties,
        VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VkResult result =
            vkCreateBuffer(device, &bufferInfo, nullptr, &buffer);
        if (result != VK_SUCCESS) {
            PrintVkError(result);
            throw std::runtime_error("failed to create vertex buffer!");
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex =
            findMemoryType(memRequirements.memoryTypeBits, properties);

        result = vkAllocateMemory(device, &allocInfo, nullptr,
            &bufferMemory);
        if (result != VK_SUCCESS) {
            PrintVkError(result);
            throw std::runtime_error("failed to allocate vertex buffer memory!");
        }

        vkBindBufferMemory(device, buffer, bufferMemory, 0);
    }

    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands();

        VkBufferCopy copyRegion{};
        copyRegion.srcOffset = 0; // Optional copyRegion.dstOffset = 0; // Optional
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        endSingleTimeCommands(commandBuffer);
    }

    void createIndexBuffer(ModelData& Md) {
        VkDeviceSize bufferSize = sizeof(Md.indices[0]) * Md.indices.size();

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, Md.indices.data(), (size_t)bufferSize);
        vkUnmapMemory(device, stagingBufferMemory);

        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT |
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            Md.indexBuffer, Md.indexBufferMemory);

        copyBuffer(stagingBuffer, Md.indexBuffer, bufferSize);
        vkDestroyBuffer(device, stagingBuffer, nullptr);
        vkFreeMemory(device, stagingBufferMemory, nullptr);
    }

    void createTextureImage(const char* FName, TextureData& TD) {
        int texWidth, texHeight, texChannels;
        stbi_uc* pixels = stbi_load((TEXTURE_PATH + FName).c_str(), &texWidth, &texHeight,
            &texChannels, STBI_rgb_alpha);
        if (!pixels) {
            std::cout << (TEXTURE_PATH + FName).c_str() << "\n";
            throw std::runtime_error("failed to load texture image!");
        }
        std::cout << FName << " -> size: " << texWidth
            << "x" << texHeight << ", ch: " << texChannels << "\n";

        VkDeviceSize imageSize = texWidth * texHeight * 4;
        TD.mipLevels = static_cast<uint32_t>(std::floor(
            std::log2(std::max(texWidth, texHeight)))) + 1;

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;

        createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer, stagingBufferMemory);
        void* data;
        vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
        memcpy(data, pixels, static_cast<size_t>(imageSize));
        vkUnmapMemory(device, stagingBufferMemory);

        stbi_image_free(pixels);

        createImage(texWidth, texHeight, TD.mipLevels,
            VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
            VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, TD.textureImage,
            TD.textureImageMemory);

        transitionImageLayout(TD.textureImage, VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, TD.mipLevels, 1);
        copyBufferToImage(stagingBuffer, TD.textureImage,
            static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), 1);

        generateMipmaps(TD.textureImage, VK_FORMAT_R8G8B8A8_SRGB,
            texWidth, texHeight, TD.mipLevels, 1);

        vkDestroyBuffer(device, stagingBuffer, nullptr);
        vkFreeMemory(device, stagingBufferMemory, nullptr);
    }

    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t
        width, uint32_t height, int layerCount) {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands();

        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = layerCount;
        region.imageOffset = { 0, 0, 0 };
        region.imageExtent = { width, height, 1 };

        vkCmdCopyBufferToImage(commandBuffer, buffer, image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

        endSingleTimeCommands(commandBuffer);
    }

    void generateMipmaps(VkImage image, VkFormat imageFormat,
        int32_t texWidth, int32_t texHeight,
        uint32_t mipLevels, int layerCount) {
        VkFormatProperties formatProperties;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, imageFormat,
            &formatProperties);

        if (!(formatProperties.optimalTilingFeatures &
            VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
            throw std::runtime_error("texture image format does not support linear blitting!");
        }

        VkCommandBuffer commandBuffer = beginSingleTimeCommands();

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.image = image;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = layerCount;
        barrier.subresourceRange.levelCount = 1;

        int32_t mipWidth = texWidth;
        int32_t mipHeight = texHeight;

        for (uint32_t i = 1; i < mipLevels; i++) {
            barrier.subresourceRange.baseMipLevel = i - 1;
            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

            vkCmdPipelineBarrier(commandBuffer,
                VK_PIPELINE_STAGE_TRANSFER_BIT,
                VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
                0, nullptr, 0, nullptr,
                1, &barrier);

            VkImageBlit blit{};
            blit.srcOffsets[0] = { 0, 0, 0 };
            blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
            blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.srcSubresource.mipLevel = i - 1;
            blit.srcSubresource.baseArrayLayer = 0;
            blit.srcSubresource.layerCount = layerCount;
            blit.dstOffsets[0] = { 0, 0, 0 };
            blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1,
                                   mipHeight > 1 ? mipHeight / 2 : 1, 1 };
            blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.dstSubresource.mipLevel = i;
            blit.dstSubresource.baseArrayLayer = 0;
            blit.dstSubresource.layerCount = layerCount;

            vkCmdBlitImage(commandBuffer, image,
                VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
                &blit, VK_FILTER_LINEAR);

            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            vkCmdPipelineBarrier(commandBuffer,
                VK_PIPELINE_STAGE_TRANSFER_BIT,
                VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                0, nullptr, 0, nullptr,
                1, &barrier);
            if (mipWidth > 1) mipWidth /= 2;
            if (mipHeight > 1) mipHeight /= 2;
        }

        barrier.subresourceRange.baseMipLevel = mipLevels - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        vkCmdPipelineBarrier(commandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
            0, nullptr, 0, nullptr,
            1, &barrier);

        endSingleTimeCommands(commandBuffer);
    }

    void createTextureImageView(TextureData& TD) {
        TD.textureImageView = createImageView(TD.textureImage,
            VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_ASPECT_COLOR_BIT,
            TD.mipLevels,
            VK_IMAGE_VIEW_TYPE_2D, 1);
    }

    void createTextureSampler(TextureData& TD) {
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = 16;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = static_cast<float>(TD.mipLevels);

        VkResult result = vkCreateSampler(device, &samplerInfo, nullptr,
            &TD.textureSampler);
        if (result != VK_SUCCESS) {
            PrintVkError(result);
            throw std::runtime_error("failed to create texture sampler!");
        }
    }

    void loadSkyBox() {
        int i = 0;
        SkyBox.resize(SkyBoxToLoad.size());
        for (const auto& M : SkyBoxToLoad) {
            loadMesh(M.ObjFile, M.type, SkyBox[i].MD, phongAndSkyBoxVertices);
            createVertexBuffer(SkyBox[i].MD);
            createIndexBuffer(SkyBox[i].MD);

            createCubicTextureImage(M.TextureFile, SkyBox[i].TD);
            createSkyBoxImageView(SkyBox[i].TD);
            createTextureSampler(SkyBox[i].TD);

            i++;
        }
    }

    void createCubicTextureImage(const char* const FName[6], TextureData& TD) {
        int texWidth, texHeight, texChannels;
        stbi_uc* pixels[6];

        for (int i = 0; i < 6; i++) {
            pixels[i] = stbi_load((TEXTURE_PATH + FName[i]).c_str(), &texWidth, &texHeight,
                &texChannels, STBI_rgb_alpha);
            if (!pixels[i]) {
                std::cout << (TEXTURE_PATH + FName[i]).c_str() << "\n";
                throw std::runtime_error("failed to load texture image!");
            }
            std::cout << FName[i] << " -> size: " << texWidth
                << "x" << texHeight << ", ch: " << texChannels << "\n";
        }

        VkDeviceSize imageSize = texWidth * texHeight * 4;
        VkDeviceSize totalImageSize = texWidth * texHeight * 4 * 6;
        TD.mipLevels = static_cast<uint32_t>(std::floor(
            std::log2(std::max(texWidth, texHeight)))) + 1;

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;

        createBuffer(totalImageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer, stagingBufferMemory);
        void* data;
        vkMapMemory(device, stagingBufferMemory, 0, totalImageSize, 0, &data);
        for (int i = 0; i < 6; i++) {
            memcpy(static_cast<char*>(data) + imageSize * i, pixels[i], static_cast<size_t>(imageSize));
        }
        vkUnmapMemory(device, stagingBufferMemory);

        for (int i = 0; i < 6; i++) {
            stbi_image_free(pixels[i]);
        }
        createSkyBoxImage(texWidth, texHeight, TD.mipLevels, TD.textureImage,
            TD.textureImageMemory);

        transitionImageLayout(TD.textureImage, VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, TD.mipLevels, 6);
        copyBufferToImage(stagingBuffer, TD.textureImage,
            static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), 6);

        generateMipmaps(TD.textureImage, VK_FORMAT_R8G8B8A8_SRGB,
            texWidth, texHeight, TD.mipLevels, 6);

        vkDestroyBuffer(device, stagingBuffer, nullptr);
        vkFreeMemory(device, stagingBufferMemory, nullptr);
    }

    void createSkyBoxImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkImage& image,
        VkDeviceMemory& imageMemory) {
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = width;
        imageInfo.extent.height = height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = mipLevels;
        imageInfo.arrayLayers = 6;
        imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;

        VkResult result = vkCreateImage(device, &imageInfo, nullptr, &image);
        if (result != VK_SUCCESS) {
            PrintVkError(result);
            throw std::runtime_error("failed to create image!");
        }

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(device, image, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to allocate image memory!");
        }

        vkBindImageMemory(device, image, imageMemory, 0);
    }

    void createSkyBoxImageView(TextureData& TD) {
        TD.textureImageView = createImageView(TD.textureImage,
            VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_ASPECT_COLOR_BIT,
            TD.mipLevels,
            VK_IMAGE_VIEW_TYPE_CUBE, 6);
    }

    void createUniformBuffers() {
        VkDeviceSize bufferSize = sizeof(UniformBufferObject);

        uniformBuffers.resize(swapChainImages.size() * Scene.size());
        uniformBuffersMemory.resize(swapChainImages.size() * Scene.size());

        for (size_t i = 0; i < swapChainImages.size() * Scene.size(); i++) {
            createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                uniformBuffers[i], uniformBuffersMemory[i]);
        }

        bufferSize = sizeof(GlobalUniformBufferObject);

        globalUniformBuffers.resize(swapChainImages.size());
        globalUniformBuffersMemory.resize(swapChainImages.size());

        for (size_t i = 0; i < swapChainImages.size(); i++) {
            createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                globalUniformBuffers[i], globalUniformBuffersMemory[i]);
        }

        bufferSize = sizeof(UniformBufferObject);

        SkyBoxUniformBuffers.resize(swapChainImages.size() * SkyBox.size());
        SkyBoxUniformBuffersMemory.resize(swapChainImages.size() * SkyBox.size());

        for (size_t i = 0; i < swapChainImages.size() * SkyBox.size(); i++) {
            createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                SkyBoxUniformBuffers[i], SkyBoxUniformBuffersMemory[i]);
        }

        /* Text buffer not needed but keet it here for now
        bufferSize = sizeof(UniformBufferObject);
        TextUniformBuffers.resize(swapChainImages.size());
        TextUniformBuffersMemory.resize(swapChainImages.size());

        for (size_t i = 0; i < swapChainImages.size(); i++) {
            createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                TextUniformBuffers[i], TextUniformBuffersMemory[i]);
        }
        */
    }

    void createDescriptorPool() {
        std::array<VkDescriptorPoolSize, 9> poolSizes{};
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = static_cast<uint32_t>(swapChainImages.size() * Scene.size());
        poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[1].descriptorCount = static_cast<uint32_t>(swapChainImages.size() * Scene.size());
        poolSizes[2].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[2].descriptorCount = static_cast<uint32_t>(swapChainImages.size() * Scene.size());
        poolSizes[3].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[3].descriptorCount = static_cast<uint32_t>(swapChainImages.size() * SkyBox.size());
        poolSizes[4].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[4].descriptorCount = static_cast<uint32_t>(swapChainImages.size() * SkyBox.size());
        poolSizes[5].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[5].descriptorCount = static_cast<uint32_t>(swapChainImages.size());
        poolSizes[6].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[6].descriptorCount = static_cast<uint32_t>(swapChainImages.size());
        poolSizes[7].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[7].descriptorCount = static_cast<uint32_t>(swapChainImages.size());
        poolSizes[8].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[8].descriptorCount = static_cast<uint32_t>(swapChainImages.size());

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = static_cast<uint32_t>(swapChainImages.size() * (Scene.size() + SkyBox.size() + 1));

        VkResult result = vkCreateDescriptorPool(device, &poolInfo, nullptr,
            &descriptorPool);
        if (result != VK_SUCCESS) {
            PrintVkError(result);
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }

    void createDescriptorSets() {
        createPhongDescriptorSets();
        // createWireframeDescriptorSets();
        createSkyBoxDescriptorSets();
    }

    void createPhongDescriptorSets() {
        std::vector<VkDescriptorSetLayout> layouts(swapChainImages.size() * Scene.size(),
            PhongDescriptorSetLayout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChainImages.size() * Scene.size());
        allocInfo.pSetLayouts = layouts.data();

        PhongDescriptorSets.resize(swapChainImages.size() * Scene.size());

        VkResult result = vkAllocateDescriptorSets(device, &allocInfo,
            PhongDescriptorSets.data());
        if (result != VK_SUCCESS) {
            PrintVkError(result);
            throw std::runtime_error("failed to allocate descriptor sets!");
        }

        for (size_t k = 0; k < swapChainImages.size(); k++) {
            for (size_t j = 0; j < Scene.size(); j++) {
                size_t i = j * swapChainImages.size() + k;

                VkDescriptorBufferInfo bufferInfo{};
                bufferInfo.buffer = uniformBuffers[i];
                bufferInfo.offset = 0;
                bufferInfo.range = sizeof(UniformBufferObject);

                VkDescriptorImageInfo imageInfo{};
                imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                imageInfo.imageView = Scene[j].TD.textureImageView;
                imageInfo.sampler = Scene[j].TD.textureSampler;

                VkDescriptorBufferInfo globalBufferInfo{};
                globalBufferInfo.buffer = globalUniformBuffers[k];
                globalBufferInfo.offset = 0;
                globalBufferInfo.range = sizeof(GlobalUniformBufferObject);

                std::array<VkWriteDescriptorSet, 3> descriptorWrites{};
                descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrites[0].dstSet = PhongDescriptorSets[i];
                descriptorWrites[0].dstBinding = 0;
                descriptorWrites[0].dstArrayElement = 0;
                descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                descriptorWrites[0].descriptorCount = 1;
                descriptorWrites[0].pBufferInfo = &bufferInfo;

                descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrites[1].dstSet = PhongDescriptorSets[i];
                descriptorWrites[1].dstBinding = 1;
                descriptorWrites[1].dstArrayElement = 0;
                descriptorWrites[1].descriptorType =
                    VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                descriptorWrites[1].descriptorCount = 1;
                descriptorWrites[1].pImageInfo = &imageInfo;

                descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrites[2].dstSet = PhongDescriptorSets[i];
                descriptorWrites[2].dstBinding = 2;
                descriptorWrites[2].dstArrayElement = 0;
                descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                descriptorWrites[2].descriptorCount = 1;
                descriptorWrites[2].pBufferInfo = &globalBufferInfo;

                vkUpdateDescriptorSets(device,
                    static_cast<uint32_t>(descriptorWrites.size()),
                    descriptorWrites.data(), 0, nullptr);
            }
        }
    }

    void createWireframeDescriptorSets() {
        std::vector<VkDescriptorSetLayout> layouts(swapChainImages.size() * Scene.size(),
            WireframeDescriptorSetLayout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChainImages.size() * Scene.size());
        allocInfo.pSetLayouts = layouts.data();

        WireframeDescriptorSets.resize(swapChainImages.size() * Scene.size());

        VkResult result = vkAllocateDescriptorSets(device, &allocInfo,
            WireframeDescriptorSets.data());
        if (result != VK_SUCCESS) {
            PrintVkError(result);
            throw std::runtime_error("failed to allocate descriptor sets!");
        }

        for (size_t k = 0; k < swapChainImages.size(); k++) {
            for (size_t j = 0; j < Scene.size(); j++) {
                size_t i = j * swapChainImages.size() + k;

                VkDescriptorBufferInfo bufferInfo{};
                bufferInfo.buffer = uniformBuffers[i];
                bufferInfo.offset = 0;
                bufferInfo.range = sizeof(UniformBufferObject);

                VkDescriptorImageInfo imageInfo{};
                imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                imageInfo.imageView = Scene[j].TD.textureImageView;
                imageInfo.sampler = Scene[j].TD.textureSampler;

                VkDescriptorBufferInfo globalBufferInfo{};
                globalBufferInfo.buffer = WireframeGlobalUniformBuffers[k];
                globalBufferInfo.offset = 0;
                globalBufferInfo.range = sizeof(GlobalUniformBufferObject);

                std::array<VkWriteDescriptorSet, 3> descriptorWrites{};
                descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrites[0].dstSet = PhongDescriptorSets[i];
                descriptorWrites[0].dstBinding = 0;
                descriptorWrites[0].dstArrayElement = 0;
                descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                descriptorWrites[0].descriptorCount = 1;
                descriptorWrites[0].pBufferInfo = &bufferInfo;

                descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrites[1].dstSet = WireframeDescriptorSets[i];
                descriptorWrites[1].dstBinding = 1;
                descriptorWrites[1].dstArrayElement = 0;
                descriptorWrites[1].descriptorType =
                    VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                descriptorWrites[1].descriptorCount = 1;
                descriptorWrites[1].pImageInfo = &imageInfo;

                descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrites[2].dstSet = WireframeDescriptorSets[i];
                descriptorWrites[2].dstBinding = 2;
                descriptorWrites[2].dstArrayElement = 0;
                descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                descriptorWrites[2].descriptorCount = 1;
                descriptorWrites[2].pBufferInfo = &globalBufferInfo;

                vkUpdateDescriptorSets(device,
                    static_cast<uint32_t>(descriptorWrites.size()),
                    descriptorWrites.data(), 0, nullptr);
            }
        }
    }

    void createSkyBoxDescriptorSets() {
        std::vector<VkDescriptorSetLayout> layouts(swapChainImages.size() * SkyBox.size(),
            SkyBoxDescriptorSetLayout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChainImages.size() * SkyBox.size());
        allocInfo.pSetLayouts = layouts.data();

        SkyBoxDescriptorSets.resize(swapChainImages.size() * SkyBox.size());

        VkResult result = vkAllocateDescriptorSets(device, &allocInfo,
            SkyBoxDescriptorSets.data());
        if (result != VK_SUCCESS) {
            PrintVkError(result);
            throw std::runtime_error("failed to allocate Skybox descriptor sets!");
        }

        for (size_t k = 0; k < swapChainImages.size(); k++) {
            for (size_t j = 0; j < SkyBox.size(); j++) {
                size_t i = j * swapChainImages.size() + k;

                VkDescriptorBufferInfo bufferInfo{};
                bufferInfo.buffer = SkyBoxUniformBuffers[i];
                bufferInfo.offset = 0;
                bufferInfo.range = sizeof(UniformBufferObject);

                VkDescriptorImageInfo imageInfo{};
                imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                imageInfo.imageView = SkyBox[j].TD.textureImageView;
                imageInfo.sampler = SkyBox[j].TD.textureSampler;

                std::array<VkWriteDescriptorSet, 2> descriptorWrites{};
                descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrites[0].dstSet = SkyBoxDescriptorSets[i];
                descriptorWrites[0].dstBinding = 0;
                descriptorWrites[0].dstArrayElement = 0;
                descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                descriptorWrites[0].descriptorCount = 1;
                descriptorWrites[0].pBufferInfo = &bufferInfo;

                descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrites[1].dstSet = SkyBoxDescriptorSets[i];
                descriptorWrites[1].dstBinding = 1;
                descriptorWrites[1].dstArrayElement = 0;
                descriptorWrites[1].descriptorType =
                    VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                descriptorWrites[1].descriptorCount = 1;
                descriptorWrites[1].pImageInfo = &imageInfo;

                vkUpdateDescriptorSets(device,
                    static_cast<uint32_t>(descriptorWrites.size()),
                    descriptorWrites.data(), 0, nullptr);
            }
        }
    }

    void createSyncObjects() {
        imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
        imagesInFlight.resize(swapChainImages.size(), VK_NULL_HANDLE);

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            VkResult result1 = vkCreateSemaphore(device, &semaphoreInfo, nullptr,
                &imageAvailableSemaphores[i]);
            VkResult result2 = vkCreateSemaphore(device, &semaphoreInfo, nullptr,
                &renderFinishedSemaphores[i]);
            VkResult result3 = vkCreateFence(device, &fenceInfo, nullptr,
                &inFlightFences[i]);
            if (result1 != VK_SUCCESS ||
                result2 != VK_SUCCESS ||
                result3 != VK_SUCCESS) {
                PrintVkError(result1);
                PrintVkError(result2);
                PrintVkError(result3);
                throw std::runtime_error("failed to create synchronization objects for a frame!!");
            }
        }
    }
};


using namespace reactphysics3d;

void testPhysics()
{
    PhysicsCommon physicsCommon;
    
    PhysicsWorld* world = physicsCommon.createPhysicsWorld();

    Vector3 position(0, 20, 0);
    Quaternion orientation = Quaternion::identity();
    Transform transform(position, orientation);
    RigidBody* body = world->createRigidBody(transform);

    const decimal timeStep = 1.0f / 60.0f;

    for (int i = 0; i < 20; i++) {

        world->update(timeStep);

        const Transform& transform = body->getTransform();
        const Vector3& position = transform.getPosition();

        std::cout << "Body Position: (" << position.x << ", " <<
            position.y << ", " << position.z << ")" << std::endl;
    }

    return 0;
}

void testECS()
{
    flecs::world world;
    world.import<Test>();

    auto a = world.entity()
        .set<Counter>({ 0 });
    auto b = world.entity()
        .set<Counter>({ 10 });
    auto c = world.entity()
        .set<Counter>({ 20 });
    world.system<Counter>().kind(flecs::OnUpdate).iter([](flecs::iter it) {
        printf("AAA\n");
        });

    world.progress();
    world.progress();
    world.progress();
    world.progress();
    world.progress();
}

int main()
{
    testPhysics();
    
    CGProject app;

    testECS();

    try
    {
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}