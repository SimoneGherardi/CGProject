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
    ENGINE.Initialize(TITLE, surfaceFactory, {WIDTH, HEIGHT});
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
        cleanup();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    TRACEEND;

    return EXIT_SUCCESS;
}
