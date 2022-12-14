#ifndef PLAYGROUND_SPHERE_HPP
#define PLAYGROUND_SPHERE_HPP

#include <vector>

#include "shape.hpp"

class Sphere : public Shape {
public:
    Sphere();

    Sphere(size_t degree, bool smooth);

    [[nodiscard]] glm::vec3 position() const { return position_; }
    void set_position(glm::vec3 position);

    [[nodiscard]] float size() const { return size_; }
    void set_size(float size);

    [[nodiscard]] size_t vertex_count() const override { return vertices_.size(); }

    [[nodiscard]] Vertex const* vbo_data() const override { return vertices_.data(); }

    void update() override;

private:
    std::vector<Vertex> unit_icosahedron_{};
    std::vector<Vertex> vertices_{};

    glm::vec3 position_{0.0F};
    float size_{1.0F};
};

#endif // PLAYGROUND_SPHERE_HPP
