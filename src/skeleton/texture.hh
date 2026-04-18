#pragma once

#include <GLFW/glfw3.h>

struct TextureHandle
{
    GLuint id = 0;
    int width = 0;
    int height = 0;
};

bool load_png_texture(const char *path, TextureHandle *texture);
void destroy_texture(TextureHandle *texture);