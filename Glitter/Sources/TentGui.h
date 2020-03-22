#ifndef TENT_GUI_H
#define TENT_GUI_H

// ImGui headers
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "ObjectManager.h"
#include "LightManager.h"

class TentGui
{
public:
    void Init(GLFWwindow*);

    void RenderGUI(ObjectManager&);

    // =================================
    // Scene related
    void ShowSceneHierarchy(ObjectManager&);
    void ShowPrimitiveGenerator(ObjectManager&);
    void ShowObjects(ObjectManager&);
    void ShowLights(LightManager);
    void ShowInspector(GlObject*);
    void ShowInspector(Light*);
    // =================================

    void ShowRenderPasses();
    void ShowMetrics();

    bool isEnabled = 1;

};

#endif // TENT_GUI_H
