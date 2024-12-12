#pragma once
#include "Camera.h"
#include "GameObject.h"


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

	void Init();
	void Shutdown();

    void Reparent(GameObject& originalGO, GameObject& newParentGO);
    GameObject& CreateEmpty(std::string name = "Empty GameObject");
    GameObject& Duplicate(GameObject& originalGO);

    GameObject editorCameraGO;
	Camera* editorCamera;
    GameObject scene;
    GameObject* selectedGO = nullptr;

private:
    std::string GenerateUniqueName(const std::string& baseName, const GameObject* go = nullptr);

    Scene() : selectedGO(nullptr), editorCamera(nullptr) {}
};

