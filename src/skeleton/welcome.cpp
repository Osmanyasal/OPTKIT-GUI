#include "skeleton/welcome.h"

#include "imgui.h"

void render_welcome_page(AppState &state, float ui_scale)
{
    ImGui::Text("Welcome");
    ImGui::Spacing();
    ImGui::TextWrapped("Start a profiling workflow from the left menu. This landing page is designed to feel like a performance workstation rather than a demo window.");
    ImGui::Dummy(ImVec2(0.0f, 18.0f * ui_scale));

    ImGui::BeginChild("WelcomeHero", ImVec2(0.0f, 180.0f * ui_scale), true);
    ImGui::Text("System Ready");
    ImGui::Separator();
    ImGui::TextWrapped("Create a new session to configure counters, capture a workload, and analyze results. Recent sessions are available from the navigation panel.");
    ImGui::Dummy(ImVec2(0.0f, 14.0f * ui_scale));
    if (ImGui::Button("Create New Session", ImVec2(220.0f * ui_scale, 40.0f * ui_scale)))
        state.current_page = AppPage_CreateSession;
    ImGui::SameLine();
    if (ImGui::Button("View Recent Sessions", ImVec2(220.0f * ui_scale, 40.0f * ui_scale)))
        state.current_page = AppPage_RecentSessions;
    ImGui::EndChild();

    ImGui::Dummy(ImVec2(0.0f, 18.0f * ui_scale));
    ImGui::Columns(3, "WelcomeStats", false);
    ImGui::BeginChild("StatCapture", ImVec2(0.0f, 110.0f * ui_scale), true);
    ImGui::TextDisabled("Capture Status");
    ImGui::Text("Idle");
    ImGui::TextWrapped("No active profile capture is running.");
    ImGui::EndChild();
    ImGui::NextColumn();
    ImGui::BeginChild("StatDevice", ImVec2(0.0f, 110.0f * ui_scale), true);
    ImGui::TextDisabled("Target");
    ImGui::Text("Local Host");
    ImGui::TextWrapped("Ready to create a CPU or GPU profiling session.");
    ImGui::EndChild();
    ImGui::NextColumn();
    ImGui::BeginChild("StatBuild", ImVec2(0.0f, 110.0f * ui_scale), true);
    ImGui::TextDisabled("Renderer");
    ImGui::Text("OpenGL + ImGui");
    ImGui::TextWrapped("UI running with monitor-aware sizing and font scaling.");
    ImGui::EndChild();
    ImGui::Columns(1);
}