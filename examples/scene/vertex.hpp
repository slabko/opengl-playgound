#ifndef PLAYGROUND_VERTEX_CPP_HPP
#define PLAYGROUND_VERTEX_CPP_HPP

#include <string>
#include <vector>
#include <memory>

#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 position{0.0F};
    glm::vec3 normal{0.0F};

    Vertex(glm::vec3 position, glm::vec3 normal) :
      position{position}, normal{normal} {}
};

using VertexModel = std::vector<Vertex>;

VertexModel read_vertex_model(std::string const& file_path);

template<class T>
T load_model(std::string const& file_path)
{
    auto model = read_vertex_model(file_path);
    auto res = T{std::make_shared<VertexModel const>(std::move(model))};
    return res;
}


#endif // PLAYGROUND_VERTEX_CPP_HPP
