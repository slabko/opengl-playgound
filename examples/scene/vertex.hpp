//
// Created by slabko on 11/20/22.
//

#ifndef PLAYGROUND_VERTEX_CPP_HPP
#define PLAYGROUND_VERTEX_CPP_HPP

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

struct Vertex {
    glm::vec3 position{0.0F};
    glm::vec3 normal{0.0F};
    float glow{0.0F};

    Vertex(glm::vec3 position, glm::vec3 normal, float glow) :
      position{position}, normal{normal}, glow{glow} {}
};

#endif // PLAYGROUND_VERTEX_CPP_HPP
