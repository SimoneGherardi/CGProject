#pragma once
#include "imgui.h"

struct WindowSize {
	int Width, Height;
};

void showCustomWindow(ImTextureID renderTexture, WindowSize windowSize);