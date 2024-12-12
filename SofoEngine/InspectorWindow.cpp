#include <iostream>
#include <string>
#include "Scene.h"
#include "InspectorWindow.h"
#include <imgui.h>
#include "Utils.h"
#include "Mesh.h"
#include "Texture.h"

void InspectorWindow::draw()
{
    if (Scene::get().selectedGO == nullptr || Scene::get().selectedGO->getName() == "")
    {
        ImGui::Begin("Inspector");
        ImGui::Text("No GameObject selected.");
        ImGui::End();
        return;
    }

	if (savedSelectedGO != Scene::get().selectedGO)
	{
		OnSelectGO();
		savedSelectedGO = Scene::get().selectedGO;
	}

    GameObject& selectedGO = *Scene::get().selectedGO;


    Transform* transform = selectedGO.GetComponent<Transform>();
    if (ImGui::Begin(name.c_str()))
    {
        ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_None | ImGuiTreeNodeFlags_DefaultOpen;

        ImGui::SameLine(); ImGui::Text("GameObject:");
        ImGui::SameLine(); ImGui::TextColored({ 0.144f, 0.422f, 0.720f, 1.0f }, selectedGO.getName().c_str());

        bool isEnabled = selectedGO.isEnabled();
        if (ImGui::Checkbox("Enable", &isEnabled)) {
            if (isEnabled != selectedGO.isEnabled())
            {
                if (isEnabled)
                    selectedGO.enable();
                else
                    selectedGO.disable();
            }
        }

        static char newNameBuffer[32];
        strcpy_s(newNameBuffer, sizeof(newNameBuffer), selectedGO.getName().c_str());
        if (ImGui::InputText("New Name", newNameBuffer, sizeof(newNameBuffer), ImGuiInputTextFlags_EnterReturnsTrue))
        {
            std::string newName(newNameBuffer);
            selectedGO.setName(newName);
            newNameBuffer[0] = '\0';
        }

        if (transform != nullptr && ImGui::CollapsingHeader("Transform", treeNodeFlags)) {
            ImGui::SetItemTooltip("Displays and sets game object transformations");

            view_pos = transform->GetLocalPosition();
            view_rot_deg = ToDegrees(view_rot_rad);
            //vec3f initialRotation = view_rot_deg;
            view_sca = transform->GetLocalScale();


            // Transform table ----------------------------------------------------------------------------------
            ImGuiTableFlags tableFlags = ImGuiTableFlags_Resizable;// | ImGuiTableFlags_SizingFixedFit;
            //ImGui::Indent(0.8f);
            if (ImGui::BeginTable("##transformTable", 4))
            {
                ImGui::TableSetupColumn("##title");
                ImGui::TableSetupColumn("##X");
                ImGui::TableSetupColumn("##Y");
                ImGui::TableSetupColumn("##Z");

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::PushItemWidth(-FLT_MIN);
                ImGui::TableSetColumnIndex(1);
                ImGui::PushItemWidth(-FLT_MIN);
                ImGui::TableSetColumnIndex(2);
                ImGui::PushItemWidth(-FLT_MIN);
                ImGui::TableSetColumnIndex(3);
                ImGui::PushItemWidth(-FLT_MIN);

                // Column 0: Labels
                ImGui::TableSetColumnIndex(0);
                ImGui::Dummy({ 0, 1 });
                ImGui::Text("Position");
                ImGui::Dummy({ 0, 4 });
                ImGui::Text("Rotation");
                ImGui::Dummy({ 0, 4 });
                ImGui::Text("Scale");

                // Column 1: X Axis
                ImGui::TableSetColumnIndex(1);
                ImGui::PushStyleColor(ImGuiCol_FrameBg, { 0.5f, 0.32f, 0.32f, 1.0f });
                ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, { 0.6f, 0.42f, 0.42f, 1.0f });
                ImGui::PushStyleColor(ImGuiCol_FrameBgActive, { 0.5f, 0.22f, 0.22f, 1.0f });
                if (ImGui::DragFloat("X##PosX", &view_pos.x, 0.5F, 0, 0, "%.3f"))       matrixChanged = true;
                if (ImGui::DragFloat("X##RotX", &view_rot_deg.x, 0.2f, 0, 0, "%.3f"))   matrixChanged = true;
                if (ImGui::DragFloat("X##ScaleX", &view_sca.x, 0.1F, 0, 0, "%.3f"))     matrixChanged = true;
                ImGui::PopStyleColor(3);

                // Column 2: Y Axis
                ImGui::TableSetColumnIndex(2);
                ImGui::PushStyleColor(ImGuiCol_FrameBg, { 0.32f, 0.5f, 0.32f, 1.0f });
                ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, { 0.42f, 0.6f, 0.42f, 1.0f });
                ImGui::PushStyleColor(ImGuiCol_FrameBgActive, { 0.22f, 0.5f, 0.22f, 1.0f });
                if (ImGui::DragFloat("Y##PosY", &view_pos.y, 0.5F, 0, 0, "%.3f"))       matrixChanged = true;
                if (ImGui::DragFloat("Y##RotY", &view_rot_deg.y, 0.2f, 0, 0, "%.3f"))   matrixChanged = true;
                if (ImGui::DragFloat("Y##ScaleY", &view_sca.y, 0.1F, 0, 0, "%.3f"))     matrixChanged = true;
                ImGui::PopStyleColor(3);

                // Column 3: Z Axis
                ImGui::TableSetColumnIndex(3);
                ImGui::PushStyleColor(ImGuiCol_FrameBg, { 0.32f, 0.32f, 0.5f, 1.0f });
                ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, { 0.42f, 0.42f, 0.6f, 1.0f });
                ImGui::PushStyleColor(ImGuiCol_FrameBgActive, { 0.22f, 0.22f, 0.5f, 1.0f });
                if (ImGui::DragFloat("Z##PosZ", &view_pos.z, 0.5F, 0, 0, "%.3f"))       matrixChanged = true;
                if (ImGui::DragFloat("Z##RotZ", &view_rot_deg.z, 0.2f, 0, 0, "%.3f"))   matrixChanged = true;
                if (ImGui::DragFloat("Z##ScaleZ", &view_sca.z, 0.1F, 0, 0, "%.3f"))     matrixChanged = true;
                ImGui::PopStyleColor(3);

                ImGui::EndTable();

                if (matrixChanged)
                {
                    transform->SetLocalPosition(view_pos);
                    view_rot_rad = ToRadians(view_rot_deg);
                    transform->SetLocalRotation(view_rot_rad);

                    transform->SetLocalScale(view_sca);
                }
            }
        }

        Mesh* mesh = selectedGO.GetComponent<Mesh>();
        if (mesh != nullptr && ImGui::CollapsingHeader("Mesh", treeNodeFlags))
        {

            ImGui::Text("Mesh vertices: %zu", mesh->vertices().size());
            ImGui::Text("Mesh indices: %zu", mesh->indices().size());

            static bool showTriangleNormals = true;
            static bool showFaceNormals = true;
            ImGui::Checkbox("Show triangle normals", &showTriangleNormals);
            ImGui::Checkbox("Show face normals", &showFaceNormals);

            if (showTriangleNormals)
            {
                mesh->drawTriangleNormals();
            }
            if (showFaceNormals)
            {
                mesh->drawFaceNormals();
            }
        }

        Texture* texture = selectedGO.GetComponent<Texture>();
        if (texture != nullptr && ImGui::CollapsingHeader("Texture", treeNodeFlags))
        {
            if (&texture->image() == nullptr)
            {
                ImGui::Text("No texture loaded.");
            }
            else
            {
                ImGui::Text("Texture size: %dx%d", texture->image().width(), texture->image().height());
            }

            static bool useCheckerTexture = false;
            ImGui::Checkbox("Use checker texture", &useCheckerTexture);

            if (useCheckerTexture)
            {
                texture->applyCheckerTexture();
            }
            else
            {
                texture->applyOriginalTexture();
            }
        }

        /*Camera Component*/
        Camera* camera = selectedGO.GetComponent<Camera>();

        if (camera != nullptr && ImGui::CollapsingHeader("Camera", treeNodeFlags))
        {
            bool isDirty = false;

            float fov = static_cast<float>(camera->fov);
            float aspect = static_cast<float>(camera->aspect);
            float zNear = static_cast<float>(camera->zNear);
            float zFar = static_cast<float>(camera->zFar);

            if (ImGui::SliderFloat("FOV", &fov, 20.0, 120.0))
            {
                camera->fov = fov;
                isDirty = true;
            }

            if (ImGui::SliderFloat("Aspect", &aspect, 0.1, 10.0))
            {
                camera->aspect = aspect;
                isDirty = true;
            }

            ImGui::Text("Clipping Planes");
            if (ImGui::SliderFloat("Near", &zNear, 0.01, 10.0))
            {
                camera->zNear = zNear;
                isDirty = true;
            }

            if (ImGui::SliderFloat("Far ", &zFar, 1.0, 10000.0))
            {
                camera->zFar = zFar;
                isDirty = true;
            }

            //ImGui::Checkbox("Draw Frustrum", &camera->drawFrustum);

            if (ImGui::Button("Primary Camera"))
            {
				Scene::get().mainCamera = &selectedGO;
            }

            if (isDirty) camera->UpdateCamera();

            ImGui::Dummy(ImVec2(0.0f, 10.0f));
        }


        if (ImGui::BeginMenu("Add Component"))
        {

            if (ImGui::MenuItem("Camera"))
            {
                selectedGO.AddComponent<Camera>();
            }

            ImGui::EndMenu();
        }
        ImGui::End();
    }
}

void InspectorWindow::OnSelectGO()
{
    view_rot_rad = Scene::get().selectedGO->GetComponent<Transform>()->GetRotationEuler();
}
