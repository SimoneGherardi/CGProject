#pragma once
#include "imgui.h"
#include "imgui_internal.h"
#include<GLFW/glfw3.h>
#include "stb_image.h"

struct WindowSize {
	int Width, Height;
};

void scaledGetCursorPos(GLFWwindow* window, double* xpos, double* ypos);
void showCustomWindow(ImTextureID renderTexture, WindowSize windowSize);

class EditorGUI
{
public:
	float WindowWidth;
	float WindowHeight;
    ImVec4 ClearColor;
    bool ShowDemoWindow;
    bool ShowAnotherWindow;
    int Counter = 0;

    int Block1width;
    int Block1height;
    ImTextureID Block1png;

    // Dimensions
    float ScaleFactor;
    float MenuBarHeight;
    float HorizontalBorder;
    ImVec2 MenuBarDimensions;
    ImVec2 SceneDimensions;
    ImVec2 PrefabContainerDimensions;
    ImVec2 LogDimensions;

    // Positions
    ImVec2 MenuBarPosition;
    ImVec2 ScenePosition;
    ImVec2 PrefabContainerPosition;
    ImVec2 LogPosition;

	
    EditorGUI(WindowSize windowSize);
    
    void showCustomWindow(ImTextureID renderTexture, WindowSize windowSize);
};
