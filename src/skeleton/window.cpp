#include "skeleton/window.h"

#include <cstdio>

void glfw_error_callback(int error, const char *description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void get_initial_window_size(GLFWmonitor *monitor, int *window_width, int *window_height)
{
    const float target_aspect_ratio = 16.0f / 9.0f;
    *window_width = 1280;
    *window_height = 720;

    if (monitor == nullptr)
        return;

    const GLFWvidmode *video_mode = glfwGetVideoMode(monitor);
    if (video_mode == nullptr)
        return;

    int work_x = 0;
    int work_y = 0;
    int work_width = video_mode->width;
    int work_height = video_mode->height;
    glfwGetMonitorWorkarea(monitor, &work_x, &work_y, &work_width, &work_height);

    int max_width = static_cast<int>(work_width * 0.70f);
    int max_height = static_cast<int>(work_height * 0.70f);

    int candidate_width = max_width;
    int candidate_height = static_cast<int>(candidate_width / target_aspect_ratio);

    if (candidate_height > max_height)
    {
        candidate_height = max_height;
        candidate_width = static_cast<int>(candidate_height * target_aspect_ratio);
    }

    if (candidate_width < 960)
    {
        candidate_width = work_width < 960 ? work_width : 960;
        candidate_height = static_cast<int>(candidate_width / target_aspect_ratio);
    }

    if (candidate_height < 540)
    {
        candidate_height = work_height < 540 ? work_height : 540;
        candidate_width = static_cast<int>(candidate_height * target_aspect_ratio);
    }

    *window_width = candidate_width;
    *window_height = candidate_height;
}

float get_monitor_scale(GLFWwindow *window, GLFWmonitor *monitor)
{
    float xscale = 1.0f;
    float yscale = 1.0f;

    if (window != nullptr)
        glfwGetWindowContentScale(window, &xscale, &yscale);
    else if (monitor != nullptr)
        glfwGetMonitorContentScale(monitor, &xscale, &yscale);

    if (yscale > 0.0f)
        return yscale;
    if (xscale > 0.0f)
        return xscale;
    return 1.0f;
}