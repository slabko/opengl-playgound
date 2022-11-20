#ifndef PLAYGROUND_CUBE_HPP
#define PLAYGROUND_CUBE_HPP

#include <vector>

#include "vertex.hpp"

class Cube {
public:
    Cube() = delete;
    explicit Cube(size_t index_offset = 0);
    Vertex const* vertex_data() { return vertices_.data(); }
    size_t vertex_count() { return vertices_.size(); }
    size_t vertex_data_size() { return vertex_count() * sizeof(Vertex); }

    glm::uvec3 const* index_data() { return index_.data(); }
    size_t index_count() { return index_.size(); }
    size_t index_data_size() { return index_count() * sizeof(glm::uvec3); }

    size_t index_offset() { return static_cast<uint32_t>(index_offset_); }
    void set_index_offset(uint32_t index_offset);

    glm::vec3 postion() { return position_; }
    void set_position(glm::vec3 position);

    float size() { return size_; }
    void set_size(float size);

    float glow() { return glow_; }
    void set_glow(float glow);

private:
    std::vector<Vertex> vertices_{};
    size_t index_offset_{0};
    std::vector<glm::uvec3> index_{};

    glm::vec3 position_{0.0F};
    float size_{1.0F};
    float glow_{0.0F};

    void update_vertices();
};

#endif // PLAYGROUND_CUBE_HPP
