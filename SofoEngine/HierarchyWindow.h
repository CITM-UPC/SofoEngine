#pragma once
#include "GUIwindow.h"

class GraphicObject;

class HierarchyWindow : public GUIwindow 
{
public:
	HierarchyWindow(std::string name) : GUIwindow(name) {}
	void draw() override;
	void RecurseShowChildren(GraphicObject& parent);
};

