#include "skeleton/texture.h"

#include <png.h>
#include <vector>

bool load_png_texture(const char *path, TextureHandle *texture)
{
    FILE *file = fopen(path, "rb");
    if (file == nullptr)
        return false;

    png_byte header[8];
    if (fread(header, 1, sizeof(header), file) != sizeof(header) || png_sig_cmp(header, 0, sizeof(header)) != 0)
    {
        fclose(file);
        return false;
    }

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (png == nullptr)
    {
        fclose(file);
        return false;
    }

    png_infop info = png_create_info_struct(png);
    if (info == nullptr)
    {
        png_destroy_read_struct(&png, nullptr, nullptr);
        fclose(file);
        return false;
    }

    if (setjmp(png_jmpbuf(png)))
    {
        png_destroy_read_struct(&png, &info, nullptr);
        fclose(file);
        return false;
    }

    png_init_io(png, file);
    png_set_sig_bytes(png, sizeof(header));
    png_read_info(png, info);

    png_uint_32 width = png_get_image_width(png, info);
    png_uint_32 height = png_get_image_height(png, info);
    png_byte color_type = png_get_color_type(png, info);
    png_byte bit_depth = png_get_bit_depth(png, info);

    if (bit_depth == 16)
        png_set_strip_16(png);
    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png);
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(png);
    if (png_get_valid(png, info, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png);
    if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
    if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png);

    png_read_update_info(png, info);

    png_size_t row_bytes = png_get_rowbytes(png, info);
    std::vector<unsigned char> pixels(row_bytes * height);
    std::vector<png_bytep> rows(height);
    for (png_uint_32 y = 0; y < height; ++y)
        rows[y] = pixels.data() + y * row_bytes;

    png_read_image(png, rows.data());
    png_read_end(png, nullptr);
    png_destroy_read_struct(&png, &info, nullptr);
    fclose(file);

    GLuint texture_id = 0;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

    texture->id = texture_id;
    texture->width = static_cast<int>(width);
    texture->height = static_cast<int>(height);
    return true;
}

void destroy_texture(TextureHandle *texture)
{
    if (texture->id != 0)
        glDeleteTextures(1, &texture->id);
    texture->id = 0;
    texture->width = 0;
    texture->height = 0;
}