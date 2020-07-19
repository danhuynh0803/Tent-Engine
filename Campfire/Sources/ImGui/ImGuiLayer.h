#ifndef IMGUI_LAYER_H
#define IMGUI_LAYER_H

#include "Core/Layer.h"

// ImGui headers
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "ImGuizmo.h"
//#include "imfilebrowser.h"

class ImGuiLayer : public Layer
{
public:
    ImGuiLayer();
    ~ImGuiLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnEvent(Event& event) override;

    void Begin(); // Start of frame
    void End();   // End of frame

private:
    float time = 0.0f;
};

#endif // IMGUI_LAYER_H
