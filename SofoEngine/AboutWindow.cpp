#include "AboutWindow.h"
#include "imgui.h"
#include "SDL2/SDL.h"

void AboutWindow::draw()
{
	if (ImGui::Begin(name.c_str()))
	{
		ImGui::Text("SofoEngine by Sofia Liles.");
		ImGui::Text("Game Engines, CITM - UPC");

		if (ImGui::Button("Visit GitHub")) {
			const char* url = "https://github.com/Sofialiles55/SofoEngine";

			if (SDL_OpenURL(url) != 0) {
				SDL_Log("No se pudo abrir la URL: %s", SDL_GetError());
			}
		}
	}
	ImGui::End();

}
