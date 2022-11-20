#include <Eigen/Eigen>
#include <glm/ext/matrix_transform.hpp>
#include <glm/mat4x4.hpp>

#include <imgui.h>

#include "../../playground/png.hpp"

#include "quad.hpp"

using RowMajorMatrixXf = Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
using RowMajorMatrixXui = Eigen::Matrix<GLuint, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

static char const* vertex_shader = R"(
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

static char const* fragment_shader = R"(
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
    RowMajorMatrixXf xs {
    //  X       Y      R      G      B      U      V    //
      {-1.0F,  1.0F,  1.0F,  0.0F,  0.0F,  0.0F,  1.0F,},
      { 1.0F,  1.0F,  1.0F,  0.0F,  0.0F,  1.0F,  1.0F,},
      { 1.0F, -1.0F,  0.0F,  1.0F,  0.0F,  1.0F,  0.0F,},
      {-1.0F, -1.0F,  1.0F,  0.0F,  0.0F,  0.0F,  0.0F,},
    };
    // clang-format on

    auto vbo_size = sizeof(float) * static_cast<size_t>(xs.size());
    alloc_vbo(vbo_size);
    upload_vbo(xs.data(), 0, vbo_size);
    size_t stride_size = sizeof(float) * static_cast<size_t>(xs.cols());
    assign_vbo("vertexIn", 2, stride_size, 0);
    assign_vbo("colorIn", 3, stride_size, sizeof(float) * 2);
    assign_vbo("textCoordIn", 2, stride_size, sizeof(float) * 5);

    RowMajorMatrixXui indices{{0, 1, 2}, {2, 3, 0}};
    auto ibo_size = sizeof(uint32_t) * static_cast<size_t>(indices.size());
    alloc_ibo(ibo_size);
    upload_ibo(indices.data(), 0, ibo_size);

    auto image = png::read_png<png::RgbPixel>("textures/chest.png");
    texture_ = std::make_unique<playground::Texture>(image.width, image.height, 1);

    // Upload only first channel
    png::Pixels<png::RedPixel> red{image.width * image.height};
    std::transform(image.pixels.cbegin(), image.pixels.cend(), red.begin(), [](auto x) {
        return png::RedPixel{x.r};
    });
    texture_->upload(red, 0, 0, image.width, image.height);

    // Put a glow gray box in the middle of the texture
    size_t block_size = 30;
    uint8_t color = 30;
    size_t x_offset = (image.width - block_size) / 2;
    size_t y_offset = (image.height - block_size) / 2;

    auto block = png::Pixels<png::RedPixel>(block_size * block_size);
    std::fill(block.begin(), block.end(), png::RedPixel{color});
    texture_->upload(block, x_offset, y_offset, block_size, block_size);
}

void Quad::present_imgui()
{
    ImGui::Begin("Configuration");
    ImGui::Text("Show the animated cube");
    ImGui::SliderFloat("Scale", &scale_, 0, 1);
    ImGui::Text("Application average %.1f FPS", ImGui::GetIO().Framerate);
    ImGui::End();
}

void Quad::update()
{
    auto view = glm::mat4(1.0);
    view = glm::scale(view, glm::vec3(scale_));
    set_uniform_data("view", view);
}

void Quad::render()
{
    texture_->bind();
    draw_indices(6);
}
