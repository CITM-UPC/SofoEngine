#include "Component.h"
#include "GameObject.h"

Component::Component(GameObject* containerGO, ComponentType type) :
	type(type),
	enabled(true)
{
	gameObject = containerGO;
	createName();
}

Component::Component(GameObject* containerGO, Component* ref, ComponentType type) :
	type(type),
	enabled(ref->enabled)
{
	gameObject = containerGO;
	createName();
}

Component::~Component()
{
	disable();
}

void Component::createName()
{
	switch (type)
	{
	case ComponentType::Transform: name = "Transform"; break;
	case ComponentType::Camera: name = "Camera"; break;
	case ComponentType::Mesh: name = "Mesh"; break;
	case ComponentType::Texture: name = "Texture"; break;
	case ComponentType::Unknown: name = "Unknown"; break;
	default: name = "Specified Type was not found"; break;
	}
}
