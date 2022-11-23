#include <algorithm>
#include <vector>

#include <glm/ext/matrix_transform.hpp>
#include <gsl/narrow>

#include "cube.hpp"

static std::vector<Vertex> create_unit_cube()
{
    // clang-format off
    return {
      {{-0.5F, -0.5F, -0.5F}, { 0.0F,  0.0F, -1.0F}, {0.0F, 0.0F}, 0.0F},
      {{ 0.5F,  0.5F, -0.5F}, { 0.0F,  0.0F, -1.0F}, {1.0F, 1.0F}, 0.0F},
      {{ 0.5F, -0.5F, -0.5F}, { 0.0F,  0.0F, -1.0F}, {1.0F, 0.0F}, 0.0F},
      {{ 0.5F,  0.5F, -0.5F}, { 0.0F,  0.0F, -1.0F}, {1.0F, 1.0F}, 0.0F},
      {{-0.5F, -0.5F, -0.5F}, { 0.0F,  0.0F, -1.0F}, {0.0F, 0.0F}, 0.0F},
      {{-0.5F,  0.5F, -0.5F}, { 0.0F,  0.0F, -1.0F}, {0.0F, 1.0F}, 0.0F},
      {{-0.5F, -0.5F,  0.5F}, { 0.0F,  0.0F,  1.0F}, {0.0F, 0.0F}, 0.0F},
      {{ 0.5F, -0.5F,  0.5F}, { 0.0F,  0.0F,  1.0F}, {1.0F, 0.0F}, 0.0F},
      {{ 0.5F,  0.5F,  0.5F}, { 0.0F,  0.0F,  1.0F}, {1.0F, 1.0F}, 0.0F},
      {{ 0.5F,  0.5F,  0.5F}, { 0.0F,  0.0F,  1.0F}, {1.0F, 1.0F}, 0.0F},
      {{-0.5F,  0.5F,  0.5F}, { 0.0F,  0.0F,  1.0F}, {0.0F, 1.0F}, 0.0F},
      {{-0.5F, -0.5F,  0.5F}, { 0.0F,  0.0F,  1.0F}, {0.0F, 0.0F}, 0.0F},
      {{-0.5F,  0.5F,  0.5F}, {-1.0F,  0.0F,  0.0F}, {0.0F, 0.0F}, 0.0F},
      {{-0.5F,  0.5F, -0.5F}, {-1.0F,  0.0F,  0.0F}, {1.0F, 0.0F}, 0.0F},
      {{-0.5F, -0.5F, -0.5F}, {-1.0F,  0.0F,  0.0F}, {1.0F, 1.0F}, 0.0F},
      {{-0.5F, -0.5F, -0.5F}, {-1.0F,  0.0F,  0.0F}, {1.0F, 1.0F}, 0.0F},
      {{-0.5F, -0.5F,  0.5F}, {-1.0F,  0.0F,  0.0F}, {0.0F, 1.0F}, 0.0F},
      {{-0.5F,  0.5F,  0.5F}, {-1.0F,  0.0F,  0.0F}, {0.0F, 0.0F}, 0.0F},
      {{ 0.5F,  0.5F,  0.5F}, { 1.0F,  0.0F,  0.0F}, {0.0F, 0.0F}, 0.0F},
      {{ 0.5F, -0.5F, -0.5F}, { 1.0F,  0.0F,  0.0F}, {1.0F, 1.0F}, 0.0F},
      {{ 0.5F,  0.5F, -0.5F}, { 1.0F,  0.0F,  0.0F}, {1.0F, 0.0F}, 0.0F},
      {{ 0.5F, -0.5F, -0.5F}, { 1.0F,  0.0F,  0.0F}, {1.0F, 1.0F}, 0.0F},
      {{ 0.5F,  0.5F,  0.5F}, { 1.0F,  0.0F,  0.0F}, {0.0F, 0.0F}, 0.0F},
      {{ 0.5F, -0.5F,  0.5F}, { 1.0F,  0.0F,  0.0F}, {0.0F, 1.0F}, 0.0F},
      {{-0.5F, -0.5F, -0.5F}, { 0.0F, -1.0F,  0.0F}, {0.0F, 0.0F}, 0.0F},
      {{ 0.5F, -0.5F, -0.5F}, { 0.0F, -1.0F,  0.0F}, {1.0F, 0.0F}, 0.0F},
      {{ 0.5F, -0.5F,  0.5F}, { 0.0F, -1.0F,  0.0F}, {1.0F, 1.0F}, 0.0F},
      {{ 0.5F, -0.5F,  0.5F}, { 0.0F, -1.0F,  0.0F}, {1.0F, 1.0F}, 0.0F},
      {{-0.5F, -0.5F,  0.5F}, { 0.0F, -1.0F,  0.0F}, {0.0F, 1.0F}, 0.0F},
      {{-0.5F, -0.5F, -0.5F}, { 0.0F, -1.0F,  0.0F}, {0.0F, 0.0F}, 0.0F},
      {{-0.5F,  0.5F, -0.5F}, { 0.0F,  1.0F,  0.0F}, {0.0F, 0.0F}, 0.0F},
      {{ 0.5F,  0.5F,  0.5F}, { 0.0F,  1.0F,  0.0F}, {1.0F, 1.0F}, 0.0F},
      {{ 0.5F,  0.5F, -0.5F}, { 0.0F,  1.0F,  0.0F}, {1.0F, 0.0F}, 0.0F},
      {{ 0.5F,  0.5F,  0.5F}, { 0.0F,  1.0F,  0.0F}, {1.0F, 1.0F}, 0.0F},
      {{-0.5F,  0.5F, -0.5F}, { 0.0F,  1.0F,  0.0F}, {0.0F, 0.0F}, 0.0F},
      {{-0.5F,  0.5F,  0.5F}, { 0.0F,  1.0F,  0.0F}, {0.0F, 1.0F}, 0.0F},
    };
    // clang-format on
}

Cube::Cube() :
  Shape(),
  vertices_{create_unit_cube()} {}

void Cube::set_position(glm::vec3 position)
{
    position_ = position;
    update_vertices();
}

void Cube::set_size(float size)
{
    size_ = size;
    update_vertices();
}

void Cube::set_glow(float glow)
{
    glow_ = glow;
    update_vertices();
}

void Cube::update_vertices()
{
    auto unit = create_unit_cube();
    std::transform(unit.cbegin(), unit.cend(), vertices_.begin(), [this](Vertex v) {
        v.position = v.position * size_ + position_;
        v.glow = glow_;
        return v;
    });
}
