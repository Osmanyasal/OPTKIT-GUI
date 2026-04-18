#pragma once

#include <GLFW/glfw3.h>

void glfw_error_callback(int error, const char *description);
void get_initial_window_size(GLFWmonitor *monitor, int *window_width, int *window_height);
float get_monitor_scale(GLFWwindow *window, GLFWmonitor *monitor);