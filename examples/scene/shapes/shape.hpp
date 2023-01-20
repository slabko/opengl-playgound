#ifndef PLAYGROUND_SHAPE_HPP
#define PLAYGROUND_SHAPE_HPP

#include <cstdlib>

#include <glm/vec3.hpp>

#include "../vertex.hpp"

class Shape {
public:
    Shape() = default;
    Shape(Shape const&) = default;
    Shape(Shape&&) noexcept = default;
    Shape& operator=(Shape const&) = default;
    Shape& operator=(Shape&&) noexcept = default;
    virtual ~Shape() = default;

    [[nodiscard]] virtual size_t vertex_count() const = 0;

    [[nodiscard]] virtual Vertex const* vbo_data() const = 0;

    bool needs_update() { return needs_update_; };

    virtual void update() = 0;

    // used by the scene class to store location in the memory;
    // these fields have no logic attached to the shape itself
    [[nodiscard]] size_t vbo_offset() const { return vbo_offset_; }
    void set_vbo_offset(size_t vbo_offset_bytes) { vbo_offset_ = vbo_offset_bytes; }

protected:
    void set_needs_update() { needs_update_ = true; };

private:
    size_t vbo_offset_{};
    bool needs_update_{true};
};

#endif // PLAYGROUND_SHAPE_HPP
