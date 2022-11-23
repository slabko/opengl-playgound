#ifndef PLAYGROUND_SHAPE_HPP
#define PLAYGROUND_SHAPE_HPP

#include <cstdlib>

#include <glm/vec3.hpp>

#include "vertex.hpp"

class Shape {
public:
    [[nodiscard]] virtual size_t vertex_count() const = 0;

    [[nodiscard]] virtual Vertex const* vbo_data() const = 0;

    // used by the scene class to store location in the memory;
    // these fields have no logic attached to the shape itself
    [[nodiscard]] size_t vbo_offset_bytes() const { return vbo_offset_bytes_; }
    void set_vbo_offset_bytes(size_t vbo_offset_bytes) { vbo_offset_bytes_ = vbo_offset_bytes; }

    virtual ~Shape() = default;

private:
    size_t vbo_offset_bytes_{};
};

#endif // PLAYGROUND_SHAPE_HPP
