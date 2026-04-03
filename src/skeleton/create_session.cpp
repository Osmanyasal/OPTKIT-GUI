#include "skeleton/create_session.h"

#include "imgui.h"

void render_create_session_page(AppState &state, float ui_scale)
{
    ImGui::Text("Create Session");
    ImGui::Spacing();
    ImGui::InputText("Session Name", state.session_draft.session_name, IM_ARRAYSIZE(state.session_draft.session_name));
    ImGui::InputText("Target Binary", state.session_draft.target_binary, IM_ARRAYSIZE(state.session_draft.target_binary));

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
}