#ifndef PLAYGROUND_TEXTURE_HPP
#define PLAYGROUND_TEXTURE_HPP

#include <cstddef>

#include <glad/glad.h>

#include "png.hpp"
#include "types.hpp"

namespace playground {

class Texture final {
public:
    Texture() = default;
    Texture(size_t width, size_t height, size_t total_channels);
    ~Texture();

    Texture(Texture const&) = delete;
    Texture(Texture&&) = delete;
    Texture& operator=(Texture const&) = delete;
    Texture& operator=(Texture&&) = delete;

    template <class PixelType>
    void upload(png::Pixels<PixelType> const& data, size_t x_offset, size_t y_offset, size_t width, size_t height);

    void bind();
    void unbind();

private:
    size_t total_channels_{};

    GLuint id_{};
};

} // namespace playground

#endif // PLAYGROUND_TEXTURE_HPP
