#include <algorithm>

#include <glm/mat3x3.hpp>
#include <gsl/narrow>

#include "sphere.hpp"

static std::vector<Vertex> create_unit_icosahedron(size_t degree)
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
        glm::vec2 uv{0.0F, 0.0F};

        res.emplace_back(a, norm, uv, 0.0F);
        res.emplace_back(b, norm, uv, 0.0F);
        res.emplace_back(c, norm, uv, 0.0F);
    }

    return res;
}

static void smoothen(std::vector<Vertex>& vertices)
{
    for (auto& p : vertices) {
        p.normal = glm::normalize(p.position);
    }
}

Sphere::Sphere() :
  Sphere(0, false) {}

Sphere::Sphere(size_t degree, bool smooth) :
  Shape(),
  unit_icosahedron_{create_unit_icosahedron(degree)},
  vertices_{unit_icosahedron_}
{
    if (smooth) {
        smoothen(unit_icosahedron_);
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
