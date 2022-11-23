#include <algorithm>

#include <glm/mat3x3.hpp>
#include <gsl/narrow>

#include "sphere.hpp"

// TODO: split the function up
// TODO: remove `smooth`, instead introduce another function `smoothen`
static std::vector<Vertex> create_unit_icosahedron(size_t degree, bool smooth)
{
    // clang-format off
    std::vector<glm::mat3> polygons {
      {{0.0, 0.525731, 0.850651}, {0.0, -0.525731, 0.850651}, {0.850651, 0.0, 0.525731}},
      {{0.0, 0.525731, 0.850651}, {-0.850651, 0.0, 0.525731}, {0.0, -0.525731, 0.850651}},
      {{0.0, 0.525731, -0.850651}, {0.850651, 0.0, -0.525731}, {0.0, -0.525731, -0.850651}},
      {{0.0, 0.525731, -0.850651}, {0.0, -0.525731, -0.850651}, {-0.850651, 0.0, -0.525731}},
      {{-0.525731, -0.850651, 0.0}, {0.525731, -0.850651, 0.0}, {0.0, -0.525731, 0.850651}},
      {{-0.525731, -0.850651, 0.0}, {0.0, -0.525731, -0.850651}, {0.525731, -0.850651, 0.0}},
      {{-0.525731, 0.850651, 0.0}, {0.0, 0.525731, 0.850651}, {0.525731, 0.850651, 0.0}},
      {{-0.525731, 0.850651, 0.0}, {0.525731, 0.850651, 0.0}, {0.0, 0.525731, -0.850651}},
      {{0.850651, 0.0, 0.525731}, {0.850651, 0.0, -0.525731}, {0.525731, 0.850651, 0.0}},
      {{0.850651, 0.0, 0.525731}, {0.525731, -0.850651, 0.0}, {0.850651, 0.0, -0.525731}},
      {{-0.850651, 0.0, 0.525731}, {-0.525731, 0.850651, 0.0}, {-0.850651, 0.0, -0.525731}},
      {{-0.850651, 0.0, 0.525731}, {-0.850651, 0.0, -0.525731}, {-0.525731, -0.850651, 0.0}},
      {{0.0, 0.525731, 0.850651}, {0.850651, 0.0, 0.525731}, {0.525731, 0.850651, 0.0}},
      {{0.0, -0.525731, 0.850651}, {0.525731, -0.850651, 0.0}, {0.850651, 0.0, 0.525731}},
      {{0.0, 0.525731, -0.850651}, {0.525731, 0.850651, 0.0}, {0.850651, 0.0, -0.525731}},
      {{0.0, -0.525731, -0.850651}, {0.850651, 0.0, -0.525731}, {0.525731, -0.850651, 0.0}},
      {{0.0, 0.525731, 0.850651}, {-0.525731, 0.850651, 0.0}, {-0.850651, 0.0, 0.525731}},
      {{0.0, -0.525731, 0.850651}, {-0.850651, 0.0, 0.525731}, {-0.525731, -0.850651, 0.0}},
      {{0.0, 0.525731, -0.850651}, {-0.850651, 0.0, -0.525731}, {-0.525731, 0.850651, 0.0}},
      {{0.0, -0.525731, -0.850651}, {-0.525731, -0.850651, 0.0}, {-0.850651, 0.0, -0.525731}}
    };
    // clang-format on

    for (size_t i = 0; i < degree; ++i) {
        std::vector<glm::mat3> refined_polygons{};
        refined_polygons.reserve(polygons.size() * 4);
        for (auto const& p : polygons) {
            auto a = p[0];
            auto b = p[1];
            auto c = p[2];
            auto mid_a = (a + b) / 2.0F;
            auto mid_b = (b + c) / 2.0F;
            auto mid_c = (a + c) / 2.0F;

            refined_polygons.emplace_back(a, mid_a, mid_c);
            refined_polygons.emplace_back(mid_a, b, mid_b);
            refined_polygons.emplace_back(mid_b, c, mid_c);
            refined_polygons.emplace_back(mid_a, mid_b, mid_c);
        }
        polygons = std::move(refined_polygons);
    }

    std::vector<Vertex> res{};
    res.reserve(polygons.size() * 3);
    for (auto const& p : polygons) {
        auto a = glm::normalize(p[0]);
        auto b = glm::normalize(p[1]);
        auto c = glm::normalize(p[2]);

        auto norm = glm::normalize(glm::cross(b - a, c - a));

        auto norm_a = smooth ? glm::normalize(a) : norm;
        auto norm_b = smooth ? glm::normalize(b) : norm;
        auto norm_c = smooth ? glm::normalize(c) : norm;

        // TODO: use emplace_back
        res.push_back({a, norm_a, {0.0F, 0.0F}, 0.0F});
        res.push_back({b, norm_b, {0.0F, 0.0F}, 0.0F});
        res.push_back({c, norm_c, {0.0F, 0.0F}, 0.0F});
    }

    return res;
}

// TODO: copy-paste from cube.cpp
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

Sphere::Sphere() :
  Sphere(0, false) {}

Sphere::Sphere(size_t degree, bool smooth) :
  Shape(),
  unit_icosahedron_{create_unit_icosahedron(degree, smooth)},
  vertices_{unit_icosahedron_},
  index_{create_cube_index(vertices_.size(), start_index_)} {}

// TODO: copy-paste from cube.cpp
void Sphere::set_start_index(size_t start_index)
{
    start_index_ = start_index;
    for (auto& i : index_) {
        i.x += gsl::narrow<uint32_t>(start_index_);
        i.y += gsl::narrow<uint32_t>(start_index_);
        i.z += gsl::narrow<uint32_t>(start_index_);
    }
}

void Sphere::set_position(glm::vec3 position)
{
    position_ = position;
    update_vertices();
}

void Sphere::set_size(float size)
{
    size_ = size;
    update_vertices();
}

void Sphere::set_glow(float glow)
{
    glow_ = glow;
    update_vertices();
}

void Sphere::update_vertices()
{
    std::transform(unit_icosahedron_.cbegin(), unit_icosahedron_.cend(), vertices_.begin(), [this](Vertex v) {
        v.position = v.position * size_ + position_;
        v.glow = glow_;
        return v;
    });
}
