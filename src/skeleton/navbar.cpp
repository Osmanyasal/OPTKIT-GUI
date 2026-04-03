#include "skeleton/navbar.h"

#include "imgui.h"

static bool nav_button(const char *label, bool selected, const ImVec2 &size, AppTheme theme)
{
    ImVec4 selected_button_color = theme == AppTheme_White ? ImVec4(0.12f, 0.12f, 0.12f, 1.0f) : ImVec4(0.92f, 0.92f, 0.92f, 1.0f);
    ImVec4 selected_text_color = theme == AppTheme_White ? ImVec4(0.96f, 0.96f, 0.96f, 1.0f) : ImVec4(0.05f, 0.05f, 0.05f, 1.0f);

    if (selected)
        ImGui::PushStyleColor(ImGuiCol_Button, selected_button_color);
    if (selected)
        ImGui::PushStyleColor(ImGuiCol_Text, selected_text_color);

    bool clicked = ImGui::Button(label, size);

    if (selected)
        ImGui::PopStyleColor(2);

    return clicked;
}

void render_navbar(AppState &state, float ui_scale, float panel_height, float panel_width)
{
    const ImVec4 nav_panel_bg = state.current_theme == AppTheme_White ? ImVec4(0.95f, 0.95f, 0.95f, 1.0f) : ImVec4(0.07f, 0.07f, 0.07f, 1.0f);
    const ImVec4 nav_text_color = state.current_theme == AppTheme_White ? ImVec4(0.10f, 0.10f, 0.10f, 1.0f) : ImVec4(0.95f, 0.95f, 0.95f, 1.0f);
    const ImVec4 nav_button_color = state.current_theme == AppTheme_White ? ImVec4(0.88f, 0.88f, 0.88f, 1.0f) : ImVec4(0.12f, 0.12f, 0.12f, 1.0f);
    const ImVec4 nav_button_hovered = state.current_theme == AppTheme_White ? ImVec4(0.80f, 0.80f, 0.80f, 1.0f) : ImVec4(0.20f, 0.20f, 0.20f, 1.0f);
    const ImVec4 nav_button_active = state.current_theme == AppTheme_White ? ImVec4(0.70f, 0.70f, 0.70f, 1.0f) : ImVec4(0.28f, 0.28f, 0.28f, 1.0f);
    const ImVec4 nav_title_color = state.current_theme == AppTheme_White ? ImVec4(0.08f, 0.08f, 0.08f, 1.0f) : ImVec4(0.97f, 0.97f, 0.97f, 1.0f);

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(18.0f * ui_scale, 12.0f * ui_scale));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10.0f * ui_scale, 10.0f * ui_scale));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, nav_panel_bg);
    ImGui::PushStyleColor(ImGuiCol_Text, nav_text_color);
    ImGui::SetCursorPos(ImVec2(0.0f, 0.0f));
    ImGui::BeginChild("NavigationPanel", ImVec2(panel_width, panel_height), true, ImGuiWindowFlags_NoScrollbar);

    ImGui::Dummy(ImVec2(0.0f, 6.0f * ui_scale));
    ImGui::TextColored(nav_title_color, "OPTKIT GUI");
    ImGui::TextDisabled("Session Workspace");
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.0f, 8.0f * ui_scale));

    ImGui::PushStyleColor(ImGuiCol_Button, nav_button_color);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, nav_button_hovered);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, nav_button_active);

    const ImVec2 nav_button_size(ImGui::GetContentRegionAvail().x, 46.0f * ui_scale);

    if (nav_button("Welcome", state.current_page == AppPage_Welcome, nav_button_size, state.current_theme))
        state.current_page = AppPage_Welcome;
    if (nav_button("Recent Sessions", state.current_page == AppPage_RecentSessions, nav_button_size, state.current_theme))
        state.current_page = AppPage_RecentSessions;
    if (nav_button("Create Session", state.current_page == AppPage_CreateSession, nav_button_size, state.current_theme))
        state.current_page = AppPage_CreateSession;
    if (nav_button("About", state.current_page == AppPage_About, nav_button_size, state.current_theme))
        state.current_page = AppPage_About;

    ImGui::PopStyleColor(3);

    ImGui::Dummy(ImVec2(0.0f, 16.0f * ui_scale));
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.0f, 10.0f * ui_scale));
    ImGui::TextDisabled("Runtime");
    ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
    ImGui::Text("%.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
    ImGui::EndChild();
    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar(2);
}