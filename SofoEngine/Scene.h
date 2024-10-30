#pragma once
#include "Camera.h"
#include "GraphicObject.h"


class Scene
{
public:
    static Scene& get() {
        static Scene instance;
        return instance;
    }

    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;
    Scene(Scene&&) = delete;
    Scene& operator=(Scene&&) = delete;

    const GraphicObject& getSelectedGO() { return selectedGO; }
    void setSelectedGO(GraphicObject& go) { selectedGO = go; }

    Camera camera;
    GraphicObject scene;
    GraphicObject selectedGO;

private:
    Scene() = default;
};

