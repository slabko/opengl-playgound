#ifndef PLAYGROUND_VERTEX_CPP_HPP
#define PLAYGROUND_VERTEX_CPP_HPP

#include <string>
#include <vector>
#include <memory>

#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 position{0.0F};
    glm::vec3 normal{0.0F};
    glm::vec2 uv{0.0F};

    Vertex(glm::vec3 position, glm::vec3 normal) :
      position{position}, normal{normal} {}

    Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 uv) :
      position{position}, normal{normal}, uv{uv} {}
};

using VertexModel = std::vector<Vertex>;

/*******************************************************************************
 * parses and wavefront file and returns a 3D model,
 * i.e. vertices grouped by 3 to represent a polygon and normal vector
 * for each vertex. The normal vectors are automatically generated based
 * on normal vectors of adjacent polygons for each vertex
 ******************************************************************************/
VertexModel read_vertex_model(std::string const& file_path);

/*******************************************************************************
 * loads a model and instantiates and object feeding it with this model
 ******************************************************************************/
template<class T>
T load_model(std::string const& file_path)
{
    auto model = read_vertex_model(file_path);
    auto res = T{std::make_shared<VertexModel const>(std::move(model))};
    return res;
}


#endif // PLAYGROUND_VERTEX_CPP_HPP
