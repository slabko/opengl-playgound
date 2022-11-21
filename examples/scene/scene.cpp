#include <algorithm>
#include <cmath>
#include <fstream>
#include <numeric>
#include <sstream>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <imgui.h>

#include "../../playground/png.hpp"

#include "vertex.hpp"

#include "scene.hpp"

static std::string read_file(std::string const& path)
{
    std::ifstream in_file{path};
    std::ostringstream buffer;
    buffer << in_file.rdbuf();
    in_file.close();
    return buffer.str();
}

Scene::Scene() :
  playground::Program{
    read_file("GLSL/vertex.glsl"),
    read_file("GLSL/fragment.glsl")},
  shapes_(4),
  light_{shapes_[0]},
  cube1_{shapes_[1]},
  cube2_{shapes_[2]},
  cube3_{shapes_[3]},
  texture_{256, 256, 3}
{
    light_.set_glow(1.0F);
    light_.set_size(0.2F);
    light_.set_position(light_position_);

    cube2_.set_position({1.0, 0.0, 0.0});
    cube3_.set_position({-1.0, 0.0, 0.0});

    size_t vertex_data_size = std::accumulate(shapes_.begin(), shapes_.end(), 0UL, [](auto sum, auto& s) {
        return sum + s.vbo_size();
    });

    size_t index_data_size = std::accumulate(shapes_.begin(), shapes_.end(), 0UL, [](auto sum, auto& s) {
        return sum + s.ibo_size();
    });

    alloc_vbo(vertex_data_size);
    alloc_ibo(index_data_size);

    size_t vertex_data_offset = 0;
    size_t index_data_offset = 0;
    size_t index_offset = 0;
    for (auto& s : shapes_) {
        s.set_vbo_offset(vertex_data_offset);
        upload_vbo(s.vbo_data(), vertex_data_offset, s.vbo_size());
        vertex_data_offset += s.vbo_size();

        s.set_start_index(index_offset);
        index_offset += s.vertex_count();

        s.set_ibo_offset(index_data_offset);
        upload_ibo(s.ibo_data(), index_data_offset, s.ibo_size());
        index_data_offset += s.ibo_size();
    }

    assign_vbo("position", decltype(Vertex::position)::length(), sizeof(Vertex), offsetof(Vertex, position));
    assign_vbo("normal", decltype(Vertex::normal)::length(), sizeof(Vertex), offsetof(Vertex, normal));
    assign_vbo("uv", decltype(Vertex::uv)::length(), sizeof(Vertex), offsetof(Vertex, uv));
    assign_vbo("glow", 1, sizeof(Vertex), offsetof(Vertex, glow));

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
        upload_vbo(light_.vbo_data(), light_.vbo_offset(), light_.vbo_size());
    }

    if (ImGui::SliderFloat("Cube Size", &cube_size_, 0.0F, 2.0F)) {
        cube1_.set_size(cube_size_);
        cube2_.set_size(cube_size_);
        cube3_.set_size(cube_size_);
        upload_vbo(cube1_.vbo_data(), cube1_.vbo_offset(), cube1_.vbo_size());
        upload_vbo(cube2_.vbo_data(), cube2_.vbo_offset(), cube2_.vbo_size());
        upload_vbo(cube3_.vbo_data(), cube3_.vbo_offset(), cube3_.vbo_size());
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

        size_t vertex_count = std::accumulate(shapes_.begin(), shapes_.end(), 0UL, [](auto sum, auto& s) {
            return sum + s.vertex_count();
        });

        draw_indices(vertex_count);
    }
}

void Scene::resize(int width, int height)
{
    auto aspect_ratio = static_cast<float>(width) / static_cast<float>(height);
    auto proj = glm::perspective(glm::quarter_pi<float>(), aspect_ratio, 2.0F, 100.0F);
    set_uniform_data("proj", proj);
}
