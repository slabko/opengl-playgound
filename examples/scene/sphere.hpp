#ifndef PLAYGROUND_SPHERE_HPP
#define PLAYGROUND_SPHERE_HPP

#include <vector>

#include "shape.hpp"

class Sphere : public Shape {
public:
    Sphere();

    Sphere(size_t degree, bool smooth);

    [[nodiscard]] size_t vertex_count() const override { return vertices_.size(); }
    [[nodiscard]] size_t index_count() const override { return index_.size(); }

    [[nodiscard]] size_t start_index() const { return static_cast<uint32_t>(start_index_); }
    void set_start_index(size_t start_index) override;

    [[nodiscard]] glm::vec3 position() const { return position_; }
    void set_position(glm::vec3 position);

    [[nodiscard]] float size() const { return size_; }
    void set_size(float size);

    [[nodiscard]] float glow() const { return glow_; }
    void set_glow(float glow);

    [[nodiscard]] Vertex const* vbo_data() const override { return vertices_.data(); }
    [[nodiscard]] size_t vbo_size() const override { return vertex_count() * sizeof(Vertex); }

    [[nodiscard]] glm::uvec3 const* ibo_data() const override { return index_.data(); }
    [[nodiscard]] size_t ibo_size() const override { return index_count() * sizeof(glm::uvec3); }

private:
    std::vector<Vertex> unit_icosahedron_{};
    std::vector<Vertex> vertices_{};
    size_t start_index_{0};
    std::vector<glm::uvec3> index_{};

    glm::vec3 position_{0.0F};
    float size_{1.0F};
    float glow_{0.0F};

    void update_vertices();
};

#endif // PLAYGROUND_SPHERE_HPP
