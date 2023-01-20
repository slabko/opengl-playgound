#include "texture.hpp"

#include <fmt/core.h>

namespace playground {

GLint get_channels_format(size_t channels)
{
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

Texture::Texture(size_t width, size_t height, size_t total_channels, GLenum unit) :
  total_channels_{total_channels}, unit_{unit}
{
    glGenTextures(1, &id_);

    bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(
      GL_TEXTURE_2D,
      0,
      get_channels_format(total_channels),
      static_cast<GLint>(width),
      static_cast<GLint>(height),
      0,
      get_channels_format(total_channels_),
      GL_UNSIGNED_BYTE,
      nullptr);
}

template <class PixelType>
void Texture::upload(PixelType const* data, size_t x_offset, size_t y_offset, size_t width, size_t height)
{
    size_t const received_channels = png::total_channels<PixelType>::value;
    if (total_channels_ != received_channels) {
        throw std::runtime_error(fmt::format("Expected {} channels, got {}", total_channels_, received_channels));
    }

    bind();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexSubImage2D(
      GL_TEXTURE_2D,
      0,
      static_cast<GLint>(x_offset),
      static_cast<GLint>(y_offset),
      static_cast<GLsizei>(width),
      static_cast<GLsizei>(height),
      get_channels_format(total_channels_),
      GL_UNSIGNED_BYTE,
      (uint8_t*)data); // NOLINT(*-readability-casting)
}

template <class PixelType>
void Texture::upload(png::Pixels<PixelType> const& data, size_t x_offset, size_t y_offset, size_t width, size_t height)
{
    upload(data.data(), x_offset, y_offset, width, height);
}

template void Texture::upload<uint8_t>(png::Pixels<uint8_t> const& data, size_t x_offset, size_t y_offset, size_t width, size_t height);
template void Texture::upload<png::RedPixel>(png::Pixels<png::RedPixel> const& data, size_t x_offset, size_t y_offset, size_t width, size_t height);
template void Texture::upload<png::RgbPixel>(png::Pixels<png::RgbPixel> const& data, size_t x_offset, size_t y_offset, size_t width, size_t height);
template void Texture::upload<png::RgbaPixel>(png::Pixels<png::RgbaPixel> const& data, size_t x_offset, size_t y_offset, size_t width, size_t height);

void Texture::bind()
{
    glActiveTexture(unit_);
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
