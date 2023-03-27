#ifdef __APPLE__
    #define sprintf_s(buf, ...) snprintf((buf), sizeof(buf), __VA_ARGS__)
#endif

#include "game_engine.h"
#include "window.h"
#include "gltf_loader.h"
#include <chrono>


const char* TITLE = "CG Project";
const uint32_t WIDTH = 1600;
const uint32_t HEIGHT = 900;

GLFWwindow* Window;
GameEngine ENGINE;

void onResize(GLFWwindow* window, int width, int height)
{
    TRACESTART;
    auto engine = reinterpret_cast<GameEngine*>(glfwGetWindowUserPointer(window));
    // TODO engine resize
    TRACEEND;
}

void initialize()
{
    TRACESTART;
    GLFWSurfaceFactory* surfaceFactory = new GLFWSurfaceFactory(&Window);
    Window = initializeWindow(TITLE, WIDTH, HEIGHT, &ENGINE, onResize);
    ENGINE.Initialize(TITLE, surfaceFactory, {WIDTH, HEIGHT}, Window);
    TRACEEND;
}

void cleanup()
{
    TRACESTART;
    ENGINE.Cleanup();
    cleanupWindow(Window);
    TRACEEND;
}

int main()
{
    TRACESTART;
    try
    {
        initialize();
        float delta = 0;
        using clock = std::chrono::system_clock;
        using millisec = std::chrono::duration<double, std::milli>;
        while (!glfwWindowShouldClose(Window)) {
            const auto start = clock::now();
            glfwPollEvents();


            // https://vkguide.dev/docs/extra-chapter/implementing_imgui/
            ImGuiIO& io = ImGui::GetIO(); (void)io;
            
            ImGui_ImplVulkan_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            
            // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
            {
                static float f = 0.0f;
                static int counter = 0;
                bool show_demo_window = false;
                bool show_another_window = true;
                ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

                ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

                ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
                ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
                ImGui::Checkbox("Another Window", &show_another_window);

                ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
                ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

                if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                    counter++;
                ImGui::SameLine();
                ImGui::Text("counter = %d", counter);

                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
                ImGui::End();
            }

            ENGINE.Render(delta);
            const millisec duration = clock::now() - start;
            delta = (float) duration.count() / 1000.0f;
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
