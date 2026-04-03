#include "skeleton/settings.h"

#include "skeleton/theme.h"

#include <cmath>
#include <cstdint>

static bool gear_button(const char *id, float button_size, AppTheme theme)
{
    const float two_pi = 6.28318530718f;

    bool clicked = ImGui::InvisibleButton(id, ImVec2(button_size, button_size));

    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    ImVec2 min_pos = ImGui::GetItemRectMin();
    ImVec2 max_pos = ImGui::GetItemRectMax();
    ImVec2 center((min_pos.x + max_pos.x) * 0.5f, (min_pos.y + max_pos.y) * 0.5f);
    float body_radius = button_size * 0.19f;
    float tooth_radius = button_size * 0.052f;
    float tooth_distance = button_size * 0.27f;
    float inner_hole_radius = button_size * 0.085f;
    ImVec4 icon_tint = theme == AppTheme_Dark ? ImVec4(0.08f, 0.08f, 0.08f, 1.0f) : ImVec4(0.96f, 0.96f, 0.96f, 1.0f);

    ImU32 icon_color = ImGui::GetColorU32(icon_tint);
    ImU32 accent_color = ImGui::GetColorU32(ImGui::IsItemHovered() ? ImVec4(icon_tint.x, icon_tint.y, icon_tint.z, 0.18f)
                                                                  : ImVec4(icon_tint.x, icon_tint.y, icon_tint.z, 0.0f));

    draw_list->AddCircleFilled(center, button_size * 0.42f, accent_color, 24);
    draw_list->AddCircleFilled(center, body_radius, icon_color, 28);

    for (int tooth = 0; tooth < 8; ++tooth)
    {
        float angle = (two_pi * tooth) / 8.0f;
        ImVec2 tooth_center(center.x + cosf(angle) * tooth_distance, center.y + sinf(angle) * tooth_distance);
        draw_list->AddCircleFilled(tooth_center, tooth_radius, icon_color, 12);
    }

    draw_list->AddCircleFilled(center, inner_hole_radius, ImGui::GetColorU32(ImGuiCol_WindowBg), 18);
    draw_list->AddCircle(center, inner_hole_radius, icon_color, 18, 1.5f);

    return clicked;
}

static bool gear_texture_button(const char *id, const TextureHandle &texture, float button_size, AppTheme theme)
{
    bool clicked = ImGui::InvisibleButton(id, ImVec2(button_size, button_size));
    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    ImVec2 min_pos = ImGui::GetItemRectMin();
    ImVec2 max_pos = ImGui::GetItemRectMax();
    float padding = button_size * 0.08f;
    ImVec4 hover_tint = theme == AppTheme_Dark ? ImVec4(1.0f, 1.0f, 1.0f, 0.10f) : ImVec4(0.0f, 0.0f, 0.0f, 0.08f);
    if (ImGui::IsItemHovered())
    {
        ImVec2 center((min_pos.x + max_pos.x) * 0.5f, (min_pos.y + max_pos.y) * 0.5f);
        draw_list->AddCircleFilled(center, button_size * 0.42f, ImGui::GetColorU32(hover_tint), 24);
    }

    ImTextureID texture_id = static_cast<ImTextureID>(static_cast<uintptr_t>(texture.id));
    draw_list->AddImage(texture_id,
                        ImVec2(min_pos.x + padding, min_pos.y + padding),
                        ImVec2(max_pos.x - padding, max_pos.y - padding));
    return clicked;
}

void render_settings_button_and_popup(AppState &state,
                                      const TextureHandle &dark_theme_gear_icon,
                                      const TextureHandle &white_theme_gear_icon,
                                      float ui_scale,
                                      ImVec4 *clear_color)
{
    const float gear_size = 36.0f * ui_scale;
    ImGui::SetCursorPos(ImVec2(ImGui::GetWindowContentRegionMax().x - gear_size - 24.0f * ui_scale, 10.0f * ui_scale));

    const TextureHandle &active_gear_texture = state.current_theme == AppTheme_White ? white_theme_gear_icon : dark_theme_gear_icon;
    bool pressed_settings = active_gear_texture.id != 0
                                ? gear_texture_button("SettingsGear", active_gear_texture, gear_size, state.current_theme)
                                : gear_button("SettingsGear", gear_size, state.current_theme);

    if (pressed_settings)
        state.show_settings_popup = true;
    if (state.show_settings_popup)
        ImGui::OpenPopup("Settings");

    ImGui::SetNextWindowPos(ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowWidth() - 260.0f * ui_scale,
                                   ImGui::GetWindowPos().y + 56.0f * ui_scale));
    if (ImGui::BeginPopup("Settings"))
    {
        state.show_settings_popup = false;
        ImGui::Text("Settings");
        ImGui::Separator();
        ImGui::TextDisabled("Theme");

        int theme_index = static_cast<int>(state.current_theme);
        const char *theme_names[] = {"Dark", "White"};
        if (ImGui::Combo("##ThemeSelector", &theme_index, theme_names, IM_ARRAYSIZE(theme_names)))
        {
            state.current_theme = static_cast<AppTheme>(theme_index);
            apply_theme(state.current_theme, clear_color);
        }

        ImGui::Dummy(ImVec2(0.0f, 8.0f * ui_scale));
        ImGui::TextDisabled("Window");
        ImGui::Text("Monitor-aware 16:9 startup");
        ImGui::Dummy(ImVec2(0.0f, 8.0f * ui_scale));
        ImGui::TextDisabled("UI Scale");
        ImGui::Text("%.2fx", ui_scale);
        ImGui::EndPopup();
    }
}