#include "imgui.h"

#include "../playground/png.hpp"

#include "quad.hpp"

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
    Eigen::MatrixXf xs {
    //  X       Y      R      G      B      U      V    //
      {-1.0F,  1.0F,  1.0F,  0.0F,  0.0F,  0.0F,  1.0F,},
      { 1.0F,  1.0F,  1.0F,  0.0F,  0.0F,  1.0F,  1.0F,},
      {-1.0F, -1.0F,  1.0F,  0.0F,  0.0F,  0.0F,  0.0F,},
      {-1.0F, -1.0F,  0.0F,  1.0F,  0.0F,  0.0F,  0.0F,},
      { 1.0F, -1.0F,  0.0F,  1.0F,  0.0F,  1.0F,  0.0F,},
      { 1.0F,  1.0F,  0.0F,  1.0F,  0.0F,  1.0F,  1.0F,}
    };
    // clang-format on

    alloc_vbo(static_cast<int>(sizeof(float)) * xs.size());
    upload_vbo(xs, 0);
    size_t stride_size = sizeof(float) * xs.cols();
    assign_vbo("vertexIn", 2, stride_size, 0);
    assign_vbo("colorIn", 3, stride_size, sizeof(float) * 2);
    assign_vbo("textCoordIn", 2, stride_size, sizeof(float) * 5);

    auto image = png::read_png("textures/crate.png");
    texture_ = std::make_unique<playground::Texture>(image.width, image.height, 1);

    // Upload only first channel
    std::vector<uint8_t> red(image.width * image.height);
    for (size_t i = 0; i < image.width * image.height * 3; ++i) {
        if (i % 3 == 0) {
            red[i / 3] = image.pixels[i];
        }
    }

    texture_->upload(red.data(), 0, 0, image.width, image.height);

    // Put a light gray box in the middle of the texture
    size_t block_size = 30;
    uint8_t color = 200;
    std::vector<uint8_t> block(block_size * block_size);
    std::fill(block.begin(), block.end(), color);
    size_t offset = (image.width - block_size) / 2;
    texture_->upload(block.data(), offset, offset, block_size, block_size);
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
    Eigen::Matrix4f view = Eigen::Matrix4f::Identity() * scale_;
    view(3, 3) = 1;
    set_uniform_data("view", view);
}

void Quad::render()
{
    texture_->bind();
    draw_simple_triangles(6);
}
