#ifndef EXAMPLES_CUBE_HPP
#define EXAMPLES_CUBE_HPP

#include "../../playground/program.hpp"
#include "../../playground/texture.hpp"
#include "cube.hpp"

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
    std::vector<Cube> shapes_{};

    Cube& light_;
    Cube& cube1_;
    Cube& cube2_;
    Cube& cube3_;

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
