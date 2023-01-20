#ifndef PLAYGROUND_MATERIALS_HPP
#define PLAYGROUND_MATERIALS_HPP

#include <glm/vec3.hpp>

namespace materials {

struct Material {

    glm::vec3 ambient{};
    glm::vec3 diffuse{};
    glm::vec3 specular{};
    float shininess{};

    constexpr Material(
      float ambient_red, float ambient_green, float ambient_blue,
      float diffuse_red, float diffuse_green, float diffuse_blue,
      float specular_red, float specular_green, float specular_blue,
      float shininess) :
      ambient{ambient_red, ambient_green, ambient_blue},
      diffuse{diffuse_red, diffuse_green, diffuse_blue},
      specular{specular_red, specular_green, specular_blue},
      shininess{shininess} {};
};

static constexpr Material Wood{
  0.05F, 0.05F, 0.05F,
  0.5F, 0.5F, 0.5F,
  0.9F, 0.9F, 0.9F,
  0.278125F * 128.0F};

static constexpr Material WhiteRubber{
  0.05F, 0.05F, 0.05F,
  0.5F, 0.5F, 0.5F,
  0.7F, 0.7F, 0.7F,
  0.078125F * 128.0F};

static constexpr Material BlackRubber{
  0.02F, 0.02F, 0.02F,
  0.01F, 0.01F, 0.01F,
  0.40F, 0.40F, 0.40F,
  0.078125F * 128.0F};

static constexpr Material YellowRubber{
  0.05F, 0.05F, 0.0F,
  0.5F, 0.5F, 0.4F,
  0.7F, 0.7F, 0.04F,
  0.078125F * 128.0F};

static constexpr Material BlackPlastic{
  0.0F, 0.0F, 0.0F,
  0.01F, 0.01F, 0.01F,
  0.50F, 0.50F, 0.50F,
  0.25F * 128.0F};

static constexpr Material Gold{
  0.24725F, 0.1995F, 0.0745F,
  0.75164F, 0.60648F, 0.22648F,
  0.628281F, 0.555802F, 0.366065F,
  0.4F * 128.0F};

static constexpr Material Bronze{
  0.2125F, 0.1275F, 0.054F,
  0.714F, 0.4284F, 0.18144F,
  0.393548F, 0.271906F, 0.166721F,
  0.2F * 128.0F};

} // namespace materials

#endif // PLAYGROUND_MATERIALS_HPP
