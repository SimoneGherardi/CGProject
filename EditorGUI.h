#ifndef EDITOR_CLASS_H
#define EDITOR_CLASS_H

#pragma once
#include "imgui.h"
#include "imgui_internal.h"
#include <GLFW/glfw3.h>
#include "stb_image.h"
#include "glm/ext.hpp"
#include <iostream>
#include "flecs.h"

#include "glm/gtx/string_cast.hpp"
#include <string>
#include <vector>

struct WindowSize {
	int Width, Height;
};

struct LogEntry {
    flecs::entity* Entity;
    bool IsSelected;

    LogEntry(flecs::entity* entity, bool isSelected);
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

    // Debug
    float vec4f[4] = { 0.10f, 0.20f, 0.30f, 0.44f };

    std::vector <flecs::entity> Entities;
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
    ImVec2 LogEditPromptDimensions = ImVec2(200, 100);

    // Positions
    glm::vec2 SceneCenterPosition;
    ImVec2 MenuBarPosition;
    ImVec2 ScenePosition;
    ImVec2 PrefabContainerPosition;
    ImVec2 LogPosition;
    ImVec2 LogEditPromptPositions;

	
    static EditorGUI* GetInstance();
    void Initialize(WindowSize windowSize, GLFWwindow* window);
    void ShowCustomWindow(ImTextureID renderTexture, WindowSize windowSize, GLFWwindow* window);
    void AddLogEntry(flecs::entity* entity);
    bool CheckMouseInsideScene(float mouseX, float mouseY);
    bool ScaledGetCursorPos(GLFWwindow* window, double* xpos, double* ypos);
    void Inputs(GLFWwindow* window);
    void CheckSpaceForPrompt(double* spawnX, double* spawnY, ImVec2 dimensions);
    void PrintPrompt(LogEntry entry);
    
    
protected:
    EditorGUI();
    GLFWwindow* _Window;
    static EditorGUI* _Instance;


};
#endif