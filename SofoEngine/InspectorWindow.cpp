#include <iostream>
#include <string>
#include "Scene.h"
#include "InspectorWindow.h"
#include <imgui.h>

void InspectorWindow::draw()
{
    if (Scene::get().selectedGO == nullptr || Scene::get().selectedGO->getName() == "")
    {
        ImGui::Begin("Inspector");
        ImGui::Text("No GameObject selected.");
        ImGui::End();
        return;
    }

    GraphicObject& selectedGO = *Scene::get().selectedGO;

    if (ImGui::Begin(name.c_str())) 
    {
        ImGui::Text("Selected GameObject: %s", selectedGO.getName().c_str());
        ImGui::Separator();

        if (ImGui::CollapsingHeader("Transform")) {
            const auto& position = selectedGO.transform().pos();
            const auto& rotation = selectedGO.transform().rotation();
            const auto& scale = selectedGO.transform().scale();

            ImGui::Text("Position: (%.2f, %.2f, %.2f)", position.x, position.y, position.z);
            ImGui::Text("Rotation: (%.2f, %.2f, %.2f)", rotation.x, rotation.y, rotation.z);
            ImGui::Text("Scale: (%.2f, %.2f, %.2f)", scale.x, scale.y, scale.z);
        }

        if (selectedGO.hasMesh() && ImGui::CollapsingHeader("Mesh")) 
        {
            const Mesh& mesh = selectedGO.mesh();

            ImGui::Text("Mesh vertices: %zu", mesh.vertices());
            ImGui::Text("Mesh indices: %zu", mesh.indices());

            static bool showTriangleNormals = false;
            static bool showFaceNormals = false;
            ImGui::Checkbox("Show triangle normals", &showTriangleNormals);
            ImGui::Checkbox("Show face normals", &showFaceNormals);

            if (showTriangleNormals) 
            {
                mesh.drawTriangleNormals();
            }
            if (showFaceNormals) 
            {
                mesh.drawFaceNormals();
            }
        }

        if (selectedGO.hasTexture() && ImGui::CollapsingHeader("Texture")) 
        {
            const Texture& texture = selectedGO.texture();

            ImGui::Text("Texture size: %dx%d", texture.image().width(), texture.image().height());

            static bool useCheckerTexture = false;
            ImGui::Checkbox("Use checker texture", &useCheckerTexture);

            if (useCheckerTexture) 
            {
                selectedGO.texture().applyCheckerTexture();
            }
            else 
            {
                selectedGO.texture().applyOriginalTexture();
            }
        }
    }
    ImGui::End();
}
