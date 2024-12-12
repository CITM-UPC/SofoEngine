#pragma once
#include "Transform.h"
#include "TreeExt.h"
#include <string>
#include <vector>
#include "Component.h"
#include "BoundingBox.h"

class GameObject : public TreeExt<GameObject> {
private:
    std::string name = "";
    std::vector<std::unique_ptr<Component>> components;
    bool enabled;
    BoundingBox aabb;

public:

    GameObject(std::string name = "gameObject");
    ~GameObject();

    GameObject(const GameObject&) = delete;
    GameObject& operator=(const GameObject&) = delete;

    GameObject(GameObject&& other) noexcept; 
    
    GameObject& operator=(GameObject&& other) noexcept {
        if (this != &other) {
            TreeExt::operator=(std::move(other));
            name = std::move(other.name);
            components = std::move(other.components);
            enabled = other.enabled;
            aabb = std::move(other.aabb);
        }
        return *this;
    }


    void update();

	void draw();

    BoundingBox localBoundingBox() const;
    BoundingBox boundingBox() const { return GetComponent<Transform>()->GetLocalTransform() * localBoundingBox(); }

    BoundingBox worldBoundingBox() const;


	bool isEnabled() const { return enabled; }
	void enable() { enabled = true; }
	void disable() { enabled = false; }

    void destroy();

	std::string getName() const { return name; }
	void setName(const std::string& name) { this->name = name; }

    //json SaveGameObject();
    //void LoadGameObject(const json& gameObjectJSON);

    // Components
    template <typename TComponent>
    TComponent* GetComponent()
    {
        for (const auto& component : components)
        {
            if (dynamic_cast<TComponent*>(component.get()))
                return static_cast<TComponent*>(component.get());
        }
        return nullptr;
    }

    template <typename TComponent>
    TComponent* GetComponent() const
    {
        for (const auto& component : components)
        {
            if (dynamic_cast<TComponent*>(component.get()))
                return static_cast<TComponent*>(component.get());
        }
        return nullptr;
    }

    std::vector<Component*> GetAllComponents();

    GameObject Clone();

    template <typename TComponent>
    std::vector<TComponent*> GetComponents() const
    {
        std::vector<TComponent*> matchingComponents;
        for (const auto& component : components)
        {
            if (TComponent* castedComponent = dynamic_cast<TComponent*>(component.get())) {
                matchingComponents.push_back(castedComponent);
            }
        }
        return matchingComponents;
    }

    template <typename TComponent, typename... Args>
    bool AddComponent(Args&&... args)
    {
        Component* component = this->GetComponent<TComponent>();

        // Check for already existing Component
        if (component)
        {
            return false;
        }

        std::unique_ptr<Component> newComponent = std::make_unique<TComponent>(this, std::forward<Args>(args)...);
        newComponent->enable();
        components.push_back(std::move(newComponent));

        return true;
    }

    template <typename TComponent>
    bool AddCopiedComponent(TComponent* ref)
    {
        Component* component = this->GetComponent<TComponent>();

        // Check for already existing Component
        if (component != nullptr)
        {
            return false;
        }

        std::unique_ptr<Component> newComponent = std::make_unique<TComponent>(this, ref);
        newComponent->enable();
        components.push_back(std::move(newComponent));

        return true;
    }

    template <typename TComponent>
    void RemoveComponent(TComponent* compToRemove)
    {
        auto it = std::find_if(components.begin(), components.end(),
            [compToRemove](const std::unique_ptr<Component>& comp) {
                return comp.get() == compToRemove;
            });

        if (it != components.end())
            components.erase(it);
    }

    bool operator==(const GameObject& g) const {
        return name == g.name;
    }
};



