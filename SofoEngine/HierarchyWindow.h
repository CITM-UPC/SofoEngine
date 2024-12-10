#pragma once
#include "GUIwindow.h"
#include <vector>

class GraphicObject;

class HierarchyWindow : public GUIwindow 
{
public:
	HierarchyWindow(std::string name) : GUIwindow(name) {}
	void draw() override;
	void RecurseShowChildren(GraphicObject& parent);
	void ContextMenu(GraphicObject& parent);
	//bool ReparentDragDrop(GraphicObject& childGO);

private:
	bool reparent = false;
	bool remove = false;
	bool duplicate = false;
	bool createEmpty = false;

	std::vector<GraphicObject*> toDeleteList;
};

