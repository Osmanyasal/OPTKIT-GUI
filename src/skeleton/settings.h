#pragma once

#include "imgui.h"

#include "skeleton/app_state.h"
#include "skeleton/texture.h"

void render_settings_button_and_popup(AppState &state,
                                      const TextureHandle &dark_theme_gear_icon,
                                      const TextureHandle &white_theme_gear_icon,
                                      float ui_scale,
                                      ImVec4 *clear_color);