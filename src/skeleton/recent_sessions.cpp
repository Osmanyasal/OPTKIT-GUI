#include "skeleton/recent_sessions.h"

#include "imgui.h"

void render_recent_sessions_page(float ui_scale)
{
    ImGui::Text("Recent Sessions");
    ImGui::Spacing();

    ImGui::BeginChild("RecentSessionA", ImVec2(0.0f, 96.0f * ui_scale), true);
    ImGui::Text("Roofline Analysis - matmul_bench");
    ImGui::TextDisabled("Captured today, 14:32");
    ImGui::TextWrapped("CPU counters, memory bandwidth, and hotspot summary were saved in the latest workspace.");
    ImGui::EndChild();

    ImGui::Dummy(ImVec2(0.0f, 12.0f * ui_scale));
    ImGui::BeginChild("RecentSessionB", ImVec2(0.0f, 96.0f * ui_scale), true);
    ImGui::Text("Cache Study - stencil_solver");
    ImGui::TextDisabled("Yesterday, 18:07");
    ImGui::TextWrapped("Captured LLC misses, branch behavior, and thread timing across 16 worker threads.");
    ImGui::EndChild();

    ImGui::Dummy(ImVec2(0.0f, 12.0f * ui_scale));
    ImGui::BeginChild("RecentSessionC", ImVec2(0.0f, 96.0f * ui_scale), true);
    ImGui::Text("GPU Timeline - vector_ops");
    ImGui::TextDisabled("Two days ago, 09:15");
    ImGui::TextWrapped("Session contains kernel duration traces and launch configuration snapshots.");
    ImGui::EndChild();
}