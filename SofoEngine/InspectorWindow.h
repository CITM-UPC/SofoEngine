#pragma once
#include "GUIwindow.h"

class InspectorWindow : public GUIwindow
{
public:
    InspectorWindow(std::string name) : GUIwindow(name) {}
    void draw();

};

