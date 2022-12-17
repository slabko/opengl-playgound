#ifndef PLAYGROUND_CUBOID_HPP
#define PLAYGROUND_CUBOID_HPP

#include <vector>

#include "../vertex.hpp"
#include "shape.hpp"

class Cuboid : public Shape {
public:
    Cuboid();

    [[nodiscard]] glm::vec3 position() const { return position_; }
    void set_position(glm::vec3 position);

    [[nodiscard]] float height() const { return height_; }
    void set_height(float height);

    [[nodiscard]] float width() const { return width_; }
    void set_width(float width);

    [[nodiscard]] float depth() const { return depth_; }
    void set_depth(float depth);

    [[nodiscard]] size_t vertex_count() const override { return vertices_.size(); }

    [[nodiscard]] Vertex const* vbo_data() const override { return vertices_.data(); }

    void update() override;

private:
    std::vector<Vertex> vertices_{};

    glm::vec3 position_{0.0F};
    float height_{1.0F};
    float width_{1.0F};
    float depth_{1.0F};
};

#endif // PLAYGROUND_CUBOID_HPP
