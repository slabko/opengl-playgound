#ifndef PLAYGROUND_CUBE_HPP
#define PLAYGROUND_CUBE_HPP

#include <vector>

#include "vertex.hpp"

class Cube {
public:
    Cube() :
      Cube(0) {}

    explicit Cube(size_t index_offset);

    [[nodiscard]] size_t vertex_count() const { return vertices_.size(); }
    [[nodiscard]] size_t index_count() const { return index_.size(); }

    // Controls the index offset used to create new indices,
    // for example, if we already have few shapes that take 20 indices
    // from 0 to 19, then we start with 20 and set start index to 20
    [[nodiscard]] size_t start_index() const { return static_cast<uint32_t>(start_index_); }
    void set_start_index(size_t start_index);

    [[nodiscard]] glm::vec3 position() const { return position_; }
    void set_position(glm::vec3 position);

    [[nodiscard]] float size() const { return size_; }
    void set_size(float size);

    [[nodiscard]] float glow() const { return glow_; }
    void set_glow(float glow);

    [[nodiscard]] Vertex const* vbo_data() const { return vertices_.data(); }
    [[nodiscard]] size_t vbo_size() const { return vertex_count() * sizeof(Vertex); }

    [[nodiscard]] glm::uvec3 const* ibo_data() const { return index_.data(); }
    [[nodiscard]] size_t ibo_size() const { return index_count() * sizeof(glm::uvec3); }

    // used by the scene class to store location in the memory;
    // these fields no logic attached to the shape itself
    [[nodiscard]] size_t vbo_offset() const { return vbo_offset_; }
    void set_vbo_offset(size_t vbo_offset) { vbo_offset_ = vbo_offset; }

    [[nodiscard]] size_t ibo_offset() const { return ibo_offset_; }
    void set_ibo_offset(size_t ibo_offset) { ibo_offset_ = ibo_offset; }

private:
    std::vector<Vertex> vertices_{};
    size_t start_index_{0};
    std::vector<glm::uvec3> index_{};

    glm::vec3 position_{0.0F};
    float size_{1.0F};
    float glow_{0.0F};

    size_t vbo_offset_{};
    size_t ibo_offset_{};

    void update_vertices();
};

#endif // PLAYGROUND_CUBE_HPP
