#ifdef __APPLE__
    #define sprintf_s(buf, ...) snprintf((buf), sizeof(buf), __VA_ARGS__)
#endif

#include "game_engine.h"
#include "rendering_engine.h"
#include "window.h"
#include "gltf_loader.h"
#include "scene_elements.h"
#include <chrono>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>


const char* TITLE = "CG Project";
const uint32_t WIDTH = 1600;
const uint32_t HEIGHT = 900;

GLFWwindow* Window;

void inputHandler(GLFWwindow* window, Camera camera, float delta_time) {
    // TODO here we will handle the inputs
    
}

void onResize(GLFWwindow* window, int width, int height)
{
    TRACESTART;
    auto engine = reinterpret_cast<RenderingEngine*>(glfwGetWindowUserPointer(window));
    // TODO engine resize
    TRACEEND;
}

void initialize()
{
    TRACESTART;
    GLFWSurfaceFactory* surfaceFactory = new GLFWSurfaceFactory(&Window);
    Window = initializeWindow(TITLE, WIDTH, HEIGHT, &(RenderingEngine::GetInstance()), onResize);
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForVulkan(Window, true);
    RenderingEngine::GetInstance().Initialize(TITLE, surfaceFactory, {WIDTH, HEIGHT});
    TRACEEND;
}

void cleanup()
{
    TRACESTART;
    RenderingEngine::GetInstance().Cleanup();
    cleanupWindow(Window);
    TRACEEND;
}

int main(int argc, char** argv)
{
    TRACESTART;
    if (argc > 1 && strcmp(argv[1], "--build-assets") == 0) {
        LOGDBG("Building assets");
        RenderContext::GetInstance().BuildAssets();
		return EXIT_SUCCESS;
	}
    try
    {
        initialize();
        float delta = 0;
        using clock = std::chrono::system_clock;
        using millisec = std::chrono::duration<float>;
        while (!glfwWindowShouldClose(Window)) {
            const auto start = clock::now();
            glfwPollEvents();
            GameEngine::GetInstance().Loop(delta);
            ImGui_ImplVulkan_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            ImGui::ShowDemoWindow();
            // inputs


            RenderingEngine::GetInstance().Render(delta);
            const millisec duration = clock::now() - start;
            delta = duration.count();
        }
        cleanup();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    TRACEEND;

    return EXIT_SUCCESS;
}
