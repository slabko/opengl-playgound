#include <execution>
#include <iostream>

#include <Eigen/Eigen>

#include <imgui.h>

#include "game_of_life.hpp"

using RowMajorMatrixXf = Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
using RowMajorMatrixXui = Eigen::Matrix<GLuint, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

static char const* vertex_shader = R"(
#version 460
in   vec2  position;
in   vec2  uv;
out  vec2  v_uv;

uniform mat4 view;

void main() {
    v_uv = uv;
    gl_Position = vec4(position, 0.0, 1.0);
}
)";

static char const* fragment_shader = R"(
#version 460
in   vec2  v_uv;

out  vec4  fragColor;

uniform sampler2D textureImage;

void main() {
    float g = 1 - texture(textureImage, v_uv).r * 255;
    fragColor = vec4(g, g, g, 1.0);
}
)";

GameOfLife::GameOfLife() :
  playground::Program{vertex_shader, fragment_shader, BOARD_SIZE * BOARD_SCALE, BOARD_SIZE * BOARD_SCALE},
  texture_{BOARD_SIZE, BOARD_SIZE, 1},
  generation_(BOARD_SIZE, BOARD_SIZE),
  neighbors_(BOARD_SIZE, BOARD_SIZE)
{
    reset();

    // clang-format off
    RowMajorMatrixXf position {
      //  X       Y      U      V    //
      {-1.0F,  1.0F,  0.0F,  1.0F,},
      { 1.0F,  1.0F,  1.0F,  1.0F,},
      { 1.0F, -1.0F,  1.0F,  0.0F,},
      {-1.0F, -1.0F,  0.0F,  0.0F,},
    };
    // clang-format on

    RowMajorMatrixXui index{{0, 1, 2}, {2, 3, 0}};

    auto vbo_size = sizeof(float) * static_cast<size_t>(position.size());
    alloc_vbo(vbo_size);
    upload_vbo(position.data(), 0, vbo_size);
    auto stride_size = sizeof(float) * static_cast<size_t>(position.cols());
    assign_vbo("position", 2, stride_size, 0);
    assign_vbo("uv", 2, stride_size, sizeof(float) * 2);

    auto ibo_size = sizeof(uint32_t) * static_cast<size_t>(index.size());
    alloc_ibo(ibo_size);
    upload_ibo(index.data(), 0, ibo_size);
}

void GameOfLife::reset()
{
    Eigen::Matrix<float, -1, -1, Eigen::RowMajor> random(BOARD_SIZE, BOARD_SIZE);
    random.setRandom();

    // Add an empty block in the middle for fun
    Eigen::Index empty_block_size = BOARD_SIZE / 2;
    Eigen::Index offset = (BOARD_SIZE - empty_block_size) / 2;
    random.block(offset, offset, empty_block_size, empty_block_size).setConstant(0);

    auto random_reshaped = random.reshaped();
    std::transform(
      std::execution::unseq,
      random_reshaped.cbegin(),
      random_reshaped.cend(),
      generation_.reshaped().begin(),
      [](auto x) {
          return static_cast<uint8_t>(x > 0.5);
      });

    generation_.row(0).setConstant(0);
    generation_.row(generation_.rows() - 1).setConstant(0);
    generation_.col(0).setConstant(0);
    generation_.col(generation_.cols() - 1).setConstant(0);
}

void GameOfLife::render()
{
    texture_.bind();
    draw_indices(6);
}

void GameOfLife::update()
{
    if (continue_) {
        update_generation();
    }

    texture_.upload(generation_.data(), 0, 0, BOARD_SIZE, BOARD_SIZE);
}

void GameOfLife::present_imgui()
{
    ImGui::Begin("Configuration");
    ImGui::Checkbox("Continue", &continue_);
    if (ImGui::Button("One Step")) {
        update_generation();
    }
    if (ImGui::Button("Reset")) {
        reset();
    }
    ImGui::Text("Application average %.1f FPS", ImGui::GetIO().Framerate);
    ImGui::End();
}

void GameOfLife::update_generation()
{
    neighbors_.setConstant(0);
    for (Eigen::Index i = 1; i < generation_.rows() - 1; ++i) {
        for (Eigen::Index j = 1; j < generation_.cols() - 1; ++j) {
            neighbors_(i, j) = generation_.block(i - 1, j - 1, 3, 3).sum();
        }
    }
    neighbors_ -= generation_;

    auto generation_reshaped = generation_.reshaped();
    auto neighbors_reshaped = neighbors_.reshaped();
    std::transform(
      std::execution::unseq,
      generation_reshaped.cbegin() + 1,
      generation_reshaped.cend() - 1,
      neighbors_reshaped.cbegin() + 1,
      generation_reshaped.begin() + 1,
      [](auto y, auto x) {
          // clang-format off
          if (x < 2) { return 0; }
          if (x > 3) { return 0; }
          if (y == 1) { return 1; }
          if (x == 3) { return 1; }
          return 0;
          // clang-format on
      });
}
