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


    void Reparent(GraphicObject& originalGO, GraphicObject& newParentGO);
    GraphicObject& CreateEmpty(std::string name = "Empty GameObject");
    GraphicObject& Duplicate(GraphicObject& originalGO);

    Camera camera;
    GraphicObject scene;
    GraphicObject* selectedGO = nullptr;

private:
    std::string GenerateUniqueName(const std::string& baseName, const GraphicObject* go = nullptr);
    Scene() : selectedGO(nullptr) {}
};

