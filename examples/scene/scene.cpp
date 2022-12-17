#include <cmath>
#include <fstream>
#include <numeric>
#include <sstream>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <gsl/narrow>
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
  shapes_{&light_, &sphere1_, &sphere2_, &cube1_}
{
    light_.set_glow(1.0F);
    light_.set_size(0.2F);
    light_.set_position(light_position_);
    light_.update();

    sphere1_.set_size(0.5);
    sphere1_.update();

    cube1_.set_width(10.0F);
    cube1_.set_depth(10.0F);
    cube1_.set_height(0.5F);
    cube1_.set_position({0.0F, -1.0F, 0.0F});
    cube1_.update();

    sphere2_.set_size(0.5);
    sphere2_.set_position({1.0, 0.0, 0.0});
    sphere2_.update();

    size_t vertex_count = std::accumulate(shapes_.begin(), shapes_.end(), 0UL, [](auto sum, auto& s) {
        return sum + s->vertex_count();
    });

    /////// VBO ////////
    size_t vertex_data_size = vertex_count * sizeof(Vertex);

    alloc_vbo(vertex_data_size);

    size_t vbo_data_offset = 0;
    for (auto& s : shapes_) {
        auto const vbo_chunk_size = s->vertex_count() * sizeof(Vertex);
        upload_vbo(s->vbo_data(), vbo_data_offset, vbo_chunk_size);
        s->set_vbo_offset_bytes(vbo_data_offset);
        vbo_data_offset += vbo_chunk_size;
    }

    assign_vbo("position", decltype(Vertex::position)::length(), sizeof(Vertex), offsetof(Vertex, position));
    assign_vbo("normal", decltype(Vertex::normal)::length(), sizeof(Vertex), offsetof(Vertex, normal));
    assign_vbo("glow", 1, sizeof(Vertex), offsetof(Vertex, glow));

    //////// IBO ////////
    assert(vertex_count % 3 == 0); // We expect to see 3 vertices on each polygon
    size_t index_count = vertex_count / 3;
    size_t index_data_size = index_count * sizeof(glm::uvec3);

    indices_.resize(index_count);
    std::generate(indices_.begin(), indices_.end(), [i = 0U]() mutable {
        return glm::uvec3{0, 1, 2} + glm::uvec3{3, 3, 3} * i++;
    });
    alloc_ibo(index_data_size);
    upload_ibo(indices_.data(), 0, index_data_size);

    /////// Uniform Data ////////
    auto model = glm::mat4(1.0F);
    auto view = glm::translate(glm::mat4(1.0F), {0.0F, 0.0F, -5.0F});
    auto proj = glm::perspective(glm::quarter_pi<float>(), 1920.0F / 1080.0F, 2.0F, 100.0F);

    set_uniform_data("model", model);
    set_uniform_data("view", view);
    set_uniform_data("proj", proj);

    set_uniform_data("light_position", light_position_);
    set_uniform_data("camera_position", glm::vec3{0.0F, 0.0F, 5.0F});
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
        light_.update();
        upload_vbo(light_.vbo_data(), light_.vbo_offset_bytes(), light_.vertex_count() * sizeof(Vertex));
    }

    if (ImGui::SliderFloat("Cuboid Size", &cube_size_, 0.0F, 2.0F)) {
        sphere1_.set_size(cube_size_ * 0.5F);
        sphere1_.update();

        sphere2_.set_size(cube_size_ * 0.5F);
        sphere2_.set_position({cube_size_, 0.0F, 0.0F});
        sphere2_.update();

        upload_vbo(sphere1_.vbo_data(), sphere1_.vbo_offset_bytes(), sphere1_.vertex_count() * sizeof(Vertex));
        upload_vbo(sphere2_.vbo_data(), sphere2_.vbo_offset_bytes(), sphere2_.vertex_count() * sizeof(Vertex));
        upload_vbo(cube1_.vbo_data(), cube1_.vbo_offset_bytes(), cube1_.vertex_count() * sizeof(Vertex));
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
        draw_indices(indices_.size() * glm::uvec3::length());
    }
}

void Scene::resize(int width, int height)
{
    auto aspect_ratio = static_cast<float>(width) / static_cast<float>(height);
    auto proj = glm::perspective(glm::quarter_pi<float>(), aspect_ratio, 2.0F, 100.0F);
    set_uniform_data("proj", proj);
}
