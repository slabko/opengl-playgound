#include "texture.hpp"

#include <fmt/core.h>

namespace playground {

GLenum get_channels_format(int channels) {
    switch (channels) {
        case 1:
            return GL_RED;
        case 2:
            return GL_RG;
        case 3:
            return GL_RGB;
        case 4:
            return GL_RGBA;
        default:
            throw std::runtime_error(fmt::format("Unexpected number of channels: {}", channels));
    }
}


Texture::Texture(int width, int height, int channels) : format_{get_channels_format(channels)}
{
    glGenTextures(1, &id_);

    bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, format_, width, height, 0, format_, GL_UNSIGNED_BYTE, nullptr);
}

void Texture::upload(unsigned char* data, int x_offset, int y_offset, int width, int height)
{

    bind();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexSubImage2D(GL_TEXTURE_2D, 0, x_offset, y_offset, width, height, format_, GL_UNSIGNED_BYTE, data);
}

void Texture::bind()
{
    glBindTexture(GL_TEXTURE_2D, id_);
}

void Texture::unbind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture()
{
    glDeleteTextures(1, &id_);
}

} // namespace playground
