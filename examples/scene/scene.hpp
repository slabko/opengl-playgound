#ifndef EXAMPLES_CUBE_HPP
#define EXAMPLES_CUBE_HPP

#include "../../playground/program.hpp"
#include "../../playground/texture.hpp"
#include "shapes/cuboid.hpp"
#include "shapes/sphere.hpp"

#include <boost/ptr_container/ptr_vector.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

class Scene : public playground::Program {
public:
    Scene();
    void render() override;
    void update() override;
    void present_imgui() override;
    void resize(int width, int height) override;

private:
    Sphere light_{2, false};
    Sphere sphere1_{1, true};
    Sphere sphere2_{3, false};
    Cuboid cube1_{};

    std::vector<Shape*> shapes_{};

    std::vector<glm::uvec3> indices_{};

    playground::Texture texture_{};

    bool show_cube_{true};
    bool animate_{false};
    float cube_size_{1.0F};
    glm::vec3 camera_position_{0.0F, 1.0F, 5.0F};
    glm::vec2 camera_rotation_{0.0F, 0.0F};
    glm::vec3 light_position_{0.0F, 1.5F, -0.5F};

    void update_view_projection();
};

#endif // EXAMPLES_CUBE_HPP
