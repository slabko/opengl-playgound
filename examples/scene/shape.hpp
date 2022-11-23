#ifndef PLAYGROUND_SHAPE_HPP
#define PLAYGROUND_SHAPE_HPP

#include <cstdlib>

#include <glm/vec3.hpp>

#include "vertex.hpp"

class Shape {
public:
    [[nodiscard]] virtual size_t vertex_count() const = 0;
    [[nodiscard]] virtual size_t index_count() const = 0;

    [[nodiscard]] virtual Vertex const* vbo_data() const = 0;
    [[nodiscard]] virtual size_t vbo_size() const = 0;

    [[nodiscard]] virtual glm::uvec3 const* ibo_data() const = 0;
    [[nodiscard]] virtual size_t ibo_size() const = 0;

    // Controls the index offset used to create new indices,
    // for example, if we already have few shapes that take 20 indices
    // from 0 to 19, then we start with 20 and set start index to 20
    virtual void set_start_index(size_t start_index) = 0;

    // used by the scene class to store location in the memory;
    // these fields no logic attached to the shape itself
    [[nodiscard]] size_t vbo_offset() const { return vbo_offset_; }
    void set_vbo_offset(size_t vbo_offset) { vbo_offset_ = vbo_offset; }

    [[nodiscard]] size_t ibo_offset() const { return ibo_offset_; }
    void set_ibo_offset(size_t ibo_offset) { ibo_offset_ = ibo_offset; }

    virtual ~Shape() = default;

private:
    size_t vbo_offset_{};
    size_t ibo_offset_{};
};

#endif // PLAYGROUND_SHAPE_HPP
