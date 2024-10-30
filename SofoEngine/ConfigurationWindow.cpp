#include "ConfigurationWindow.h"
#include "imgui.h"
#include "SDL2/SDL.h"
#include <IL/il.h>
#include <GL/glew.h>

void ConfigurationWindow::draw() 
{
    float currentFPS = ImGui::GetIO().Framerate;
    updateFPS(currentFPS);
    memoryConsumption = 512.0f;

    if (ImGui::Begin(name.c_str())) 
    {
        ImGui::Text("Frames per second:");
        ImGui::PlotLines("##FPS", fpsHistory.data(), fpsHistory.size(), 0, nullptr, 0.0f, 100.0f, ImVec2(0, 80));

        ImGui::Text("Memory consumption: %.2f MB", memoryConsumption);

        ImGui::Separator();
        ImGui::Text("Hardware Information:");
        ImGui::Text("CPU cores: %d", SDL_GetCPUCount());
        ImGui::Text("RAM: %d MB", SDL_GetSystemRAM());

        ImGui::Separator();
        ImGui::Text("Software Versions:");
        ImGui::Text("SDL version: %d.%d.%d", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_PATCHLEVEL);
        ImGui::Text("OpenGL version: %s", glewGetString(GL_VERSION));
        ImGui::Text("DevIL version: %d.%d.%d", IL_VERSION / 100, (IL_VERSION / 10) % 10, IL_VERSION % 10);
    }
    ImGui::End();
}
