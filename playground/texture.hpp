#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <cstddef>

#include <glad/glad.h>

#include "playgound.hpp"

namespace playground {

class Texture final {
public:
    Texture() = default;
    Texture(int width, int height, int channels);
    ~Texture();

    Texture(Texture const&) = delete;
    Texture(Texture&&) = delete;
    Texture& operator=(Texture const&) = delete;
    Texture& operator=(Texture&&) = delete;

    void upload(unsigned char* data, int x_offset, int y_offset, int width, int height);

    void bind();
    void unbind();

private:
    GLenum format_{};

    GLuint id_{};
};

} // namespace playground

#endif // TEXTURE_HPP
