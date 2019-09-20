#pragma once
#include "scene_edit.h"
#include <quick_imgui.h>

namespace akane::gui
{
    class AkaneApp : public Application
    {
    public:
        void Initialize() override
        {
            // Enable docking in ImGui
            //
            auto& io = ImGui::GetIO();
            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
            // io.ConfigDockingWithShift = false;

            // Initialize editor instance
            //
            scene_edit_.Initialize();
        }
        void Update() override
        {
            UpdateDockspace();

            scene_edit_.Update();
        }

        void UpdateDockspace()
        {
            ImScoped::StyleVar style_1{ImGuiStyleVar_WindowRounding, 0.f};
            ImScoped::StyleVar style_2{ImGuiStyleVar_WindowBorderSize, 0.f};
            ImScoped::StyleVar style_3{ImGuiStyleVar_WindowPadding, ImVec2{0.0f, 0.0f}};

            auto window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
                                ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                                ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
                                ImGuiWindowFlags_NoNavFocus;
            ImGui::SetNextWindowMaximized();
            ImScoped::Window dock_window{"Dockspace Container", nullptr, window_flags};

            auto dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id);
        }

    private:
        SceneEditWindow scene_edit_;
        // RawEditWindow raw_edit_;
    };
} // namespace akane::gui
