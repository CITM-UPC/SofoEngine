#pragma once

#include "MyWindow.h"
#include "vector"
#include "GUIwindow.h"

class MyGUI : public IEventProcessor
{
public:
	MyGUI(SDL_Window* window, void* context);
	MyGUI(MyGUI&&) noexcept = delete;
	MyGUI(const MyGUI&) = delete;
	MyGUI& operator=(const MyGUI&) = delete;
	~MyGUI();
	void render();
	void Begin();
	void End();

	void processEvent(const SDL_Event& event) override;

	void MainMenuHelp();

	std::vector<GUIwindow*> GUIwindows;

};
