#include <algorithm>
#include <cmath>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>

#include <imgui.h>

#include "../playground/png.hpp"

#include "cube.hpp"

static char const* vertex_shader = R"(
#version 460

in vec3 position;
in vec3 normal;
in vec2 uv;
in float light;

uniform mat4   model;
uniform mat4   view;
uniform mat4   proj;

uniform vec3   light_position;

out vec3 v_normal;
out vec2 v_uv;
out vec3 v_fragment_position;
out float v_light;

void main()
{
    v_uv = uv;
    v_light = light;
    v_normal = normal;

    // each vec4 in the constructor defines columns
    mat4 light_translate = mat4(
        vec4(1.0F, 0.0F, 0.0F, 0.0F),
        vec4(0.0F, 1.0F, 0.0F, 0.0F),
        vec4(0.0F, 0.0F, 1.0F, 0.0F),
        vec4(light_position, 1.0F)
    );

    if (!bool(v_light)) {
        gl_Position = proj * view * model * vec4(position, 1.0);
        v_fragment_position = vec3(model * vec4(position, 1.0));
    } else {
        gl_Position = proj * view * model * light_translate * vec4(position, 1.0);
        v_fragment_position = vec3(model * light_translate * vec4(position, 1.0));
    }
}
)";

static char const* fragment_shader = R"(
#version 460

in vec3 v_normal;
in vec2 v_uv;
in float v_light;
in vec3 v_fragment_position;
out vec4 frag_color;

uniform vec3 light_position;

uniform sampler2D texture;

vec3 object_color = vec3(1.0, 0.5, 0.31);
vec3 light_color = vec3(1.0, 1.0, 1.0);
float ambient_strength = 0.1F;

uniform vec3 camera_position;

float specular_strength = 0.7;

void main()
{
    if (!bool(v_light)) {
        vec3 ambient = ambient_strength * light_color;

        vec3 norm = normalize(v_normal);

        vec3 light_dir = normalize(light_position - v_fragment_position);
        float diff = max(dot(norm, light_dir), 0.0);
        vec3 diffuse = diff * light_color;

        vec3 view_dir = normalize(camera_position - v_fragment_position);
        vec3 reflect_dir = reflect(-light_dir, norm);

        float spec = pow(max(dot(view_dir, reflect_dir), 0.0F), 64);
        vec3 specular = specular_strength * spec * light_color;

        vec3 res = (ambient + diffuse + specular) * object_color;

        frag_color = vec4(res, 1.0);
    } else {
        frag_color = vec4(light_color, 1.0);
    }
}
)";

struct Vertex {
    [[maybe_unused]] glm::vec3 position{0.0F};
    [[maybe_unused]] glm::vec3 normal{0.0F};
    [[maybe_unused]] glm::vec2 uv{0.0F};
    [[maybe_unused]] float light{0.0F};
};

static std::vector<Vertex> create_cube()
{
    // clang-format off
    std::vector<Vertex> cube = {
        {{-1.0F, -1.0F, -1.0F}, { 0.0F,  0.0F, -1.0F}, {0.0F, 0.0F}, 0.0F},
        {{ 1.0F, -1.0F, -1.0F}, { 0.0F,  0.0F, -1.0F}, {1.0F, 0.0F}, 0.0F},
        {{ 1.0F,  1.0F, -1.0F}, { 0.0F,  0.0F, -1.0F}, {1.0F, 1.0F}, 0.0F},
        {{ 1.0F,  1.0F, -1.0F}, { 0.0F,  0.0F, -1.0F}, {1.0F, 1.0F}, 0.0F},
        {{-1.0F,  1.0F, -1.0F}, { 0.0F,  0.0F, -1.0F}, {0.0F, 1.0F}, 0.0F},
        {{-1.0F, -1.0F, -1.0F}, { 0.0F,  0.0F, -1.0F}, {0.0F, 0.0F}, 0.0F},
        {{-1.0F, -1.0F,  1.0F}, { 0.0F,  0.0F,  1.0F}, {0.0F, 0.0F}, 0.0F},
        {{ 1.0F, -1.0F,  1.0F}, { 0.0F,  0.0F,  1.0F}, {1.0F, 0.0F}, 0.0F},
        {{ 1.0F,  1.0F,  1.0F}, { 0.0F,  0.0F,  1.0F}, {1.0F, 1.0F}, 0.0F},
        {{ 1.0F,  1.0F,  1.0F}, { 0.0F,  0.0F,  1.0F}, {1.0F, 1.0F}, 0.0F},
        {{-1.0F,  1.0F,  1.0F}, { 0.0F,  0.0F,  1.0F}, {0.0F, 1.0F}, 0.0F},
        {{-1.0F, -1.0F,  1.0F}, { 0.0F,  0.0F,  1.0F}, {0.0F, 0.0F}, 0.0F},
        {{-1.0F,  1.0F,  1.0F}, {-1.0F,  0.0F,  0.0F}, {0.0F, 0.0F}, 0.0F},
        {{-1.0F,  1.0F, -1.0F}, {-1.0F,  0.0F,  0.0F}, {1.0F, 0.0F}, 0.0F},
        {{-1.0F, -1.0F, -1.0F}, {-1.0F,  0.0F,  0.0F}, {1.0F, 1.0F}, 0.0F},
        {{-1.0F, -1.0F, -1.0F}, {-1.0F,  0.0F,  0.0F}, {1.0F, 1.0F}, 0.0F},
        {{-1.0F, -1.0F,  1.0F}, {-1.0F,  0.0F,  0.0F}, {0.0F, 1.0F}, 0.0F},
        {{-1.0F,  1.0F,  1.0F}, {-1.0F,  0.0F,  0.0F}, {0.0F, 0.0F}, 0.0F},
        {{ 1.0F,  1.0F,  1.0F}, { 1.0F,  0.0F,  0.0F}, {0.0F, 0.0F}, 0.0F},
        {{ 1.0F,  1.0F, -1.0F}, { 1.0F,  0.0F,  0.0F}, {1.0F, 0.0F}, 0.0F},
        {{ 1.0F, -1.0F, -1.0F}, { 1.0F,  0.0F,  0.0F}, {1.0F, 1.0F}, 0.0F},
        {{ 1.0F, -1.0F, -1.0F}, { 1.0F,  0.0F,  0.0F}, {1.0F, 1.0F}, 0.0F},
        {{ 1.0F, -1.0F,  1.0F}, { 1.0F,  0.0F,  0.0F}, {0.0F, 1.0F}, 0.0F},
        {{ 1.0F,  1.0F,  1.0F}, { 1.0F,  0.0F,  0.0F}, {0.0F, 0.0F}, 0.0F},
        {{-1.0F, -1.0F, -1.0F}, { 0.0F, -1.0F,  0.0F}, {0.0F, 0.0F}, 0.0F},
        {{ 1.0F, -1.0F, -1.0F}, { 0.0F, -1.0F,  0.0F}, {1.0F, 0.0F}, 0.0F},
        {{ 1.0F, -1.0F,  1.0F}, { 0.0F, -1.0F,  0.0F}, {1.0F, 1.0F}, 0.0F},
        {{ 1.0F, -1.0F,  1.0F}, { 0.0F, -1.0F,  0.0F}, {1.0F, 1.0F}, 0.0F},
        {{-1.0F, -1.0F,  1.0F}, { 0.0F, -1.0F,  0.0F}, {0.0F, 1.0F}, 0.0F},
        {{-1.0F, -1.0F, -1.0F}, { 0.0F, -1.0F,  0.0F}, {0.0F, 0.0F}, 0.0F},
        {{-1.0F,  1.0F, -1.0F}, { 0.0F,  1.0F,  0.0F}, {0.0F, 0.0F}, 0.0F},
        {{ 1.0F,  1.0F, -1.0F}, { 0.0F,  1.0F,  0.0F}, {1.0F, 0.0F}, 0.0F},
        {{ 1.0F,  1.0F,  1.0F}, { 0.0F,  1.0F,  0.0F}, {1.0F, 1.0F}, 0.0F},
        {{ 1.0F,  1.0F,  1.0F}, { 0.0F,  1.0F,  0.0F}, {1.0F, 1.0F}, 0.0F},
        {{-1.0F,  1.0F,  1.0F}, { 0.0F,  1.0F,  0.0F}, {0.0F, 1.0F}, 0.0F},
        {{-1.0F,  1.0F, -1.0F}, { 0.0F,  1.0F,  0.0F}, {0.0F, 0.0F}, 0.0F},
    };
    // clang-format on

    std::vector<Vertex> res(cube.size() * 2);

    std::transform(cube.cbegin(), cube.cend(), res.begin(), [](auto v) {
        v.position *= 0.5;
        return v;
    });

    auto cube_size = static_cast<int64_t>(cube.size());
    std::transform(cube.cbegin(), cube.cend(), res.begin() + cube_size, [](auto v) {
        v.position *= 0.1;
        v.light = 1.0F;
        return v;
    });

    return res;
}

Cube::Cube() :
  playground::Program{vertex_shader, fragment_shader},
  texture_{256, 256, 3}
{
    auto glm_position = create_cube();

    auto size = sizeof(Vertex) * glm_position.size();
    alloc_vbo(size);
    upload_vbo(glm_position.data(), 0, size);

    assign_vbo("position", decltype(Vertex::position)::length(), sizeof(Vertex), offsetof(Vertex, position));
    assign_vbo("normal", decltype(Vertex::normal)::length(), sizeof(Vertex), offsetof(Vertex, normal));
    assign_vbo("uv", decltype(Vertex::uv)::length(), sizeof(Vertex), offsetof(Vertex, uv));
    assign_vbo("light", 1, sizeof(Vertex), offsetof(Vertex, light));

    std::vector<glm::uvec3> glm_index(glm_position.size() / 3);
    for (uint32_t i = 0; i < glm_index.size(); ++i) {
        glm_index[i].x = i * 3;
        glm_index[i].y = i * 3 + 1;
        glm_index[i].z = i * 3 + 2;
    }

    auto index_size = sizeof(glm::uvec3) * glm_index.size();
    alloc_ibo(index_size);
    upload_ibo(glm_index.data(), 0, index_size);

    auto model = glm::mat4(1.0F);
    auto view = glm::translate(glm::mat4(1.0F), {0.0F, 0.0F, -5.0F});
    auto proj = glm::perspective(glm::quarter_pi<float>(), 1920.0F / 1080.0F, 2.0F, 100.0F);

    set_uniform_data("model", model);
    set_uniform_data("view", view);
    set_uniform_data("proj", proj);

    set_uniform_data("light_position", light_position_);
    set_uniform_data("camera_position", glm::vec3{0.0F, 0.0F, 5.0F});

    auto image = png::read_png<png::RgbPixel>("textures/crate.png");
    texture_.upload(image.pixels, 0, 0, image.width, image.height);
}

void Cube::present_imgui()
{
    ImGui::Begin("Configuration");
    ImGui::Text("Show the animated cube");
    ImGui::Checkbox("Show Cube", &show_cube_);
    ImGui::Checkbox("Animate Cube", &animate_);
    ImGui::Text("Application average %.1f FPS", ImGui::GetIO().Framerate);
    ImGui::SliderFloat2("Rotation", glm::value_ptr(camera_rotation_), 0, 360);
    ImGui::SliderFloat3("Light Position", glm::value_ptr(light_position_), -5, 5);
    ImGui::End();
}

void Cube::update_view_projection()
{
    auto view = glm::mat4(1.0F);
    view = glm::translate(view, {-camera_position_.x, -camera_position_.y, -camera_position_.z});
    view = glm::rotate(view, glm::radians(camera_rotation_.x), {1.0F, 0.0F, 0.0F});
    view = glm::rotate(view, glm::radians(camera_rotation_.y), {0.0F, 1.0F, 0.0F});

    set_uniform_data("view", view);

    // Undo camera transformation
    auto camera_position = glm::inverse(view) * glm::vec4{0.0F, 0.0F, 0.0F, 1.0F};
    set_uniform_data("camera_position", glm::vec3(camera_position));
}

void Cube::update()
{
    if (animate_) {
        camera_rotation_[0] = std::fmod(camera_rotation_[0] + 1.0F, 360.0F);
        camera_rotation_[1] = std::fmod(camera_rotation_[1] + 1.0F, 360.0F);
    }

    update_view_projection();

    set_uniform_data("light_position", light_position_);
}

void Cube::render()
{
    if (show_cube_) {
        texture_.bind();
        draw_indices(72);
    }
}

void Cube::resize(int width, int height)
{
    auto aspect_ratio = static_cast<float>(width) / static_cast<float>(height);
    auto proj = glm::perspective(glm::quarter_pi<float>(), aspect_ratio, 2.0F, 100.0F);
    set_uniform_data("proj", proj);
}
