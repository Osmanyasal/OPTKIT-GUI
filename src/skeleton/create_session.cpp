#include "skeleton/create_session.h"

#include "imgui.h"

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

    if (draft.target_arguments[0] != '\0')
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

    std::sort(entries.begin(), entries.end(), [](const fs::directory_entry &left, const fs::directory_entry &right) {
        std::error_code left_error;
        std::error_code right_error;
        const bool left_is_directory = left.is_directory(left_error);
        const bool right_is_directory = right.is_directory(right_error);
        if (left_is_directory != right_is_directory)
            return left_is_directory > right_is_directory;
        return left.path().filename().string() < right.path().filename().string();
    });

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
                        write_buffer(state.session_draft.target_binary, IM_ARRAYSIZE(state.session_draft.target_binary), entry.path().string());
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
    ImGui::Text("Create Session");
    ImGui::Spacing();
    ImGui::InputText("Session Name", state.session_draft.session_name, IM_ARRAYSIZE(state.session_draft.session_name));
    ImGui::TextDisabled("Target Binary");
    ImGui::InputText("##TargetBinary", state.session_draft.target_binary, IM_ARRAYSIZE(state.session_draft.target_binary));
    ImGui::SameLine();
    if (ImGui::Button("Browse...", ImVec2(110.0f * ui_scale, 0.0f)))
        state.open_target_binary_picker = true;
    if (state.session_draft.target_binary[0] != '\0')
    {
        ImGui::TextDisabled("Selected Path");
        ImGui::TextWrapped("%s", state.session_draft.target_binary);
    }

    ImGui::InputText("Parameters", state.session_draft.target_arguments, IM_ARRAYSIZE(state.session_draft.target_arguments));
    ImGui::TextDisabled("Launch Command");
    ImGui::TextWrapped("%s", make_command_preview(state.session_draft).c_str());

    const char *templates[] = {"Quick CPU Sample", "Roofline Capture", "Custom Advanced"};
    ImGui::Combo("Template", &state.session_draft.selected_template, templates, IM_ARRAYSIZE(templates));

    ImGui::Dummy(ImVec2(0.0f, 8.0f * ui_scale));
    ImGui::TextDisabled("Collectors");
    ImGui::Checkbox("CPU Counters", &state.session_draft.collect_cpu);
    ImGui::Checkbox("Memory Metrics", &state.session_draft.collect_memory);
    ImGui::Checkbox("GPU Metrics", &state.session_draft.collect_gpu);

    ImGui::Dummy(ImVec2(0.0f, 18.0f * ui_scale));
    if (ImGui::Button("Launch Session", ImVec2(190.0f * ui_scale, 40.0f * ui_scale)))
        state.current_page = AppPage_RecentSessions;
    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(120.0f * ui_scale, 40.0f * ui_scale)))
        state.current_page = AppPage_Welcome;

    render_target_binary_picker(state, ui_scale);
}