#include "skeleton/about.h"

#include "imgui.h"

void render_about_page(float ui_scale)
{
    ImGui::Text("About");
    ImGui::Spacing();
    ImGui::TextWrapped("OPTKIT-GUI is a Dear ImGui based front-end for performance analysis workflows. This shell layout uses a left navigation menu and page-oriented content similar to workstation tools like AMD uProf or other profiling dashboards.");
    ImGui::Dummy(ImVec2(0.0f, 16.0f * ui_scale));
    ImGui::BeginChild("AboutInfo", ImVec2(0.0f, 140.0f * ui_scale), true);
    ImGui::Text("Version");
    ImGui::SameLine(160.0f * ui_scale);
    ImGui::TextDisabled("Prototype UI");
    ImGui::Text("Backend");
    ImGui::SameLine(160.0f * ui_scale);
    ImGui::TextDisabled("GLFW / OpenGL3 / Dear ImGui / ImPlot");
    ImGui::Text("Focus");
    ImGui::SameLine(160.0f * ui_scale);
    ImGui::TextDisabled("Session-driven performance analysis");
    ImGui::EndChild();
}