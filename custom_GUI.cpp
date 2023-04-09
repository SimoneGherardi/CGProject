#include "custom_GUI.h"


void showCustomWindow(ImTextureID renderTexture) {
    {
        static float f = 0.0f;
        static int counter = 0;
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        bool show_demo_window = false;
        bool show_another_window = true;

        ImGui::SetNextWindowSize(ImVec2(1600, 900));
        ImGui::SetNextWindowPos(ImVec2(0,0));
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;

        ImGui::Begin("Hello, world!", NULL, flags);                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &show_another_window);

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Image(renderTexture, ImVec2(800, 450));
        ImGui::End();
    }
}