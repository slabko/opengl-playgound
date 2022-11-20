#include <algorithm>
#include <cmath>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>

#include <imgui.h>

#include "../../playground/png.hpp"
#include "vertex.hpp"

#include "scene.hpp"

static char const* vertex_shader = R"(
#version 460

in vec3 position;
in vec3 normal;
in vec2 uv;
in float glow;

uniform mat4   model;
uniform mat4   view;
uniform mat4   proj;

uniform vec3   light_position;

out vec3 v_normal;
out vec2 v_uv;
out vec3 v_fragment_position;
out float v_glow;

void main()
{
    v_uv = uv;
    v_glow = glow;
    v_normal = normal;

    gl_Position = proj * view * model * vec4(position, 1.0);
    v_fragment_position = vec3(model * vec4(position, 1.0));
}
)";

static char const* fragment_shader = R"(
#version 460

in vec3 v_normal;
in vec2 v_uv;
in float v_glow;
in vec3 v_fragment_position;
out vec4 frag_color;

uniform vec3 light_position;

uniform sampler2D texture_image;

vec3 object_color = vec3(1.0, 0.5, 0.31);
vec3 light_color = vec3(1.0, 1.0, 1.0);
float ambient_strength = 0.1F;

uniform vec3 camera_position;

float specular_strength = 0.7;

void main()
{
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

    frag_color = texture(texture_image, v_uv) * vec4(res, 1.0) + v_glow;
}
)";

Scene::Scene() :
  playground::Program{vertex_shader, fragment_shader},
  cube_{0},
  light_{cube_.vertex_count()},
  texture_{256, 256, 3}
{
    light_.set_glow(1.0F);
    light_.set_size(0.2F);
    light_.set_position(light_position_);

    auto vbo_size = cube_.vertex_data_size() + light_.vertex_data_size();
    alloc_vbo(vbo_size);
    upload_vbo(cube_.vertex_data(), 0, cube_.vertex_data_size());
    upload_vbo(light_.vertex_data(), cube_.vertex_data_size(), light_.vertex_data_size());

    assign_vbo("position", decltype(Vertex::position)::length(), sizeof(Vertex), offsetof(Vertex, position));
    assign_vbo("normal", decltype(Vertex::normal)::length(), sizeof(Vertex), offsetof(Vertex, normal));
    assign_vbo("uv", decltype(Vertex::uv)::length(), sizeof(Vertex), offsetof(Vertex, uv));
    assign_vbo("glow", 1, sizeof(Vertex), offsetof(Vertex, glow));

    auto ibo_size = cube_.index_data_size() + light_.vertex_data_size();
    alloc_ibo(ibo_size);
    upload_ibo(cube_.index_data(), 0, cube_.index_data_size());
    upload_ibo(light_.index_data(), cube_.index_data_size(), light_.index_data_size());

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

void Scene::present_imgui()
{
    ImGui::Begin("Configuration");
    ImGui::Text("Show the animated cube");
    ImGui::Checkbox("Show Scene", &show_cube_);
    ImGui::Checkbox("Animate Scene", &animate_);
    ImGui::Text("Application average %.1f FPS", ImGui::GetIO().Framerate);
    ImGui::SliderFloat2("Rotation", glm::value_ptr(camera_rotation_), 0, 360);

    if (ImGui::SliderFloat3("Light Position", glm::value_ptr(light_position_), -5, 5)) {
        light_.set_position(light_position_);
        upload_vbo(light_.vertex_data(), cube_.vertex_data_size(), light_.vertex_data_size());
    }

    if (ImGui::SliderFloat("Cube Size", &cube_size_, 0.0F, 2.0F)) {
        cube_.set_size(cube_size_);
        upload_vbo(cube_.vertex_data(), 0, cube_.vertex_data_size());
    }

    ImGui::End();
}

void Scene::update_view_projection()
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

void Scene::update()
{
    if (animate_) {
        camera_rotation_[0] = std::fmod(camera_rotation_[0] + 1.0F, 360.0F);
        camera_rotation_[1] = std::fmod(camera_rotation_[1] + 1.0F, 360.0F);
    }

    update_view_projection();

    set_uniform_data("light_position", light_position_);
}

void Scene::render()
{
    if (show_cube_) {
        texture_.bind();
        draw_indices(cube_.vertex_count() + light_.vertex_count());
    }
}

void Scene::resize(int width, int height)
{
    auto aspect_ratio = static_cast<float>(width) / static_cast<float>(height);
    auto proj = glm::perspective(glm::quarter_pi<float>(), aspect_ratio, 2.0F, 100.0F);
    set_uniform_data("proj", proj);
}
