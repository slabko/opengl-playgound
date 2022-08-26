#include <execution>
#include <iostream>

#include "imgui.h"

#include "game_of_life.hpp"

static char const* vertex_shader = R"(
#version 460
in   vec2  position;
in   vec2  texcoord;
out  vec2  v_texcoord;

uniform mat4 view;

void main() {
    v_texcoord = texcoord;
    gl_Position = vec4(position, 0.0, 1.0);
}
)";

static char const* fragment_shader = R"(
#version 460
in   vec2  v_texcoord;

out  vec4  fragColor;

uniform sampler2D textureImage;

void main() {
    float g = texture(textureImage, v_texcoord).r;
    fragColor = vec4(g, g, g, 1.0);
    // fragColor = vec4(1,0,0,0);
}
)";

GameOfLife::GameOfLife() :
  playground::Program(vertex_shader, fragment_shader, 1024, 1024)
{

    Eigen::Matrix<float, -1, -1, Eigen::RowMajor> random(size_, size_);
    random.setRandom();

    neighbors_ = Eigen::Matrix<float, -1, -1, Eigen::RowMajor>(size_, size_);
    generation_ = Eigen::Matrix<float, -1, -1, Eigen::RowMajor>(size_, size_);
    auto random_reshaped = random.reshaped();
    std::transform(random_reshaped.cbegin(), random_reshaped.cend(), generation_.reshaped().begin(), [](auto x) {
        return static_cast<float>(x > 0.5);
    });

    // clang-format off
    Eigen::MatrixXf position {
      //  X       Y      U      V    //
      {-1.0F,  1.0F,  0.0F,  1.0F,},
      { 1.0F,  1.0F,  1.0F,  1.0F,},
      { 1.0F, -1.0F,  1.0F,  0.0F,},
      {-1.0F, -1.0F,  0.0F,  0.0F,},
    };
    // clang-format on

    Eigen::MatrixX<uint32_t> index{{0, 1, 2}, {2, 3, 0}};

    alloc_vbo(static_cast<int>(sizeof(float)) * position.size());
    upload_vbo(position, 0);
    size_t stride_size = sizeof(float) * position.cols();
    assign_vbo("position", 2, stride_size, 0);
    assign_vbo("texcoord", 2, stride_size, sizeof(float) * 2);

    alloc_ibo(sizeof(uint32_t) * index.size());
    upload_ibo(index, 0);

    texture_ = std::make_unique<playground::Texture>(size_, size_, 1);
};

void GameOfLife::render()
{
    texture_->bind();
    draw_indices(6);
};

void GameOfLife::update()
{
    if (continue_) {
        update_generation();
    }
    Eigen::Matrix<png::RedPixel, -1, -1, Eigen::RowMajor> image(size_, size_);
    auto generation_reshaped = generation_.reshaped();
    std::transform(std::execution::unseq, generation_reshaped.cbegin(), generation_reshaped.cend(), image.reshaped().begin(), [](float x) {
        return png::RedPixel{static_cast<uint8_t>((1 - x) * 255)};
    });
    texture_->upload(image, 0, 0, size_, size_);
};

void GameOfLife::present_imgui()
{
    ImGui::Begin("Configuration");
    ImGui::Checkbox("Continue", &continue_);
    if (ImGui::Button("One Step")) {
        update_generation();
    }
    ImGui::Text("Application average %.1f FPS", ImGui::GetIO().Framerate);
    ImGui::End();
};

void GameOfLife::update_generation()
{
    generation_.row(0).setConstant(0);
    generation_.row(generation_.rows() - 1).setConstant(0);
    generation_.col(0).setConstant(0);
    generation_.col(generation_.cols() - 1).setConstant(0);

    neighbors_.setConstant(0);
    for (Eigen::Index i = 1; i < generation_.cols() - 1; ++i) {
        for (Eigen::Index j = 1; j < generation_.rows() - 1; ++j) {
            neighbors_(i, j) = generation_(i - 1, j - 1) + generation_(i, j - 1) + generation_(i + 1, j - 1)
              + generation_(i - 1, j) + generation_(i + 1, j)
              + generation_(i - 1, j + 1) + generation_(i, j + 1) + generation_(i + 1, j + 1);
        }
    }

    auto generation_reshaped = generation_.reshaped();
    auto neighbors_reshaped = neighbors_.reshaped();
    std::transform(std::execution::unseq, generation_reshaped.cbegin(), generation_reshaped.cend(), neighbors_reshaped.cbegin(), generation_reshaped.begin(), [](auto y, auto x) {
        // clang-format off
      if (x < 2) { return 0.0F; }
      if (x > 3) { return 0.0F; }
      if (y == 1) { return 1.0F; }
      if (x == 3) { return 1.0F; }
      return 0.0F;
        // clang-format on
    });
}
