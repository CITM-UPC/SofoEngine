#include "HierarchyWindow.h"
#include "imgui.h"
#include <memory>
#include "GraphicObject.h"
#include "Scene.h"
#include <random>
#include <iostream>

std::string generateRandomNameFromBase(const std::string& baseName, size_t numDigits = 4) {
	std::default_random_engine rng(std::random_device{}());
	std::uniform_int_distribution<> dist(0, 9);

	std::string randomName = baseName + "_";
	for (size_t i = 0; i < numDigits; ++i) {
		randomName += std::to_string(dist(rng));
	}
	return randomName;
}

void HierarchyWindow::draw()
{

	unsigned int treeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;

	if (ImGui::Begin(name.c_str()))
	{
		if (ImGui::TreeNodeEx(Scene::get().scene.getName().c_str(), treeFlags))
		{
			RecurseShowChildren(Scene::get().scene);
			ImGui::TreePop();
		}
	}
	ImGui::End();

}

void HierarchyWindow::RecurseShowChildren(GraphicObject& parent)
{
	for (GraphicObject& childGO : parent.children())
	{
		unsigned int treeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

		if (childGO.children().empty())
			treeFlags |= ImGuiTreeNodeFlags_Leaf;

		if (Scene::get().selectedGO != nullptr && childGO == *Scene::get().selectedGO)
		{
			treeFlags |= ImGuiTreeNodeFlags_Selected;
		}

		

		bool isOpen = ImGui::TreeNodeEx(childGO.getName().c_str(), treeFlags);

		if (ImGui::IsItemClicked(0) && !ImGui::IsItemToggledOpen())
		{
			Scene::get().selectedGO = &childGO;
			std::cout << "Selected GameObject: " << childGO.getName() << Scene::get().selectedGO->getName() << std::endl;
		}

		if (isOpen)
		{
			treeFlags &= ~ImGuiTreeNodeFlags_Selected;
			RecurseShowChildren(childGO);

			ImGui::TreePop();
		}

	}
}
