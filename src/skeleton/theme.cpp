#include "skeleton/theme.h"

static void configure_base_theme()
{
    ImGuiStyle &style = ImGui::GetStyle();
    style.WindowRounding = 10.0f;
    style.ChildRounding = 8.0f;
    style.FrameRounding = 6.0f;
    style.PopupRounding = 8.0f;
    style.ScrollbarRounding = 10.0f;
    style.GrabRounding = 6.0f;
    style.WindowBorderSize = 1.0f;
    style.ChildBorderSize = 1.0f;
    style.FrameBorderSize = 1.0f;
}

static void apply_dark_theme()
{
    configure_base_theme();

    ImVec4 *colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text] = ImVec4(0.94f, 0.94f, 0.94f, 1.0f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.56f, 0.56f, 0.56f, 1.0f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.11f, 0.12f, 1.0f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.0f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.14f, 0.15f, 0.16f, 0.98f);
    colors[ImGuiCol_Border] = ImVec4(0.28f, 0.29f, 0.31f, 1.0f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.18f, 0.19f, 0.20f, 1.0f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.23f, 0.24f, 0.26f, 1.0f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.30f, 0.31f, 0.34f, 1.0f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.11f, 0.12f, 1.0f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.12f, 0.13f, 0.14f, 1.0f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.12f, 0.13f, 0.14f, 1.0f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.11f, 0.12f, 0.13f, 1.0f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.33f, 0.34f, 0.36f, 1.0f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.43f, 0.44f, 0.47f, 1.0f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.58f, 0.59f, 0.62f, 1.0f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.95f, 0.95f, 0.95f, 1.0f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.78f, 0.78f, 0.78f, 1.0f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.96f, 0.96f, 0.96f, 1.0f);
    colors[ImGuiCol_Button] = ImVec4(0.22f, 0.23f, 0.24f, 1.0f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.30f, 0.31f, 0.33f, 1.0f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.39f, 0.40f, 0.43f, 1.0f);
    colors[ImGuiCol_Header] = ImVec4(0.21f, 0.22f, 0.24f, 1.0f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.29f, 0.30f, 0.32f, 1.0f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.38f, 0.39f, 0.41f, 1.0f);
    colors[ImGuiCol_Separator] = ImVec4(0.30f, 0.31f, 0.33f, 1.0f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.31f, 0.32f, 0.34f, 0.8f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.46f, 0.47f, 0.49f, 0.8f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.60f, 0.61f, 0.64f, 0.8f);
}

static void apply_white_theme()
{
    configure_base_theme();

    ImVec4 *colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text] = ImVec4(0.08f, 0.08f, 0.08f, 1.0f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.45f, 0.45f, 0.45f, 1.0f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.96f, 0.96f, 0.96f, 1.0f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.985f, 0.985f, 0.985f, 1.0f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.98f, 0.98f, 0.98f, 0.98f);
    colors[ImGuiCol_Border] = ImVec4(0.78f, 0.78f, 0.78f, 1.0f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.92f, 0.92f, 0.92f, 1.0f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.88f, 0.88f, 0.88f, 1.0f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.82f, 0.82f, 0.82f, 1.0f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.93f, 0.93f, 0.93f, 1.0f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.91f, 0.91f, 0.91f, 1.0f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.94f, 0.94f, 0.94f, 1.0f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.93f, 0.93f, 0.93f, 1.0f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.72f, 0.72f, 0.72f, 1.0f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.60f, 0.60f, 0.60f, 1.0f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.48f, 0.48f, 0.48f, 1.0f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.12f, 0.12f, 0.12f, 1.0f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.30f, 0.30f, 0.30f, 1.0f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.12f, 0.12f, 0.12f, 1.0f);
    colors[ImGuiCol_Button] = ImVec4(0.88f, 0.88f, 0.88f, 1.0f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.82f, 0.82f, 0.82f, 1.0f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.74f, 0.74f, 0.74f, 1.0f);
    colors[ImGuiCol_Header] = ImVec4(0.88f, 0.88f, 0.88f, 1.0f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.82f, 0.82f, 0.82f, 1.0f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.74f, 0.74f, 0.74f, 1.0f);
    colors[ImGuiCol_Separator] = ImVec4(0.76f, 0.76f, 0.76f, 1.0f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.70f, 0.70f, 0.70f, 0.8f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.56f, 0.8f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.40f, 0.40f, 0.8f);
}

void apply_theme(AppTheme theme, ImVec4 *clear_color)
{
    if (theme == AppTheme_White)
    {
        apply_white_theme();
        *clear_color = ImVec4(0.94f, 0.94f, 0.94f, 1.0f);
        return;
    }

    apply_dark_theme();
    *clear_color = ImVec4(0.08f, 0.09f, 0.10f, 1.0f);
}