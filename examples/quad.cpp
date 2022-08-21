#include "quad.hpp"

#include "../playground/png.hpp"

char const* vertex_shader = R"(
#version 460
in   vec2  vertexIn;
in   vec3  colorIn;
in   vec2  textCoordIn;

out  vec3  colorOut;
out  vec2  texCoordOut;

uniform mat4 view;

void main() {
    colorOut = colorIn;
    texCoordOut = textCoordIn;
    gl_Position = view * vec4(vertexIn, 0.0, 1.0);
}
)";

char const* fragment_shader = R"(
#version 460
in   vec3  colorOut;
in   vec2  texCoordOut;

out  vec4  fragColor;

uniform sampler2D textureImage;

void main() {
    float g = texture(textureImage, texCoordOut).r;
    fragColor = vec4(colorOut, 1.0) * vec4(g, g, g, 1.0);
}
)";

Quad::Quad() :
  playground::Program{vertex_shader, fragment_shader}
{
    // clang-format off
        Eigen::MatrixXf xs{
          {-1.0F,  1.0F,  1.0F,  0.0F,  0.0F,  0.0F,  1.0F,},
          { 1.0F,  1.0F,  1.0F,  0.0F,  0.0F,  1.0F,  1.0F,},
          {-1.0F, -1.0F,  1.0F,  0.0F,  0.0F,  0.0F,  0.0F,},
          {-1.0F, -1.0F,  0.0F,  1.0F,  0.0F,  0.0F,  0.0F,},
          { 1.0F, -1.0F,  0.0F,  1.0F,  0.0F,  1.0F,  0.0F,},
          { 1.0F,  1.0F,  0.0F,  1.0F,  0.0F,  1.0F,  1.0F,}
        };
    // clang-format on

    alloc_vbo(sizeof(float) * xs.size());
    upload_vbo(xs, 0);
    assign_vbo("vertexIn", 2, sizeof(float) * 7, 0);
    assign_vbo("colorIn", 3, sizeof(float) * 7, sizeof(float) * 2);
    assign_vbo("textCoordIn", 2, sizeof(float) * 7, sizeof(float) * 5);

    auto image = png::read_png("textures/crate.png");
    std::vector<uint8_t> red(image.width * image.height);
    for (size_t i = 0; i < image.pixels.size(); ++i) {
        if (i % 3 == 0) {
            red[i / 3] = image.pixels[i];
        }
    }

    texture_ = std::make_unique<playground::Texture>(image.width, image.height, 1);
    texture_->upload(red.data(), 0, 0, image.width, image.height);

    Eigen::Matrix4f view = Eigen::Matrix4f::Identity() * 0.75;
    view(3, 3) = 1;
    set_uniform_data("view", view);

    set_vertex_count(6);
}

void Quad::render()
{
    texture_->bind();
}
