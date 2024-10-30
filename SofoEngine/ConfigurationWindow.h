#pragma once
#include "GUIwindow.h"
#include <vector>

class ConfigurationWindow : public GUIwindow
{
public:
    ConfigurationWindow(std::string name) : GUIwindow(name) {}
    void draw();

private:
    std::vector<float> fpsHistory;
    const size_t fpsHistorySize = 100;
    float memoryConsumption = 0.0f;

    void updateFPS(float fps) {
        if (fpsHistory.size() >= fpsHistorySize) {
            fpsHistory.erase(fpsHistory.begin());
        }
        fpsHistory.push_back(fps);
    }
};

