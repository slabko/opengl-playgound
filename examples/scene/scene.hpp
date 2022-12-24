#ifndef EXAMPLES_CUBE_HPP
#define EXAMPLES_CUBE_HPP

#include "../../playground/program.hpp"
#include "../../playground/texture.hpp"
#include "shapes/cuboid.hpp"
#include "shapes/sphere.hpp"
#include "shapes/static_shape.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

class Scene : public playground::Program {
public:
    Scene();
    void init();
    void render() override;
    void update() override;
    void present_imgui() override;
    void drag_mouse(glm::ivec2 offset, KeyModifiers modifiers) override;
    void scroll_mouse(int val) override;

private:
    Sphere light_{1, false};
    Sphere sphere1_{2, true};
    Sphere sphere2_{2, false};
    Cuboid cube1_{};
    StaticShape bunny_{};

    std::unique_ptr<StaticShape const> bunny_prototype_{nullptr};

    std::vector<Shape*> shapes_{};

    std::vector<uint32_t> indices_{};

    float cube_size_{1.0F};
    float camera_zoom_{glm::half_pi<float>()};
    float lens_shift_{};
    glm::vec3 camera_position_{0.0F, 0.0F, 5.0F};
    glm::vec2 camera_rotation_{0.0F, 0.0F};
    glm::vec2 world_rotation_{0.0F, 0.0F};
    glm::vec3 light_position_{0.0F, 1.5F, -0.5F};
};

#endif // EXAMPLES_CUBE_HPP
