#ifndef PLAYGROUND_STATIC_SHAPE_HPP
#define PLAYGROUND_STATIC_SHAPE_HPP

#include <vector>
#include <memory>

#include "shape.hpp"

class StaticShape : public Shape {
public:
    StaticShape() = default;
    explicit StaticShape(std::shared_ptr<VertexModel const> model);

    StaticShape(StaticShape const&) = default;
    StaticShape(StaticShape&&) noexcept = default;

    StaticShape& operator=(StaticShape const&) = default;
    StaticShape& operator=(StaticShape&&) noexcept = default;
    ~StaticShape() noexcept override = default;

    [[nodiscard]] size_t vertex_count() const override { return vertices_.size(); }

    [[nodiscard]] Vertex const* vbo_data() const override { return vertices_.data(); }

    [[nodiscard]]  float scale() { return scale_; }
    void set_scale(float scale);

    void update() override;

private:
    float scale_{1.0};
    std::vector<Vertex> vertices_{};
    std::shared_ptr<VertexModel const> model_{};
};

#endif // PLAYGROUND_STATIC_SHAPE_HPP
