#include "MyGUI.h"
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_opengl.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>
#include "HierarchyWindow.h"
#include "AboutWindow.h"
#include "ConfigurationWindow.h"
#include "InspectorWindow.h"
#include <SDL2/SDL_misc.h>
#include "Scene.h"

MyGUI::MyGUI(SDL_Window* window, void* context) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(window, context);
    ImGui_ImplOpenGL3_Init();

	GUIwindows.push_back(new HierarchyWindow("Hierarchy"));
	GUIwindows.push_back(new AboutWindow("About"));
	GUIwindows.push_back(new ConfigurationWindow("Configuration"));
	GUIwindows.push_back(new InspectorWindow("Inspector"));
}

MyGUI::~MyGUI() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}

void MyGUI::render()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Edit"))
		{
			MainMenuEdit();
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("GameObject"))
		{
			MainMenuGameObject();
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			MainMenuHelp();
			ImGui::EndMenu();
		}

		ImGui::Dummy(ImVec2(ImGui::GetContentRegionAvail().x / 2 -100, 0.0f));
		if (ImGui::Button(" > "))  Scene::get().playState = PlayState::PLAYING;
		if (ImGui::Button(" [] "))  Scene::get().playState = PlayState::STOPPED;

		ImGui::EndMainMenuBar();
	}

	for (auto& window : GUIwindows)
	{
		window->draw();
	}
}

void MyGUI::Begin()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
}

void MyGUI::End()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void MyGUI::processEvent(const SDL_Event& event) {
	ImGui_ImplSDL2_ProcessEvent(&event);
}

void MyGUI::MainMenuEdit()
{
	if (ImGui::MenuItem("Undo", "Ctrl+Z", false, false)) {}
	if (ImGui::MenuItem("Redo", "Ctrl+Y", false, false)) {}

	ImGui::Separator();

	if (ImGui::MenuItem("Cut", "Ctrl+X", false, false)) {}
	if (ImGui::MenuItem("Copy", "Ctrl+C", false, false)) {}
	if (ImGui::MenuItem("Paste", "Ctrl+V", false, false)) {}
}

void MyGUI::MainMenuGameObject()
{
	if (ImGui::MenuItem("Create Empty")) { Scene::get().CreateEmpty(); }
}

void MyGUI::MainMenuHelp()
{
	if (ImGui::MenuItem("Documentation"))
	{
		const char* url = "https://github.com/Sofialiles55/SofoEngine";

		if (SDL_OpenURL(url) != 0) {}
	}
}