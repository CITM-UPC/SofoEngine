#pragma once
#include <string>

class GUIwindow
{
public:
	GUIwindow(std::string n) : name(n) {}
	virtual void draw() = 0;
	std::string name;
};

