#include "skeleton/about.h"
#include "imgui.h"
#include "utils/environment_config.hh"
#include <string>
void render_about_page(float ui_scale)
{
    ImGui::Text("About");
    ImGui::Spacing();
    ImGui::TextWrapped("OPTKIT-GUI is OPTKIT's front-end for performance analysis workflows.");
    ImGui::Dummy(ImVec2(0.0f, 16.0f * ui_scale));
    ImGui::BeginChild("AboutInfo", ImVec2(0.0f, 140.0f * ui_scale), true);
    ImGui::Text("Version");
    ImGui::SameLine(160.0f * ui_scale);
    ImGui::TextDisabled("Prototype UI");
    ImGui::Text("OPTKIT");
    ImGui::SameLine(160.0f * ui_scale);
    std::string version = "v" + std::string(OPTKIT_VERSION);
    ImGui::TextDisabled(version.c_str());
    ImGui::Text("Backend");
    ImGui::SameLine(160.0f * ui_scale);
    ImGui::TextDisabled("GLFW / OpenGL3 / Dear ImGui / ImPlot");
    ImGui::Text("Focus");
    ImGui::SameLine(160.0f * ui_scale);
    ImGui::TextDisabled("Session-driven performance analysis");
    ImGui::EndChild();
}