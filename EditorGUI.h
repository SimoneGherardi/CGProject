#ifndef EDITOR_CLASS_H
#define EDITOR_CLASS_H

#pragma once
#include "imgui.h"
#include "imgui_internal.h"
#include <GLFW/glfw3.h>
#include "stb_image.h"
#include "glm/ext.hpp"
#include <iostream>



#include "glm/gtx/string_cast.hpp"
#include <string>
#include <vector>

struct WindowSize {
	int Width, Height;
};

struct LogEntry {
    std::string Text;
    bool IsSelected;

    LogEntry(std::string text, bool isSelected);
};

class EditorGUI
{
public:
	float WindowWidth;
	float WindowHeight;
    ImVec4 ClearColor;
    bool ShowDemoWindow;
    bool ShowAnotherWindow;
    int Counter = 0;
    char _LastLeftEvent = GLFW_RELEASE;


    std::vector <LogEntry> Log;

    // Dimensions
    float ScaleFactor;
    float MenuBarHeight;
    float HorizontalBorder;
    ImVec2 MenuBarDimensions;
    ImVec2 SceneDimensions;
    ImVec2 PrefabContainerDimensions;
    ImVec2 LogDimensions;
    ImVec2 ButtonDimensions = ImVec2(200, 50);
    ImVec2 LogEntryDimensions  = ImVec2(100, 50);

    // Positions
    glm::vec2 SceneCenterPosition;
    ImVec2 MenuBarPosition;
    ImVec2 ScenePosition;
    ImVec2 PrefabContainerPosition;
    ImVec2 LogPosition;

	
    static EditorGUI* GetInstance();
    void Initialize(WindowSize windowSize);
    void ShowCustomWindow(ImTextureID renderTexture, WindowSize windowSize);
    void AddLogEntry(std::string entryText);
    bool CheckMouseInsideScene(float mouseX, float mouseY);
    bool ScaledGetCursorPos(GLFWwindow* window, double* xpos, double* ypos);
    void Inputs(GLFWwindow* window);
    
    
protected:
    EditorGUI();
    static EditorGUI* _Instance;


};
#endif