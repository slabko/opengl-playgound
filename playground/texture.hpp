#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <cstddef>

#include <glad/glad.h>

#include "types.hpp"

namespace playground {

class Texture final {
public:
    Texture() = default;
    Texture(size_t width, size_t height, size_t channels);
    ~Texture();

    Texture(Texture const&) = delete;
    Texture(Texture&&) = delete;
    Texture& operator=(Texture const&) = delete;
    Texture& operator=(Texture&&) = delete;

    void upload(uint8_t* data, size_t x_offset, size_t y_offset, size_t width, size_t height);

    void bind();
    void unbind();

private:
    GLenum format_{};

    GLuint id_{};
};

} // namespace playground

#endif // TEXTURE_HPP
