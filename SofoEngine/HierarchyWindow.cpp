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

		/*if (ReparentDragDrop(childGO))
		{
			ImGui::TreePop();
			break;
		}*/

		ContextMenu(childGO);
		if (duplicate)
		{
			ImGui::TreePop();
			duplicate = false;
			break;
		}
		if (createEmpty)
		{
			ImGui::TreePop();
			createEmpty = false;
			break;
		}
		if (remove)
		{
			ImGui::TreePop();
			break;
		}

		if (isOpen && !reparent)
		{
			treeFlags &= ~ImGuiTreeNodeFlags_Selected;
			RecurseShowChildren(childGO);

			ImGui::TreePop();
		}

		/*if (remove)
		{
			for (auto& item : toDeleteList)
			{
				item.Delete();
			}

			remove = false;
		}*/

	}
}


void HierarchyWindow::ContextMenu(GraphicObject& go)
{
	if (ImGui::BeginPopupContextItem())
	{
		if (ImGui::MenuItem("Create Empty"))
		{
			createEmpty = true;
			Scene::get().Reparent(go, Scene::get().CreateEmpty("Parent of " + go.getName()));
		}

		if (ImGui::MenuItem("Duplicate"))
		{
			duplicate = true;
			Scene::get().selectedGO = &Scene::get().Duplicate(go);
			//LOG(LogType::LOG_INFO, "%s has been duplicated", go.get()->GetName().c_str());
		}

		//if (ImGui::MenuItem("Remove"))
		//{
		//	remove = true;
		//	//LOG(LogType::LOG_INFO, "Use Count: %d", go.use_count());
		//	engine->N_sceneManager->SetSelectedGO(nullptr);

		//	toDeleteList.push_back(go);

		//	//go.get()->Delete();
		//	//go.get()->Disable();
		//}

		ImGui::EndPopup();
	}
}

//bool HierarchyWindow::ReparentDragDrop(GraphicObject& childGO)
//{
//	if (ImGui::BeginDragDropSource())
//	{
//		if (childGO != Scene::get().scene)
//		{
//			ImGui::SetDragDropPayload(Scene::get().selectedGO->getName().c_str(), &childGO, sizeof(GraphicObject));
//		}
//
//		ImGui::EndDragDropSource();
//	}
//
//	if (ImGui::BeginDragDropTarget())
//	{
//		if (Scene::get().selectedGO)
//		{
//			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload((Scene::get().selectedGO->getName().c_str())))
//			{
//				GraphicObject* dragging = *(GraphicObject**)payload->Data;
//				GraphicObject* currentParent = &childGO;
//
//				while (currentParent)
//				{
//					if (currentParent == dragging)
//						return false;
//
//					currentParent = &currentParent->parent();
//				}
//
//				GraphicObject* oldParent = &dragging->parent();
//				dragging->parent() = childGO;
//				GraphicObject newChild = *dragging;
//
//				if (oldParent != nullptr)
//				{
//					oldParent->removeChild(newChild);
//				}
//
//				childGO.emplaceChild(newChild);
//				reparent = true;
//			}
//		}
//
//		ImGui::EndDragDropTarget();
//	}
//
//	return reparent;
//}

