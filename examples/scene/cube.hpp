#ifndef PLAYGROUND_CUBE_HPP
#define PLAYGROUND_CUBE_HPP

#include <vector>

#include "shape.hpp"
#include "vertex.hpp"

class Cube : public Shape {
public:
    Cube();

    [[nodiscard]] glm::vec3 position() const { return position_; }
    void set_position(glm::vec3 position);

    [[nodiscard]] float size() const { return size_; }
    void set_size(float size);

    [[nodiscard]] float glow() const { return glow_; }
    void set_glow(float glow);

    [[nodiscard]] size_t vertex_count() const override { return vertices_.size(); }

    [[nodiscard]] Vertex const* vbo_data() const override { return vertices_.data(); }

private:
    std::vector<Vertex> vertices_{};

    glm::vec3 position_{0.0F};
    float size_{1.0F};
    float glow_{0.0F};

    void update_vertices();
};

#endif // PLAYGROUND_CUBE_HPP
