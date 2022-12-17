#include <algorithm>
#include <vector>

#include <glm/ext/matrix_transform.hpp>

#include "cuboid.hpp"

static std::vector<Vertex> create_unit_cube()
{
    // clang-format off
    return {
      {{-0.5F, -0.5F, -0.5F}, { 0.0F,  0.0F, -1.0F}},
      {{ 0.5F,  0.5F, -0.5F}, { 0.0F,  0.0F, -1.0F}},
      {{ 0.5F, -0.5F, -0.5F}, { 0.0F,  0.0F, -1.0F}},
      {{ 0.5F,  0.5F, -0.5F}, { 0.0F,  0.0F, -1.0F}},
      {{-0.5F, -0.5F, -0.5F}, { 0.0F,  0.0F, -1.0F}},
      {{-0.5F,  0.5F, -0.5F}, { 0.0F,  0.0F, -1.0F}},
      {{-0.5F, -0.5F,  0.5F}, { 0.0F,  0.0F,  1.0F}},
      {{ 0.5F, -0.5F,  0.5F}, { 0.0F,  0.0F,  1.0F}},
      {{ 0.5F,  0.5F,  0.5F}, { 0.0F,  0.0F,  1.0F}},
      {{ 0.5F,  0.5F,  0.5F}, { 0.0F,  0.0F,  1.0F}},
      {{-0.5F,  0.5F,  0.5F}, { 0.0F,  0.0F,  1.0F}},
      {{-0.5F, -0.5F,  0.5F}, { 0.0F,  0.0F,  1.0F}},
      {{-0.5F,  0.5F,  0.5F}, {-1.0F,  0.0F,  0.0F}},
      {{-0.5F,  0.5F, -0.5F}, {-1.0F,  0.0F,  0.0F}},
      {{-0.5F, -0.5F, -0.5F}, {-1.0F,  0.0F,  0.0F}},
      {{-0.5F, -0.5F, -0.5F}, {-1.0F,  0.0F,  0.0F}},
      {{-0.5F, -0.5F,  0.5F}, {-1.0F,  0.0F,  0.0F}},
      {{-0.5F,  0.5F,  0.5F}, {-1.0F,  0.0F,  0.0F}},
      {{ 0.5F,  0.5F,  0.5F}, { 1.0F,  0.0F,  0.0F}},
      {{ 0.5F, -0.5F, -0.5F}, { 1.0F,  0.0F,  0.0F}},
      {{ 0.5F,  0.5F, -0.5F}, { 1.0F,  0.0F,  0.0F}},
      {{ 0.5F, -0.5F, -0.5F}, { 1.0F,  0.0F,  0.0F}},
      {{ 0.5F,  0.5F,  0.5F}, { 1.0F,  0.0F,  0.0F}},
      {{ 0.5F, -0.5F,  0.5F}, { 1.0F,  0.0F,  0.0F}},
      {{-0.5F, -0.5F, -0.5F}, { 0.0F, -1.0F,  0.0F}},
      {{ 0.5F, -0.5F, -0.5F}, { 0.0F, -1.0F,  0.0F}},
      {{ 0.5F, -0.5F,  0.5F}, { 0.0F, -1.0F,  0.0F}},
      {{ 0.5F, -0.5F,  0.5F}, { 0.0F, -1.0F,  0.0F}},
      {{-0.5F, -0.5F,  0.5F}, { 0.0F, -1.0F,  0.0F}},
      {{-0.5F, -0.5F, -0.5F}, { 0.0F, -1.0F,  0.0F}},
      {{-0.5F,  0.5F, -0.5F}, { 0.0F,  1.0F,  0.0F}},
      {{ 0.5F,  0.5F,  0.5F}, { 0.0F,  1.0F,  0.0F}},
      {{ 0.5F,  0.5F, -0.5F}, { 0.0F,  1.0F,  0.0F}},
      {{ 0.5F,  0.5F,  0.5F}, { 0.0F,  1.0F,  0.0F}},
      {{-0.5F,  0.5F, -0.5F}, { 0.0F,  1.0F,  0.0F}},
      {{-0.5F,  0.5F,  0.5F}, { 0.0F,  1.0F,  0.0F}},
    };
    // clang-format on
}

Cuboid::Cuboid() :
  vertices_{create_unit_cube()} {}

void Cuboid::set_position(glm::vec3 position)
{
    position_ = position;
    set_needs_update();
}

void Cuboid::set_height(float height)
{
    height_ = height;
    set_needs_update();
}

void Cuboid::set_width(float width)
{
    width_ = width;
    set_needs_update();
}

void Cuboid::set_depth(float depth)
{
    depth_ = depth;
    set_needs_update();
}

void Cuboid::update()
{
    auto unit = create_unit_cube();
    std::transform(unit.cbegin(), unit.cend(), vertices_.begin(), [this](Vertex v) {
        v.position.x = v.position.x * width_ + position_.x;
        v.position.y = v.position.y * height_ + position_.y;
        v.position.z = v.position.z * depth_ + position_.z;
        return v;
    });
}
