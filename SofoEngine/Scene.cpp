#include "Scene.h"
#include <unordered_set>

std::string Scene::GenerateUniqueName(const std::string& baseName, const GameObject* go)
{
	std::unordered_set<std::string> existingNames;
	std::string newName = baseName;
	auto children = go ? go->parent().children() : scene.children();

	for (const auto& child : children)
		existingNames.insert(child.getName());

	if (existingNames.count(newName) > 0)
	{
		int count = 1;
		while (existingNames.count(newName) > 0)
		{
			// Check if the name already ends with a number in parentheses
			size_t pos = newName.find_last_of('(');
			if (pos != std::string::npos && newName.back() == ')' && newName[pos] == '(' && pos > 0)
			{
				// Extract the number, increment it, and update the newName
				int num = std::stoi(newName.substr(pos + 1, newName.size() - pos - 2));
				newName = newName.substr(0, pos - 1) + " (" + std::to_string(++num) + ")";
			}
			else
			{
				newName = baseName + " (" + std::to_string(count++) + ")";
			}
		}
	}

	return newName;
}

void Scene::Init()
{
	scene.setName("Scene");
	scene.AddComponent<Transform>();
	editorCameraGO.AddComponent<Transform>();
	editorCameraGO.AddComponent<Camera>();
	editorCameraGO.setName("Editor Camera");
	editorCamera = editorCameraGO.GetComponent<Camera>();
}

void Scene::Shutdown()
{
}

void Scene::Reparent(GameObject& originalGO, GameObject& newParentGO)
{
	//for (auto& go : originalGO.parent().children())
	//{
	//	if (go == originalGO)
	//	{
	//		//GraphicObject parent = originalGO.parent();
	//		

	//		//GraphicObject original(originalGO);

	//		originalGO.parent().emplaceChild(newParentGO);

	//		newParentGO.emplaceChild(originalGO);

	//		originalGO.parent().removeChild(originalGO);


	//		break;
	//	}

	//}
}

GameObject& Scene::CreateEmpty(std::string name)
{
	GameObject& empty = scene.emplaceChild();
	empty.setName(GenerateUniqueName(name));
	empty.AddComponent<Transform>();
	return empty;
}

GameObject& Scene::Duplicate(GameObject& originalGO)
{
	GameObject duplicate = originalGO.Clone();
	duplicate.setName(GenerateUniqueName(originalGO.getName(), &originalGO));

	originalGO.parent().emplaceChild(std::move(duplicate));

	return duplicate;
}
