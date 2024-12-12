#pragma once
#include "GUIwindow.h"
#include <vector>

class GameObject;

class HierarchyWindow : public GUIwindow 
{
public:
	HierarchyWindow(std::string name) : GUIwindow(name) {}
	void draw() override;
	void RecurseShowChildren(GameObject& parent);
	void ContextMenu(GameObject& parent);
	//bool ReparentDragDrop(GraphicObject& childGO);

private:
	bool reparent = false;
	bool remove = false;
	bool duplicate = false;
	bool createEmpty = false;

	std::vector<GameObject*> toDeleteList;
};

