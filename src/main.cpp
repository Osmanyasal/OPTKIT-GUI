// OPTKIT-GUI: Main application entry point
// Uses Dear ImGui + ImPlot with GLFW and OpenGL3 backend

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"

#include "skeleton/about.h"
#include "skeleton/app_state.h"
#include "skeleton/create_session.h"
#include "skeleton/navbar.h"
#include "skeleton/recent_sessions.h"
#include "skeleton/settings.h"
#include "skeleton/texture.h"
#include "skeleton/theme.h"
#include "skeleton/welcome.h"
#include "skeleton/window.h"

#include <GLFW/glfw3.h>
#include <cstdio>

static ImFont *load_ui_font(ImGuiIO &io, float ui_scale)
{
    const float font_size = 16.0f * ui_scale;
    const char *font_candidates[] = {
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/TTF/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/liberation2/LiberationSans-Regular.ttf",
        "/usr/share/fonts/liberation/LiberationSans-Regular.ttf"};

    ImFontConfig font_config;
    font_config.OversampleH = 3;
    font_config.OversampleV = 2;
    font_config.PixelSnapH = false;

    for (const char *font_path : font_candidates)
    {
        FILE *font_file = fopen(font_path, "rb");
        if (font_file == nullptr)
            continue;

        fclose(font_file);
        return io.Fonts->AddFontFromFileTTF(font_path, font_size, &font_config);
    }

    font_config.SizePixels = font_size;
    return io.Fonts->AddFontDefault(&font_config);
}

int main(int, char **)
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // GL 3.3 + GLSL 130
    const char *glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    GLFWmonitor *primary_monitor = glfwGetPrimaryMonitor();
    int window_width = 1280;
    int window_height = 720;
    get_initial_window_size(primary_monitor, &window_width, &window_height);

    GLFWwindow *window = glfwCreateWindow(window_width, window_height, "OPTKIT-GUI", nullptr, nullptr);
    if (window == nullptr)
        return 1;

    if (primary_monitor != nullptr)
    {
        const GLFWvidmode *video_mode = glfwGetVideoMode(primary_monitor);
        if (video_mode != nullptr)
        {
            int work_x = 0;
            int work_y = 0;
            int work_width = video_mode->width;
            int work_height = video_mode->height;
            glfwGetMonitorWorkarea(primary_monitor, &work_x, &work_y, &work_width, &work_height);

            int window_x = work_x + (work_width - window_width) / 2;
            int window_y = work_y + (work_height - window_height) / 2;
            if (window_x < 0)
                window_x = 0;
            if (window_y < 0)
                window_y = 0;
            glfwSetWindowPos(window, window_x, window_y);
        }
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();

    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    float ui_scale = get_monitor_scale(window, primary_monitor);
    if (ui_scale < 1.0f)
        ui_scale = 1.0f;

    ImGuiStyle &style = ImGui::GetStyle();
    style.ScaleAllSizes(ui_scale);

    io.Fonts->Clear();
    load_ui_font(io, ui_scale);

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    TextureHandle dark_theme_gear_icon;
    TextureHandle white_theme_gear_icon;
    load_png_texture("/home/rt7/Desktop/OPTKIT-GUI/src/figures/gearbox_white.png", &dark_theme_gear_icon);
    load_png_texture("/home/rt7/Desktop/OPTKIT-GUI/src/figures/gearbox_black.png", &white_theme_gear_icon);

    AppState state;
    ImVec4 clear_color;
    apply_theme(state.current_theme, &clear_color);

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Main OPTKIT-GUI window
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);

        ImGuiWindowFlags main_window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar |
                             ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_NoResize;

        ImGui::Begin("OPTKIT-GUI", nullptr, main_window_flags);

        const float sidebar_width = 200.0f * ui_scale;
        const float inner_spacing = 18.0f * ui_scale;
        const float full_panel_height = ImGui::GetWindowSize().y;
        render_navbar(state, ui_scale, full_panel_height, sidebar_width);

        ImGui::SetCursorPos(ImVec2(sidebar_width + inner_spacing, 0.0f));
        ImGui::BeginChild("ContentPanel", ImVec2(ImGui::GetWindowSize().x - sidebar_width - inner_spacing, full_panel_height), false, ImGuiWindowFlags_NoScrollbar);
        render_settings_button_and_popup(state, dark_theme_gear_icon, white_theme_gear_icon, ui_scale, &clear_color);

        switch (state.current_page)
        {
        case AppPage_Welcome:
            render_welcome_page(state, ui_scale);
            break;
        case AppPage_RecentSessions:
            render_recent_sessions_page(ui_scale);
            break;
        case AppPage_CreateSession:
            render_create_session_page(state, ui_scale);
            break;
        case AppPage_About:
            render_about_page(ui_scale);
            break;
        }

        ImGui::EndChild();
        ImGui::End();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    destroy_texture(&dark_theme_gear_icon);
    destroy_texture(&white_theme_gear_icon);
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
