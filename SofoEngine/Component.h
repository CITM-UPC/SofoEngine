#pragma once
#include <string>
#include <memory>

class GameObject;

enum class ComponentType
{
    Transform = 0,
    Camera,
    Mesh,
    Texture,

    Unknown
};

class Component
{
public:

    Component(GameObject* containerGO, ComponentType type);
    Component(GameObject* containerGO, Component* ref, ComponentType type);
    virtual ~Component();

	void enable() { enabled = true; }
	void disable() { enabled = false; }
	bool isEnabled() { return enabled; }

    virtual void update() {}

	std::string getName() { return name; }
    void createName();

	ComponentType GetType() const { return type; }
	GameObject* GetContainerGO() const { return gameObject; }

    //virtual json SaveComponent() = 0;
    //virtual void LoadComponent(const json& transformJSON) = 0;

protected:
    GameObject* gameObject;
    ComponentType type;
	std::string name;
private:
    bool enabled;
};

