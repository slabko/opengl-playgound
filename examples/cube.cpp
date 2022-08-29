#include <cmath>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/io.hpp>

#include "imgui.h"

#include "../playground/png.hpp"

#include "cube.hpp"

static char const* vertex_shader = R"(
#version 460

in vec3 position;
in vec2 texcoord;

uniform mat4   model;
uniform mat4   view;
uniform mat4   proj;

out vec2 v_texcoord;

void main()
{
    v_texcoord = texcoord;
    gl_Position = proj * view * model * vec4(position, 1.0);
}
)";

static char const* fragment_shader = R"(
#version 460

in vec2 v_texcoord;
out vec4 fragColor;

uniform sampler2D texture;

void main()
{
    fragColor = texture2D(texture, v_texcoord);
}
)";

float radians(float a)
{
    return a * M_PIf / 180.0F;
}

float gradians(float a)
{
    return a * 180.0F / M_PIf;
}

Eigen::Matrix4f frustum(float left, float right, float bottom, float top, float znear, float zfar)
{
    Eigen::Matrix4f m = Eigen::Matrix4f::Zero();
    m(0, 0) = 2.0F * znear / (right - left);
    m(1, 1) = 2.0F * znear / (top - bottom);
    m(2, 2) = -(zfar + znear) / (zfar - znear);
    m(0, 2) = (right + left) / (right - left);
    m(2, 1) = (top + bottom) / (top - bottom);
    m(2, 3) = -2.0F * znear * zfar / (zfar - znear);
    m(3, 2) = -1.0F;
    return m;
}

Eigen::Matrix4f perspective(float fovy, float aspect, float znear, float zfar)
{
    float h = std::tan(0.5F * radians(fovy)) * znear;
    float w = h * aspect;
    return frustum(-w, w, -h, h, znear, zfar);
}

Eigen::Matrix4f translate(float x, float y, float z)
{
    Eigen::Matrix4f m = Eigen::Matrix4f::Identity();
    m(0, 3) = x;
    m(1, 3) = y;
    m(2, 3) = z;
    return m;
}

Eigen::Matrix4f x_rotate(float a)
{
    float t = radians(a);
    float c = std::cos(t);
    float s = std::sin(t);

    Eigen::Matrix4f m = Eigen::Matrix4f::Identity();
    m(1, 1) = c;
    m(1, 2) = -s;
    m(2, 1) = s;
    m(2, 2) = c;

    return m;
}

Eigen::Matrix4f y_rotate(float a)
{
    float t = radians(a);
    float c = std::cos(t);
    float s = std::sin(t);

    Eigen::Matrix4f m = Eigen::Matrix4f::Identity();
    m(0, 0) = c;
    m(0, 2) = s;
    m(2, 0) = -s;
    m(2, 2) = c;

    return m;
}

Cube::Cube() :
  playground::Program{vertex_shader, fragment_shader},
  texture_{256, 256, 3}
{
    using Eigen::Dynamic;
    using Eigen::RowMajor;

    // clang-format off
    Eigen::Matrix<float, Dynamic, Dynamic, RowMajor> position {
      {-1.0F,  1.0F,  1.0F,   0.0F, 0.0F},
      { 1.0F,  1.0F,  1.0F,   1.0F, 0.0F},
      {-1.0F, -1.0F,  1.0F,   0.0F, 1.0F},
      { 1.0F, -1.0F,  1.0F,   1.0F, 1.0F},
      { 1.0F,  1.0F,  1.0F,   0.0F, 0.0F},
      { 1.0F,  1.0F, -1.0F,   1.0F, 0.0F},
      { 1.0F, -1.0F,  1.0F,   0.0F, 1.0F},
      { 1.0F, -1.0F, -1.0F,   1.0F, 1.0F},
      {-1.0F,  1.0F, -1.0F,   0.0F, 0.0F},
      {-1.0F,  1.0F,  1.0F,   1.0F, 0.0F},
      {-1.0F, -1.0F, -1.0F,   0.0F, 1.0F},
      {-1.0F, -1.0F,  1.0F,   1.0F, 1.0F},
      {-1.0F,  1.0F, -1.0F,   0.0F, 0.0F},
      { 1.0F,  1.0F, -1.0F,   1.0F, 0.0F},
      {-1.0F,  1.0F,  1.0F,   0.0F, 1.0F},
      { 1.0F,  1.0F,  1.0F,   1.0F, 1.0F},
      {-1.0F, -1.0F,  1.0F,   0.0F, 0.0F},
      { 1.0F, -1.0F,  1.0F,   1.0F, 0.0F},
      {-1.0F, -1.0F, -1.0F,   0.0F, 1.0F},
      { 1.0F, -1.0F, -1.0F,   1.0F, 1.0F},
      {-1.0F, -1.0F, -1.0F,   0.0F, 0.0F},
      { 1.0F, -1.0F, -1.0F,   1.0F, 0.0F},
      {-1.0F,  1.0F, -1.0F,   0.0F, 1.0F},
      { 1.0F,  1.0F, -1.0F,   1.0F, 1.0F},
    };
    // clang-format on

    // clang-format off
    Eigen::Matrix<uint32_t, Dynamic, Dynamic, RowMajor> index{
      { 0,  1,  2},  { 1,  2,  3},
      { 4,  5,  6},  { 5,  6,  7},
      { 8,  9, 10},  { 9, 10, 11},
      {12, 13, 14},  {13, 14, 15},
      {16, 17, 18},  {17, 18, 19},
      {20, 21, 22},  {21, 22, 23},
    };
    // clang-format on

    alloc_vbo(sizeof(float) * position.size());
    upload_vbo(position, 0);
    assign_vbo("position", 3, sizeof(float) * 5, 0);
    assign_vbo("texcoord", 2, sizeof(float) * 5, sizeof(float) * 3);

    alloc_ibo(sizeof(uint32_t) * index.size());
    upload_ibo(index, 0);

    auto model = Eigen::Matrix4f::Identity();
    auto view = translate(0.0F, 0.0F, -5.0F);
    auto proj = perspective(45.0F, 1920.0F / 1080.0F, 2.0F, 100.0F);

    set_uniform_data("model", model);
    set_uniform_data("view",  view);
    set_uniform_data("proj",  proj);

    auto image = png::read_png<png::RgbPixel>("textures/crate.png");
    texture_.upload(image.pixels, 0, 0, image.width, image.height);
}

void Cube::present_imgui()
{
    ImGui::Begin("Configuration");
    ImGui::Text("Show the animated cube");
    ImGui::Checkbox("Show Cube", &show_cube_);
    ImGui::Checkbox("Animate Cube", &animate_);
    ImGui::Text("Application average %.1f FPS", ImGui::GetIO().Framerate);
    ImGui::SliderFloat2("Rotation", rotation_.data(), 0, 360);
    ImGui::End();
}

void Cube::update()
{
    if (animate_) {
        rotation_[0] = std::fmod(rotation_[0] + 1.0F, 360.0F);
        rotation_[1] = std::fmod(rotation_[1] + 1.0F, 360.0F);
    }

    auto view =  translate(0.0F, 0.0F, -5.0F) * y_rotate(rotation_[1])  * x_rotate(rotation_[0]);
    set_uniform_data("view",  view);

}

void Cube::render() 
{
    if (show_cube_) {
        texture_.bind();
        draw_indices(36);
    }
}
