#ifdef __APPLE__
    #define sprintf_s(buf, ...) snprintf((buf), sizeof(buf), __VA_ARGS__)
#endif

#include "game_engine.h"
#include "window.h"

const char* TITLE = "CG Project";
const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

GLFWwindow* Window;
GameEngine ENGINE;

void onResize(GLFWwindow* window, int width, int height)
{
    auto engine = reinterpret_cast<GameEngine*>(glfwGetWindowUserPointer(window));
    // TODO engine resize
}

void initialize()
{
    GLFWSurfaceFactory* surfaceFactory = new GLFWSurfaceFactory(Window);
    initializeWindow(TITLE, WIDTH, HEIGHT, &ENGINE, onResize, Window);
    ENGINE.Initialize(TITLE, surfaceFactory);
}

void cleanup()
{
    ENGINE.Cleanup();
    cleanupWindow(Window);
}

int main()
{
    try
    {
        initialize();
        cleanup();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
