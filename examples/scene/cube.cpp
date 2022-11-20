#include <algorithm>

#include <glm/ext/matrix_transform.hpp>

#include "cube.hpp"

static std::vector<Vertex> create_unit_cube()
{
    // clang-format off
    return {
      {{-0.5F, -0.5F, -0.5F}, { 0.0F,  0.0F, -1.0F}, {0.0F, 0.0F}, 0.0F},
      {{ 0.5F, -0.5F, -0.5F}, { 0.0F,  0.0F, -1.0F}, {1.0F, 0.0F}, 0.0F},
      {{ 0.5F,  0.5F, -0.5F}, { 0.0F,  0.0F, -1.0F}, {1.0F, 1.0F}, 0.0F},
      {{ 0.5F,  0.5F, -0.5F}, { 0.0F,  0.0F, -1.0F}, {1.0F, 1.0F}, 0.0F},
      {{-0.5F,  0.5F, -0.5F}, { 0.0F,  0.0F, -1.0F}, {0.0F, 1.0F}, 0.0F},
      {{-0.5F, -0.5F, -0.5F}, { 0.0F,  0.0F, -1.0F}, {0.0F, 0.0F}, 0.0F},
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
      {{ 0.5F,  0.5F, -0.5F}, { 1.0F,  0.0F,  0.0F}, {1.0F, 0.0F}, 0.0F},
      {{ 0.5F, -0.5F, -0.5F}, { 1.0F,  0.0F,  0.0F}, {1.0F, 1.0F}, 0.0F},
      {{ 0.5F, -0.5F, -0.5F}, { 1.0F,  0.0F,  0.0F}, {1.0F, 1.0F}, 0.0F},
      {{ 0.5F, -0.5F,  0.5F}, { 1.0F,  0.0F,  0.0F}, {0.0F, 1.0F}, 0.0F},
      {{ 0.5F,  0.5F,  0.5F}, { 1.0F,  0.0F,  0.0F}, {0.0F, 0.0F}, 0.0F},
      {{-0.5F, -0.5F, -0.5F}, { 0.0F, -1.0F,  0.0F}, {0.0F, 0.0F}, 0.0F},
      {{ 0.5F, -0.5F, -0.5F}, { 0.0F, -1.0F,  0.0F}, {1.0F, 0.0F}, 0.0F},
      {{ 0.5F, -0.5F,  0.5F}, { 0.0F, -1.0F,  0.0F}, {1.0F, 1.0F}, 0.0F},
      {{ 0.5F, -0.5F,  0.5F}, { 0.0F, -1.0F,  0.0F}, {1.0F, 1.0F}, 0.0F},
      {{-0.5F, -0.5F,  0.5F}, { 0.0F, -1.0F,  0.0F}, {0.0F, 1.0F}, 0.0F},
      {{-0.5F, -0.5F, -0.5F}, { 0.0F, -1.0F,  0.0F}, {0.0F, 0.0F}, 0.0F},
      {{-0.5F,  0.5F, -0.5F}, { 0.0F,  1.0F,  0.0F}, {0.0F, 0.0F}, 0.0F},
      {{ 0.5F,  0.5F, -0.5F}, { 0.0F,  1.0F,  0.0F}, {1.0F, 0.0F}, 0.0F},
      {{ 0.5F,  0.5F,  0.5F}, { 0.0F,  1.0F,  0.0F}, {1.0F, 1.0F}, 0.0F},
      {{ 0.5F,  0.5F,  0.5F}, { 0.0F,  1.0F,  0.0F}, {1.0F, 1.0F}, 0.0F},
      {{-0.5F,  0.5F,  0.5F}, { 0.0F,  1.0F,  0.0F}, {0.0F, 1.0F}, 0.0F},
      {{-0.5F,  0.5F, -0.5F}, { 0.0F,  1.0F,  0.0F}, {0.0F, 0.0F}, 0.0F},
    };
    // clang-format on
}

static std::vector<glm::uvec3> create_cube_index(size_t vertices_count, size_t offset)
{
    auto uint_offset = static_cast<uint32_t>(offset);
    std::vector<glm::uvec3> res(vertices_count / 3);
    for (uint32_t i = 0; i < res.size(); ++i) {
        res[i].x = uint_offset + i * 3;
        res[i].y = uint_offset + i * 3 + 1;
        res[i].z = uint_offset + i * 3 + 2;
    }
    return res;
}

Cube::Cube(size_t index_offset) :
  vertices_{create_unit_cube()},
  index_offset_{index_offset},
  index_{create_cube_index(vertices_.size(), index_offset_)} {}

void Cube::set_index_offset(uint32_t index_offset)
{
    index_offset_ = index_offset;
    for (auto& i : index_) {
        i.x += index_offset_;
        i.y += index_offset_;
        i.z += index_offset_;
    }
}

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
