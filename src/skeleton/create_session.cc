#include "skeleton/create_session.hh"

#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"
#include "misc/cpp/imgui_stdlib.cpp"

#include <algorithm>
#include <cerrno>
#include <csignal>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <sys/wait.h>
#include <system_error>
#include <unistd.h>
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
            return "<select a target binary>";

        std::string path = OPTKIT_CLI_PATH;
        std::string params = " stat -o ";
        std::string program = " -- " + draft.target_binary;
        if (!draft.target_arguments.empty())
            program += " " + draft.target_arguments;
        std::string command = path + params + program;

        return command;
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

    std::string format_elapsed_time(double elapsed_seconds)
    {
        const int total_seconds = std::max(0, static_cast<int>(std::floor(elapsed_seconds)));
        const int hours = total_seconds / 3600;
        const int minutes = (total_seconds % 3600) / 60;
        const int seconds = total_seconds % 60;

        char buffer[32];
        if (hours > 0)
            std::snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d", hours, minutes, seconds);
        else
            std::snprintf(buffer, sizeof(buffer), "%02d:%02d", minutes, seconds);

        return buffer;
    }

    const std::vector<std::string> &get_category_metrics(const std::unordered_map<std::string, std::vector<std::string>> &metrics_by_category,
                                                         const std::string &category)
    {
        static const std::vector<std::string> empty_metrics;

        const auto it = metrics_by_category.find(category);
        if (it == metrics_by_category.end())
            return empty_metrics;

        return it->second;
    }

    bool contains_metric(const std::vector<std::string> &metrics, const std::string &metric)
    {
        return std::find(metrics.begin(), metrics.end(), metric) != metrics.end();
    }

    void render_metric_selector(const char *id,
                                const char *title,
                                const std::vector<std::string> &available_metrics,
                                std::vector<std::string> &selected_metrics,
                                float ui_scale)
    {
        ImGui::PushID(id);
        ImGui::TextDisabled("%s", title);

        ImGui::BeginGroup();
        ImGui::TextUnformatted("Available");
        if (ImGui::BeginChild("AvailableMetrics", ImVec2(250.0f * ui_scale, 130.0f * ui_scale), true))
        {
            bool has_available_metrics = false;
            for (const std::string &metric : available_metrics)
            {
                if (contains_metric(selected_metrics, metric))
                    continue;

                has_available_metrics = true;
                if (ImGui::Selectable(metric.c_str(), false))
                    selected_metrics.push_back(metric);
            }

            if (!has_available_metrics)
                ImGui::TextDisabled("No available metrics");
        }
        ImGui::EndChild();
        ImGui::EndGroup();

        ImGui::SameLine();

        ImGui::BeginGroup();
        ImGui::TextUnformatted("Selected");
        if (ImGui::BeginChild("SelectedMetrics", ImVec2(250.0f * ui_scale, 130.0f * ui_scale), true))
        {
            if (selected_metrics.empty())
            {
                ImGui::TextDisabled("No selected metrics");
            }
            else
            {
                for (size_t index = 0; index < selected_metrics.size();)
                {
                    if (ImGui::Selectable(selected_metrics[index].c_str(), false))
                    {
                        selected_metrics.erase(selected_metrics.begin() + static_cast<std::ptrdiff_t>(index));
                    }
                    else
                    {
                        ++index;
                    }
                }
            }
        }
        ImGui::EndChild();
        ImGui::EndGroup();

        ImGui::PopID();
    }

    bool start_launch_process(const std::string &command, SessionDraft &draft)
    {
        errno = 0;
        const pid_t child_pid = fork();
        if (child_pid < 0)
        {
            draft.process_status = std::string("Failed to start launch command: ") + std::strerror(errno);
            return false;
        }

        if (child_pid == 0)
        {
            if (setsid() < 0)
                _exit(127);

            execl("/bin/sh", "sh", "-c", command.c_str(), static_cast<char *>(nullptr));
            _exit(127);
        }

        draft.launched_process_id = static_cast<int>(child_pid);
        draft.launched_process_cancel_requested = false;
        draft.process_status.clear();
        return true;
    }

    void request_process_cancel(SessionDraft &draft)
    {
        if (draft.launched_process_id <= 0 || draft.launched_process_cancel_requested)
            return;

        if (kill(-draft.launched_process_id, SIGTERM) == 0)
        {
            draft.launched_process_cancel_requested = true;
            draft.process_status = "Cancelling process...";
            return;
        }

        draft.process_status = std::string("Failed to cancel process: ") + std::strerror(errno);
    }

    void poll_launch_process(SessionDraft &draft)
    {
        if (draft.launched_process_id <= 0)
            return;

        int status = 0;
        const pid_t wait_result = waitpid(static_cast<pid_t>(draft.launched_process_id), &status, WNOHANG);
        if (wait_result == 0)
            return;

        if (wait_result < 0)
        {
            draft.process_status = std::string("Failed to poll process: ") + std::strerror(errno);
            draft.launched_process_id = -1;
            draft.launched_process_cancel_requested = false;
            draft.connected_at_seconds = -1.0;
            OptkitAdapter::finalize();
            return;
        }

        if (WIFEXITED(status))
        {
            const int exit_code = WEXITSTATUS(status);
            if (exit_code == 0)
                draft.process_status = draft.launched_process_cancel_requested ? "Process cancelled." : "Process completed.";
            else
                draft.process_status = "Process exited with code " + std::to_string(exit_code) + ".";
        }
        else if (WIFSIGNALED(status))
        {
            draft.process_status = draft.launched_process_cancel_requested ? "Process cancelled." : "Process terminated by signal " + std::to_string(WTERMSIG(status)) + ".";
        }

        draft.launched_process_id = -1;
        draft.launched_process_cancel_requested = false;
        draft.connected_at_seconds = -1.0;
        OptkitAdapter::finalize();
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
    static const std::unordered_map<std::string, std::vector<std::string>> metrics_by_category = OptkitAdapter::list_available_metrics();

    poll_launch_process(state.session_draft);

    if (OptkitAdapter::is_initialized() && state.session_draft.connected_at_seconds >= 0.0)
        state.session_draft.elapsed_seconds = ImGui::GetTime() - state.session_draft.connected_at_seconds;
    else if (!OptkitAdapter::is_initialized())
        state.session_draft.elapsed_seconds = 0.0;

    ImGui::SetWindowFontScale(1.4f);
    ImGui::Text("Create Session");
    ImGui::SetWindowFontScale(1.0f);

    ImGui::Spacing();
    ImGui::Text("Session Name");
    ImGui::InputText("##session_name", &state.session_draft.session_name);
    ImGui::TextDisabled("Target Binary");
    ImGui::InputText("##TargetBinary", &state.session_draft.target_binary);
    ImGui::SameLine();
    if (ImGui::Button("Browse...", ImVec2(110.0f * ui_scale, 0.0f)))
        state.open_target_binary_picker = true;

    ImGui::Text("Parameters");
    ImGui::InputText("##parameters", &state.session_draft.target_arguments);
    const std::string launch_command = make_command_preview(state.session_draft);
    ImGui::TextDisabled("Launch Command");
    ImGui::SameLine();
    if (clipboard_icon_button("CopyLaunchCommand", 24.0f * ui_scale, state.current_theme))
    {
        ImGui::SetClipboardText(launch_command.c_str());
        last_launch_command_copy_at = ImGui::GetTime();
    }
    ImGui::TextWrapped("%s", launch_command.c_str());
    if (ImGui::GetTime() - last_launch_command_copy_at < 1)
    {
        ImGui::TextDisabled("Copied to clipboard");
    }
    ImGui::Dummy(ImVec2(0.0f, 4.0f * ui_scale));

    const char *templates[] = {"CARM", "TopdownL1", "TopdownL2-BackendBound", "TopdownL2-Retiring", "TopdownL2-FrontendBound", "TopdownL2-BadSpeculation", "Custom"};
    ImGui::Combo("Template", &state.session_draft.selected_template, templates, IM_ARRAYSIZE(templates));
    if (state.session_draft.selected_template == IM_ARRAYSIZE(templates) - 1) // if Custom selected
    {
        ImGui::Dummy(ImVec2(0.0f, 8.0f * ui_scale));
        ImGui::TextDisabled("Collectors");
        ImGui::Checkbox("CPU Counters", &state.session_draft.collect_cpu);
        if (state.session_draft.collect_cpu)
        {
            render_metric_selector("CpuPerformanceMetrics",
                                   "CPU Performance Metrics",
                                   get_category_metrics(metrics_by_category, "cpu_performance"),
                                   state.session_draft.selected_metrics_by_category["cpu_performance"],
                                   ui_scale);
        }
        ImGui::Checkbox("Energy", &state.session_draft.collect_energy);
        if (state.session_draft.collect_energy)
        {
            render_metric_selector("CpuEnergyMetrics",
                                   "CPU Energy Metrics",
                                   get_category_metrics(metrics_by_category, "cpu_energy"),
                                   state.session_draft.selected_metrics_by_category["cpu_energy"],
                                   ui_scale);
            ImGui::Dummy(ImVec2(0.0f, 6.0f * ui_scale));
            render_metric_selector("GpuEnergyMetrics",
                                   "GPU Energy Metrics",
                                   get_category_metrics(metrics_by_category, "gpu_energy"),
                                   state.session_draft.selected_metrics_by_category["gpu_energy"],
                                   ui_scale);
        }
        ImGui::Checkbox("GPU Metrics", &state.session_draft.collect_gpu);
        if (state.session_draft.collect_gpu)
        {
            render_metric_selector("GpuPerformanceMetrics",
                                   "GPU Performance Metrics",
                                   get_category_metrics(metrics_by_category, "gpu_performance"),
                                   state.session_draft.selected_metrics_by_category["gpu_performance"],
                                   ui_scale);
        }

        ImGui::Dummy(ImVec2(0.0f, 18.0f * ui_scale));
    }
    else
    {
    }

    ImGui::Dummy(ImVec2(0.0f, 4.0f * ui_scale));
    ImGui::Checkbox("Post Mortem", &state.session_draft.post_mortem);
    ImGui::Dummy(ImVec2(0.0f, 8.0f * ui_scale));
    ImGui::BeginDisabled(state.session_draft.launched_process_id > 0);
    if (launch_color_button("Launch Session", ImVec2(190.0f * ui_scale, 40.0f * ui_scale), ui_scale))
    {
        if (OptkitAdapter::initialize(state.session_draft.session_name))
        {
            if (start_launch_process(launch_command, state.session_draft))
            {
                state.session_draft.connected_at_seconds = ImGui::GetTime();
                state.session_draft.elapsed_seconds = 0.0;
            }
            else
            {
                state.session_draft.connected_at_seconds = -1.0;
                state.session_draft.elapsed_seconds = 0.0;
                OptkitAdapter::finalize();
            }
        }
    }
    ImGui::EndDisabled();
    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(120.0f * ui_scale, 40.0f * ui_scale)))
    {
        if (state.session_draft.launched_process_id > 0)
        {
            request_process_cancel(state.session_draft);
        }
        else
        {
            OptkitAdapter::finalize();
            state.current_page = AppPage_Welcome;
            state.session_draft = SessionDraft{};
        }
    }
    if (!OptkitAdapter::last_error().empty())
    {
        ImGui::Dummy(ImVec2(0.0f, 8.0f * ui_scale));
        ImGui::TextColored(ImVec4(0.92f, 0.30f, 0.30f, 1.0f), "%s", OptkitAdapter::last_error().c_str());
    }
    else if (OptkitAdapter::is_initialized())
    {
        ImGui::Dummy(ImVec2(0.0f, 8.0f * ui_scale));
        ImGui::TextColored(ImVec4(0.20f, 0.70f, 0.32f, 1.0f), "Connected to OPTKIT session: %s", OptkitAdapter::active_session_name().c_str());
        ImGui::TextDisabled("Time passed: %s", format_elapsed_time(state.session_draft.elapsed_seconds).c_str());
        if (state.session_draft.launched_process_id > 0)
        {
            ImGui::SameLine();
            if (!state.session_draft.launched_process_cancel_requested)
            {
                if (ImGui::Button("Cancel Process"))
                    request_process_cancel(state.session_draft);
            }
            else
            {
                ImGui::TextDisabled("Cancelling...");
            }
        }
    }

    if (!state.session_draft.process_status.empty())
    {
        ImGui::Dummy(ImVec2(0.0f, 8.0f * ui_scale));
        ImGui::TextDisabled("%s", state.session_draft.process_status.c_str());
    }

    render_target_binary_picker(state, ui_scale);
}