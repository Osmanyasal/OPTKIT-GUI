#include "skeleton/create_session.hh"

#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"
#include "misc/cpp/imgui_stdlib.cpp"

#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <string>
#include <system_error>
#include <vector>

namespace
{
    namespace fs = std::filesystem;

    void write_buffer(char *buffer, size_t buffer_size, const std::string &value)
    {
        std::snprintf(buffer, buffer_size, "%s", value.c_str());
    }

    std::string make_command_preview(const SessionDraft &draft)
    {
        std::string preview = draft.target_binary;
        if (preview.empty())
            preview = "<select a target binary>";

        if (!draft.target_arguments.empty())
        {
            preview += " ";
            preview += draft.target_arguments;
        }

        return preview;
    }

    std::string get_picker_directory(const AppState &state)
    {
        std::error_code error;
        fs::path directory = state.target_binary_picker_directory;
        if (directory.empty() || !fs::exists(directory, error) || !fs::is_directory(directory, error))
            directory = fs::current_path(error);

        fs::path normalized_directory = fs::absolute(directory, error);
        if (error)
            return directory.lexically_normal().string();

        return normalized_directory.lexically_normal().string();
    }

    bool is_executable_file(const fs::directory_entry &entry)
    {
        std::error_code error;
        if (!entry.is_regular_file(error))
            return false;

        const fs::perms permissions = entry.status(error).permissions();
        if (error)
            return false;

        return (permissions & fs::perms::owner_exec) != fs::perms::none ||
               (permissions & fs::perms::group_exec) != fs::perms::none ||
               (permissions & fs::perms::others_exec) != fs::perms::none;
    }

    bool clipboard_icon_button(const char *id, float button_size, AppTheme theme)
    {
        const bool clicked = ImGui::InvisibleButton(id, ImVec2(button_size, button_size));

        ImDrawList *draw_list = ImGui::GetWindowDrawList();
        const ImVec2 min_pos = ImGui::GetItemRectMin();
        const ImVec2 max_pos = ImGui::GetItemRectMax();
        const ImVec2 button_center((min_pos.x + max_pos.x) * 0.5f, (min_pos.y + max_pos.y) * 0.5f);
        const float rounding = button_size * 0.18f;
        const ImVec4 icon_tint = theme == AppTheme_Dark ? ImVec4(0.86f, 0.88f, 0.92f, 1.0f) : ImVec4(0.18f, 0.22f, 0.28f, 1.0f);
        const ImVec4 hover_tint = theme == AppTheme_Dark ? ImVec4(1.0f, 1.0f, 1.0f, 0.12f) : ImVec4(0.0f, 0.0f, 0.0f, 0.08f);
        const ImU32 icon_color = ImGui::GetColorU32(icon_tint);
        const ImU32 hover_color = ImGui::GetColorU32(ImGui::IsItemHovered() ? hover_tint : ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

        draw_list->AddRectFilled(min_pos, max_pos, hover_color, rounding);

        const float back_left = min_pos.x + button_size * 0.22f;
        const float back_top = min_pos.y + button_size * 0.26f;
        const float back_right = min_pos.x + button_size * 0.62f;
        const float back_bottom = min_pos.y + button_size * 0.72f;
        draw_list->AddRect(ImVec2(back_left, back_top), ImVec2(back_right, back_bottom), icon_color, button_size * 0.10f, 0, 1.6f);

        const float front_left = min_pos.x + button_size * 0.34f;
        const float front_top = min_pos.y + button_size * 0.16f;
        const float front_right = min_pos.x + button_size * 0.74f;
        const float front_bottom = min_pos.y + button_size * 0.62f;
        draw_list->AddRect(ImVec2(front_left, front_top), ImVec2(front_right, front_bottom), icon_color, button_size * 0.10f, 0, 1.8f);
        draw_list->AddLine(ImVec2(front_left + button_size * 0.08f, front_top + button_size * 0.14f),
                           ImVec2(front_right - button_size * 0.08f, front_top + button_size * 0.14f),
                           icon_color,
                           1.5f);

        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Copy launch command");

        return clicked;
    }

    bool launch_color_button(const char *label, const ImVec2 &size, float ui_scale)
    {
        const ImVec4 launch_green(0.15f, 0.62f, 0.28f, 1.0f);
        const bool clicked = ImGui::ColorButton(label,
                                                launch_green,
                                                ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoDragDrop | ImGuiColorEditFlags_NoBorder,
                                                size);

        ImDrawList *draw_list = ImGui::GetWindowDrawList();
        const ImVec2 min_pos = ImGui::GetItemRectMin();
        const ImVec2 max_pos = ImGui::GetItemRectMax();
        const bool is_active = ImGui::IsItemActive();
        const bool is_hovered = ImGui::IsItemHovered();
        const ImU32 overlay_color = ImGui::GetColorU32(is_active    ? ImVec4(0.0f, 0.0f, 0.0f, 0.16f)
                                                       : is_hovered ? ImVec4(1.0f, 1.0f, 1.0f, 0.10f)
                                                                    : ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
        const float rounding = ImGui::GetStyle().FrameRounding;
        if (overlay_color != 0)
            draw_list->AddRectFilled(min_pos, max_pos, overlay_color, rounding);

        const ImVec2 text_size = ImGui::CalcTextSize(label);
        const ImVec2 text_pos(min_pos.x + (size.x - text_size.x) * 0.5f,
                              min_pos.y + (size.y - text_size.y) * 0.5f - 1.0f * ui_scale);
        draw_list->AddText(text_pos, ImGui::GetColorU32(ImVec4(0.98f, 0.99f, 0.98f, 1.0f)), label);

        return clicked;
    }

    void render_target_binary_picker(AppState &state, float ui_scale)
    {
        if (state.open_target_binary_picker)
        {
            ImGui::OpenPopup("Select Target Binary");
            state.open_target_binary_picker = false;
        }

        if (!ImGui::BeginPopupModal("Select Target Binary", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
            return;

        std::error_code error;
        fs::path current_directory = get_picker_directory(state);
        write_buffer(state.target_binary_picker_directory, IM_ARRAYSIZE(state.target_binary_picker_directory), current_directory.string());

        ImGui::TextWrapped("Choose an executable or any target file to profile.");
        ImGui::Dummy(ImVec2(0.0f, 8.0f * ui_scale));
        ImGui::TextDisabled("Current Directory");
        ImGui::TextWrapped("%s", current_directory.c_str());

        const fs::path parent_directory = current_directory.has_parent_path() ? current_directory.parent_path() : current_directory.root_path();
        if (!parent_directory.empty() && parent_directory != current_directory)
        {
            if (ImGui::Button("Up", ImVec2(80.0f * ui_scale, 0.0f)))
                write_buffer(state.target_binary_picker_directory, IM_ARRAYSIZE(state.target_binary_picker_directory), parent_directory.string());
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(100.0f * ui_scale, 0.0f)))
            ImGui::CloseCurrentPopup();

        std::vector<fs::directory_entry> entries;
        for (const fs::directory_entry &entry : fs::directory_iterator(current_directory, error))
            entries.push_back(entry);

        std::sort(entries.begin(), entries.end(), [](const fs::directory_entry &left, const fs::directory_entry &right)
                  {
        std::error_code left_error;
        std::error_code right_error;
        const bool left_is_directory = left.is_directory(left_error);
        const bool right_is_directory = right.is_directory(right_error);
        if (left_is_directory != right_is_directory)
            return left_is_directory > right_is_directory;
        return left.path().filename().string() < right.path().filename().string(); });

        ImGui::Dummy(ImVec2(0.0f, 8.0f * ui_scale));
        if (ImGui::BeginChild("TargetBinaryPickerEntries", ImVec2(620.0f * ui_scale, 320.0f * ui_scale), true))
        {
            if (error)
            {
                ImGui::TextWrapped("Unable to read this directory.");
            }
            else
            {
                for (const fs::directory_entry &entry : entries)
                {
                    std::error_code entry_error;
                    const bool is_directory = entry.is_directory(entry_error);
                    const bool is_executable = is_executable_file(entry);
                    std::string label = is_directory ? "[DIR] " : (is_executable ? "[EXE] " : "[FILE] ");
                    label += entry.path().filename().string();

                    if (ImGui::Selectable(label.c_str(), false))
                    {
                        if (is_directory)
                        {
                            write_buffer(state.target_binary_picker_directory, IM_ARRAYSIZE(state.target_binary_picker_directory), entry.path().string());
                        }
                        else
                        {
                            state.session_draft.target_binary = entry.path().string();
                            write_buffer(state.target_binary_picker_directory, IM_ARRAYSIZE(state.target_binary_picker_directory), entry.path().parent_path().string());
                            ImGui::CloseCurrentPopup();
                        }
                    }
                }
            }
        }
        ImGui::EndChild();

        ImGui::EndPopup();
    }
}

void render_create_session_page(AppState &state, float ui_scale)
{
    static double last_launch_command_copy_at = -10.0;

    ImGui::Text("Create Session");
    ImGui::Spacing();
    ImGui::InputText("Session Name", &state.session_draft.session_name);
    ImGui::TextDisabled("Target Binary");
    ImGui::InputText("##TargetBinary", &state.session_draft.target_binary);
    ImGui::SameLine();
    if (ImGui::Button("Browse...", ImVec2(110.0f * ui_scale, 0.0f)))
        state.open_target_binary_picker = true;
    if (!state.session_draft.target_binary.empty())
    {
        ImGui::TextDisabled("Selected Path");
        ImGui::TextWrapped("%s", state.session_draft.target_binary.c_str());
    }

    ImGui::InputText("Parameters", &state.session_draft.target_arguments);
    const std::string launch_command = make_command_preview(state.session_draft);
    ImGui::TextDisabled("Launch Command");
    ImGui::SameLine();
    if (clipboard_icon_button("CopyLaunchCommand", 24.0f * ui_scale, state.current_theme))
    {
        ImGui::SetClipboardText(launch_command.c_str());
        last_launch_command_copy_at = ImGui::GetTime();
    }
    ImGui::Dummy(ImVec2(0.0f, 4.0f * ui_scale));
    ImGui::TextWrapped("%s", launch_command.c_str());
    if (ImGui::GetTime() - last_launch_command_copy_at < 1)
    {
        ImGui::TextDisabled("Copied to clipboard");
    }

    const char *templates[] = {"CARM", "TopdownL1", "TopdownL2-BackendBound", "TopdownL2-Retiring", "TopdownL2-FrontendBound", "TopdownL2-BadSpeculation", "Custom"};
    ImGui::Combo("Template", &state.session_draft.selected_template, templates, IM_ARRAYSIZE(templates));
    if (state.session_draft.selected_template == IM_ARRAYSIZE(templates) - 1) // if Custom selected
    {
        ImGui::Dummy(ImVec2(0.0f, 8.0f * ui_scale));
        ImGui::TextDisabled("Collectors");
        ImGui::Checkbox("CPU Counters", &state.session_draft.collect_cpu);
        ImGui::Checkbox("Memory Metrics", &state.session_draft.collect_memory);
        ImGui::Checkbox("GPU Metrics", &state.session_draft.collect_gpu);

        ImGui::Dummy(ImVec2(0.0f, 18.0f * ui_scale));
    }
    else
    {
        
    }

    ImGui::Dummy(ImVec2(0.0f, 4.0f * ui_scale));
    ImGui::Checkbox("Post Mortem", &state.session_draft.post_mortem);
    ImGui::Dummy(ImVec2(0.0f, 8.0f * ui_scale));
    if (launch_color_button("Launch Session", ImVec2(190.0f * ui_scale, 40.0f * ui_scale), ui_scale))
    {
        if (state.optkit.initialize(state.session_draft.session_name))
            state.current_page = AppPage_RecentSessions;
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(120.0f * ui_scale, 40.0f * ui_scale)))
    {
        state.current_page = AppPage_Welcome;
        state.session_draft = SessionDraft{};
    }
    if (!state.optkit.last_error().empty())
    {
        ImGui::Dummy(ImVec2(0.0f, 8.0f * ui_scale));
        ImGui::TextColored(ImVec4(0.92f, 0.30f, 0.30f, 1.0f), "%s", state.optkit.last_error().c_str());
    }
    else if (state.optkit.is_initialized())
    {
        ImGui::Dummy(ImVec2(0.0f, 8.0f * ui_scale));
        ImGui::TextColored(ImVec4(0.20f, 0.70f, 0.32f, 1.0f), "Connected to OPTKIT session: %s", state.optkit.active_session_name().c_str());
    }
    render_target_binary_picker(state, ui_scale);
}