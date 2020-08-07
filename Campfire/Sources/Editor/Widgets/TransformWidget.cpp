#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include "Editor/Widgets/TransformWidget.h"
#include "Core/Input.h"

void TransformWidget::EditTransform(Entity& entity, const Camera& editorCamera)
{
    if (!entity.HasComponent<TransformComponent>())
    {
        // Don't draw Gizmo if transform componenet not present
        return;
    }

    if (!Input::GetMouseButtonDown(MOUSE_BUTTON_RIGHT))
    {
        if (Input::GetKeyDown(KEY_W))
            operation = ImGuizmo::TRANSLATE;
        if (Input::GetKeyDown(KEY_E))
            operation = ImGuizmo::ROTATE;
        if (Input::GetKeyDown(KEY_R))
            operation = ImGuizmo::SCALE;
    }

    ImGuiIO& io = ImGui::GetIO();
    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

    float* viewMatrix = const_cast<float*>(glm::value_ptr(editorCamera.GetViewMatrix()));
    float* projMatrix = const_cast<float*>(glm::value_ptr(editorCamera.GetProjMatrix()));

    auto& transformComp = entity.GetComponent<TransformComponent>();
    float* transform = const_cast<float*>(glm::value_ptr(transformComp.GetTransform()));

    ImGuizmo::Manipulate(
        viewMatrix,
        projMatrix,
        operation,
        mode,
        transform,
        nullptr,                                // delta matrix
        useSnap ? &snap[0] : nullptr,           // snap
        boundSizing ? bounds : NULL,            // local bounds
        boundSizingSnap ? boundsSnap : NULL     // bounds snap
    );

    // Update transform component's data
    float newTranslation[3], newRotation[3], newScale[3];
    ImGuizmo::DecomposeMatrixToComponents(transform, newTranslation, newRotation, newScale);
    transformComp.position = glm::make_vec3(newTranslation);
    transformComp.rotation = glm::make_vec3(newRotation);
    transformComp.scale    = glm::make_vec3(newScale);
}

void TransformWidget::ShowTransformSettings(bool* isOpen)
{
    ImGui::Begin("Move Tool", isOpen);

    ImGui::NewLine();

    if (operation != ImGuizmo::SCALE)
    {
        if (ImGui::RadioButton("Local", mode == ImGuizmo::LOCAL))
            mode = ImGuizmo::LOCAL;
        ImGui::SameLine();
        if (ImGui::RadioButton("World", mode == ImGuizmo::WORLD))
            mode = ImGuizmo::WORLD;
    }

    ImGui::NewLine();
    ImGui::Separator();
    ImGui::NewLine();

    ImGui::Text("Snap Settings (Enable with CTRL)");
    ImGui::Checkbox("", &useSnap);
    useSnap = (Input::GetMod(MOD_CONTROL)) ? true : false;
    ImGui::SameLine();
    switch (operation)
    {
        case ImGuizmo::TRANSLATE:
            ImGui::InputFloat3("Snap", &snap[0]);
            break;
        case ImGuizmo::ROTATE:
            ImGui::InputFloat("Angle Snap", &snap[0]);
            break;
        case ImGuizmo::SCALE:
            ImGui::InputFloat("Scale Snap", &snap[0]);
            break;
    }

    ImGui::NewLine();
    ImGui::Separator();
    ImGui::NewLine();

    ImGui::Text("Bound Size Settings");
    ImGui::Checkbox("Enable Bound Sizing", &boundSizing);
    if (boundSizing)
    {
        ImGui::PushID(3);
        ImGui::Checkbox("", &boundSizingSnap);
        ImGui::SameLine();
        ImGui::InputFloat3("Snap", boundsSnap);
        ImGui::PopID();
    }

    ImGui::End();
}

