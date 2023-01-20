#ifndef EXAMPLES_CUBE_HPP
#define EXAMPLES_CUBE_HPP

#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "../../playground/application.hpp"
#include "../../playground/program.hpp"
#include "../../playground/texture.hpp"
#include "shapes/cuboid.hpp"
#include "shapes/sphere.hpp"
#include "shapes/static_shape.hpp"
#include "materials.hpp"

class Scene : public playground::Application {
public:
    Scene();
    void init();
    void render() override;
    void update() override;
    void present_imgui() override;
    void drag_mouse(glm::ivec2 offset, KeyModifiers modifiers) override;
    void scroll_mouse(int val) override;

private:
    std::unique_ptr<playground::Program> program_{};
    std::unique_ptr<playground::Program> light_program_{};

    Sphere light_{1, false};
    Sphere sphere1_{2, true};
    Sphere sphere2_{2, false};
    Cuboid cube_{};
    Cuboid floor_{};
    playground::Texture white_pixel_diffuse_{1, 1, 3, GL_TEXTURE0};
    playground::Texture white_pixel_specular_{1, 1, 1, GL_TEXTURE1};
    playground::Texture cube_diffuse_{256, 256, 3, GL_TEXTURE0};
    playground::Texture cube_specular_{256, 256, 1, GL_TEXTURE1};
    StaticShape bunny_{};
    std::unique_ptr<StaticShape const> bunny_prototype_{};
    std::vector<Shape*> shapes_{};
    std::vector<uint32_t> indices_{};
    float scale_{1.0F};
    float camera_zoom_{glm::quarter_pi<float>()};
    float lens_shift_{};
    glm::vec3 camera_position_{0.0F, 0.0F, 5.0F};
    glm::vec2 camera_rotation_{0.0F, 0.0F};
    glm::vec2 world_rotation_{20.0F, 0.0F};
    glm::vec3 light_position_{-2.0F, 3.5F, 5.0F};

    glm::mat4 view_matrix();
    glm::mat4 proj_matrix();

    void set_material(materials::Material const& material);
};

#endif // EXAMPLES_CUBE_HPP
