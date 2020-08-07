#include <imgui.h>
#include "Editor/Widgets/HierarchyWidget.h"
#include "Core/Input.h"

static void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

void HierarchyWidget::ShowHierarchy(SharedPtr<Scene>& activeScene, bool* isOpen)
{
    ImGui::Begin("Scene Hierarchy", isOpen);

    // TODO cleanup later
    static ImGuiTextFilter filter;
    const char* filterHelp(
                "Filter usage:\n"
                "  \"\"         display all lines\n"
                "  \"xxx\"      display lines containing \"xxx\"\n"
                "  \"xxx,yyy\"  display lines containing \"xxx\" or \"yyy\"\n"
                "  \"-xxx\"     hide lines containing \"xxx\""
            );

    // Create new entity
    if (ImGui::Button("+"))
    {
        ImGui::OpenPopup("NewEntityPopup");
    }
    // Also open New Entity menu when right clicking into hierarchy
    if (ImGui::BeginPopupContextWindow(0, 1, false))
    {
        ShowNewEntityMenu(activeScene);
        ImGui::EndPopup();
    }
    if (ImGui::BeginPopup("NewEntityPopup"))
    {
        ShowNewEntityMenu(activeScene);
        ImGui::EndPopup();
    }

    //HelpMarker(filterHelp);
    ImGui::SameLine(); filter.Draw();

    ImGui::Separator();

    static int selected = -1;
    int i = 0;
    for (auto entityPair : activeScene->GetEntityMap())
    {
        std::string tag = entityPair.second.GetComponent<TagComponent>().tag;
        if (filter.PassFilter(tag.c_str()))
        {
            char buf[128];
            // FIXME: adding an index into tag since duplicate tags
            // cause us to only be able to select the first of that matching tag
            sprintf(buf, "%d. %s", i, tag.c_str());
            if (ImGui::Selectable(buf, selected == i))
            {
                // Open inspector for selected object
                selected = i;
                selectedEntity = entityPair.second;
                hasSelectedEntity = true;
            }
        }
        ++i;
    }

    if (Input::GetKeyDown(KEY_DELETE) && selected != -1)
    {
        activeScene->RemoveEntity(selectedEntity);
        hasSelectedEntity = false;
    }

    if (hasSelectedEntity)
    {
       wInspector.ShowInspector(selectedEntity, isOpen);
    }

    ImGui::End();
}

void HierarchyWidget::ShowNewEntityMenu(SharedPtr<Scene>& activeScene)
{
    if (ImGui::MenuItem("Create Empty"))
    {
        activeScene->CreateEntity("Empty");
    }

    if (ImGui::BeginMenu("3D Object"))
    {
        if (ImGui::MenuItem("Cube"))
        {
            auto& entity = activeScene->CreateEntity("Cube");
            entity.AddComponent<MeshComponent>(MeshComponent::Geometry::CUBE);
        }
        if (ImGui::MenuItem("Sphere"))
        {
            auto& entity = activeScene->CreateEntity("Sphere");
            entity.AddComponent<MeshComponent>(MeshComponent::Geometry::SPHERE);
        }
        if (ImGui::MenuItem("Plane"))
        {
            auto& entity = activeScene->CreateEntity("Plane");
            entity.AddComponent<MeshComponent>(MeshComponent::Geometry::PLANE);
        }
        if (ImGui::MenuItem("Quad"))
        {
            auto& entity = activeScene->CreateEntity("Quad");
            entity.AddComponent<MeshComponent>(MeshComponent::Geometry::QUAD);
        }
        if (ImGui::MenuItem("Cone"))
        {
            auto& entity = activeScene->CreateEntity("Cone");
            entity.AddComponent<MeshComponent>(MeshComponent::Geometry::CONE);
        }
        if (ImGui::MenuItem("Cylinder"))
        {
            auto& entity = activeScene->CreateEntity("Cylinder");
            entity.AddComponent<MeshComponent>(MeshComponent::Geometry::CYLINDER);
        }
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("2D Object"))
    {
        if (ImGui::MenuItem("Sprite"))
        {
        }
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Effects"))
    {
        if (ImGui::MenuItem("Particle System"))
        {
            auto& entity = activeScene->CreateEntity("Particle System");
            entity.AddComponent<ParticleSystemComponent>();
        }
        ImGui::EndMenu();
    }

    // TODO
    if (ImGui::BeginMenu("Light"))
    {
        if (ImGui::MenuItem("Directional Light"))
        {
            auto& entity = activeScene->CreateEntity("Directional Light");
            entity.AddComponent<LightComponent>(LightComponent::LightType::DIRECTIONAL);
        }
        if (ImGui::MenuItem("Point Light"))
        {
            auto& entity = activeScene->CreateEntity("Point Light");
            entity.AddComponent<LightComponent>(LightComponent::LightType::POINT);
        }
        if (ImGui::MenuItem("Spot Light"))
        {
            auto& entity = activeScene->CreateEntity("Spot Light");
            entity.AddComponent<LightComponent>(LightComponent::LightType::SPOT);
        }
        if (ImGui::MenuItem("Area Light"))
        {
            auto& entity = activeScene->CreateEntity("Area Light");
            entity.AddComponent<LightComponent>(LightComponent::LightType::AREA);
        }

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Audio"))
    {
        // Just an object with an audio component attached
        // with transform and tag
        if (ImGui::MenuItem("Audio Source"))
        {
        }
        ImGui::EndMenu();
    }

    // TODO
    if (ImGui::BeginMenu("UI"))
    {
        ImGui::EndMenu();
    }
}