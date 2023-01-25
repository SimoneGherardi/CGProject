#pragma once
#include "defines.h"
#include "instance.h"
#include "debug_messenger.h"
#include "extensions.h"
#include "validation_layers.h"
#include "surface.h"

class GameEngine
{
private:
    VkInstance _Instance;
    VkDebugUtilsMessengerEXT _DebugMessenger;
    VkSurfaceKHR _Surface;

    #ifdef ENABLE_VALIDATION_LAYERS
    const std::vector<const char*> _ValidationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };
    #else
    const std::vector<const char*> _ValidationLayers = {};
    #endif

    void _InitializeInstance(const char* title)
    {
        auto debugCreateInfo = getDebugMessengerCreateInfo(
            DEBUG_CALLBACK,
            DEBUG_MESSENGER_SEVERITY_MASK(1, 1, 1, 1),
            DEBUG_MESSENGER_TYPE_MASK(1, 1, 1),
            nullptr
        );
        auto requiredExtensions = getRequiredExtensions();
        auto appInfo = getApplicationInfo(title, VK_MAKE_API_VERSION(0, 1, 0, 0));
        auto createInfo = getInstanceCreateInfo(
            &_ValidationLayers, &requiredExtensions, &appInfo, &debugCreateInfo
        );
        initializeInstance(&createInfo, &_Instance);
    }

    void _InitializeDebugMessenger() {
        auto createInfo = getDebugMessengerCreateInfo(
            DEBUG_CALLBACK,
            DEBUG_MESSENGER_SEVERITY_MASK(1, 1, 1, 1),
            DEBUG_MESSENGER_TYPE_MASK(1, 1, 1),
            nullptr
        );
        initializeDebugMessenger(_Instance, &createInfo, nullptr, &_DebugMessenger);
    }

    void _InitializeSurface(SurfaceFactory* factory) {
        initializeSurface(_Instance, &_Surface, factory);
    }

public:
	void Initialize(const char* title, SurfaceFactory* factory) {
        _InitializeInstance(title);
        #ifdef ENABLE_VALIDATION_LAYERS
        _InitializeDebugMessenger();
        #endif
        _InitializeSurface(factory);
	}
};