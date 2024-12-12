#pragma once
#include "GUIwindow.h"
#include "glm/glm.hpp"

class InspectorWindow : public GUIwindow
{
public:
    InspectorWindow(std::string name) : GUIwindow(name) {}
    void draw();

	void OnSelectGO();

private:
	glm::vec3 view_pos;
	glm::vec3 view_rot_rad;
	glm::vec3 view_rot_deg;
	glm::vec3 view_sca;

	bool matrixChanged = false;

	GameObject* savedSelectedGO = nullptr;
};

