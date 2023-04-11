#include "custom_GUI.h"



void showCustomWindow(ImTextureID renderTexture, WindowSize windowSize) {
    {
        static float f = 0.0f;
        static int counter = 0;
        ImVec4 clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        bool showDemoWindow = false;
        bool showAnotherWindow = true;

        // Dimensions
        float scaleRatio = 0.7f;
        ImVec2 menuBarDimensions = ImVec2(((float)windowSize.Width), ((float)windowSize.Height) * 0.03);
        ImVec2 sceneDimensions = ImVec2(((float)windowSize.Width) * scaleRatio , ((float)windowSize.Height) * scaleRatio + ((float)windowSize.Height) * 0.07);
        ImVec2 prefabContainerDimensions = ImVec2(((float)windowSize.Width) * scaleRatio, ((float)windowSize.Height) * (1.0f - scaleRatio) - menuBarDimensions[1]);
        ImVec2 logDimensions = ImVec2(((float)windowSize.Width) * (1.0f - scaleRatio), ((float)windowSize.Height) - menuBarDimensions[1]);

        // Positions
        ImVec2 menuBarPosition = ImVec2(0, 0);
        ImVec2 scenePosition = ImVec2(0, menuBarDimensions[1]);
        ImVec2 prefabContainerPosition = ImVec2(0, menuBarDimensions[1] + sceneDimensions[1]);
        ImVec2 logPosition = ImVec2(sceneDimensions[0], menuBarDimensions[1]);


        // Menu bar
        ImGui::SetNextWindowSize(menuBarDimensions);
        ImGui::SetNextWindowPos(menuBarPosition);
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;
        ImGui::Begin("Menu Bar", NULL, flags);
        if (ImGui::Button("Open")) {}
        ImGui::SameLine();
        if (ImGui::Button("Save")) {}
        ImGui::End();

        // Scene 
        ImGui::SetNextWindowSize(sceneDimensions);
        ImGui::SetNextWindowPos(scenePosition);
        //flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;

        ImGui::Begin("Scene", NULL, flags);                          // Create a window called "Hello, world!" and append into it.
        
        ImGui::Image(renderTexture, ImVec2(1200, 675));
        
        ImGui::End();


        //PrefabContainer
        ImGui::SetNextWindowSize(prefabContainerDimensions);
        ImGui::SetNextWindowPos(prefabContainerPosition);
        flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;

        ImGui::Begin("Prefab Container", NULL, flags);

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &showDemoWindow);      // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &showAnotherWindow);


        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", (float*)&clearColor); // Edit 3 floats representing a color

        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::End();

        //Log
        ImGui::SetNextWindowSize(logDimensions);
        ImGui::SetNextWindowPos(logPosition);
        flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;

        ImGui::Begin("Log", NULL, flags);

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &showDemoWindow);      // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &showAnotherWindow);


        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", (float*)&clearColor); // Edit 3 floats representing a color

        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::End();
        
    }
}