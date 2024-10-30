#pragma once
#include "GUIwindow.h"

class AboutWindow : public GUIwindow
{
public:
	AboutWindow(std::string name) : GUIwindow(name) {}
	void draw() override;
};

