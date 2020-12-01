#include "Widgets/AssetBrowser.h"
#include "Core/ResourceManager.h"
#include "IconsFontAwesome5.h"

#include <imgui.h>
#include <imgui_internal.h>

static std::string icon;

static std::map<std::string, std::string> extToIconMap
{
    {".lua", ICON_FA_FILE},
    {".cf", ICON_FA_FILE},
    {".prefab", ICON_FA_FILE},
};

static std::string MapExtToIcon(std::string ext)
{
    if (extToIconMap.find(ext) != extToIconMap.end())
    {
        return extToIconMap.at(ext);
    }

    return ICON_FA_FILE;
}

AssetBrowser::AssetBrowser()
{
    // TODO save the last visited path into some engine meta file
    // so that browser opens back where it left off
    currPath = ASSETS;
}

void AssetBrowser::OnImGuiRender(bool* isOpen)
{
    static float scale = 3.0f;
    static bool isList = false;
    uint32_t size = 40;

    // Left column -- displays directory list
    ImGui::Begin("Assets", isOpen);

    float contentWidth = ImGui::GetContentRegionAvail().x;
    float contentHeight = ImGui::GetContentRegionAvail().y;

    ImGui::Columns(2, "assetColumns");
    ImGui::SetColumnWidth(0, contentWidth * 0.3f);
    ImGui::BeginChild("Directory", ImGui::GetContentRegionAvail(), true);
    {
        RecursivelyDisplayDirectories(std::filesystem::path(ASSETS));
    }
    ImGui::EndChild();
    ImGui::NextColumn();
    ImGui::SameLine();

    // Left column -- displays contents of selected directory
    ImGui::BeginChild("Content", ImGui::GetContentRegionAvail(), true);
    {
        ImGui::Checkbox("List View", &isList);

        ImGui::SameLine();

        // Scaling size for buttons
        ImGui::SliderFloat("Scale", &scale, 1.0f, 3.0f);
        ImGui::Separator();
        ImVec2 buttonSize(size * scale, size * scale);

        ImGuiStyle& style = ImGui::GetStyle();
        float windowVisibleX2 = ImGui::GetWindowPos().x + contentWidth;

        int buttonCount = 0;
        for (auto& p : std::filesystem::directory_iterator(currPath))
        {
            buttonCount++;
        }

        if (!isList)
        {
            int numColumns = ImGui::GetContentRegionAvail().x / buttonSize.x;
            numColumns = numColumns > 0 ? numColumns : 1;
            ImGui::Columns(numColumns, nullptr, false);
        }

        int n = 0;
        for (auto& p : std::filesystem::directory_iterator(currPath))
        {
            ImGui::PushID(n);
            std::string filename = p.path().filename().string();

            //if (ImGui::Selectable(filename.c_str(), n == mSelectedItem))
            //{
            //    mSelectedItem = n;
            //}
            //ImGui::SameLine();

            if (isList) // tabulated
            {
                std::string icon = std::filesystem::is_directory(p.path())
                    ? ICON_FA_FOLDER
                    : ICON_FA_FILE
                ;
                ImGui::Text(icon.c_str());
                ImGui::SameLine();
                if (ImGui::Button(filename.c_str()))
                {
                    if (std::filesystem::is_directory(p.path()))
                    {
                        currPath = std::filesystem::relative(p.path());
                    }
                    else
                    {
                        // TODO display in assetViewer
                    }
                }
            }
            else // display icons
            {
                size_t dot = filename.find_last_of(".");
                std::string ext;
                if (dot != std::string::npos)
                {
                    ext = filename.substr(dot, filename.size() - dot);
                }

                ImGui::BeginGroup();

                // Texture files
                if (
                    ext == ".jpg"
                 || ext == ".png"
                ) {
                    auto texture = ResourceManager::GetTexture2D(p.path().string());
                    ImGui::ImageButton((ImTextureID)texture->GetRenderID(), buttonSize, ImVec2(0,1), ImVec2(1,0));
                }
                // Directories
                else if (std::filesystem::is_directory(p.path()))
                {
                    if (ImGui::Button(ICON_FA_FOLDER, buttonSize))
                    {
                        LOG_INFO(p.path().string());
                        currPath = std::filesystem::relative(p.path());
                    }
                }
                else
                {
                    ImGui::Button(MapExtToIcon(ext).c_str(), buttonSize);
                    {
                    }
                }
                ImGui::TextWrapped(filename.c_str());
            }

            if (!isList)
            {
                ImGui::EndGroup();
                ImGui::NextColumn();
            }
            ImGui::PopID();

            n++;
        }
    }
    ImGui::EndChild();

    ImGui::End();
}

void AssetBrowser::RecursivelyDisplayDirectories(std::filesystem::path dirPath)
{
    static ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

    //static int selection_mask = (1 << 2); // Dumb representation of what may be user-side selection state. You may carry selection state inside or outside your objects in whatever format you see fit.
    //const bool is_selected = (selection_mask & (1 << i)) != 0;
    //if (is_selected)
    //    flags |= ImGuiTreeNodeFlags_Selected;

    if (!std::filesystem::is_directory(dirPath))
    {
        return;
    }

    ImGui::Text(ICON_FA_FOLDER); ImGui::SameLine();
    // Not sure why this isnt working outside the recursive function
    if (dirPath.string() == ASSETS) {
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    }
    bool nodeOpen = ImGui::TreeNodeEx(dirPath.filename().string().c_str(), flags);
    // Update content view ImGuiwith selected directory
    //ImGui::Selectable()

    if (ImGui::IsItemClicked()
        && (ImGui::GetMousePos().x - ImGui::GetItemRectMin().x)
        > ImGui::GetTreeNodeToLabelSpacing()
    ){
        currPath = std::filesystem::relative(dirPath);
    }

    if (nodeOpen)
    {
        for (auto& p : std::filesystem::directory_iterator(dirPath))
        {
            RecursivelyDisplayDirectories(std::filesystem::relative(p.path()));
        }
        ImGui::TreePop();
    }
}
