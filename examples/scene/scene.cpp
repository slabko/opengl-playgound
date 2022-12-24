#include <cmath>
#include <fstream>
#include <numeric>
#include <sstream>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <imgui.h>

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
  shapes_{&light_, &sphere1_, &sphere2_, &cube1_, &bunny_} {}

void Scene::init()
{
    bunny_prototype_ = std::make_unique<StaticShape const>(load_model<StaticShape>("resources/bunny.obj"));
    bunny_ = *bunny_prototype_;
    bunny_.update();

    light_.set_size(0.2F);
    light_.set_position(light_position_);
    light_.update();

    sphere1_.set_size(0.01F);
    sphere1_.set_position({0.0, 0.0, 0.0});
    sphere1_.update();

    cube1_.set_width(10.0F);
    cube1_.set_depth(10.0F);
    cube1_.set_height(0.5F);
    cube1_.set_position({0.0F, -0.25F, 0.0F});
    cube1_.update();

    sphere2_.set_size(0.5);
    sphere2_.set_position({2.0, 0.0, 0.0});
    sphere2_.update();

    size_t const vertex_count = std::accumulate(shapes_.begin(), shapes_.end(), 0UL, [](auto sum, auto& s) {
        return sum + s->vertex_count();
    });

    /////// VBO ////////
    size_t const vertex_data_size = vertex_count * sizeof(Vertex);

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

    //////// IBO ////////
    assert(vertex_count % 3 == 0); // We expect to see 3 vertices on each polygon
    size_t const index_count = vertex_count;
    size_t const index_data_size = index_count * sizeof(uint32_t);

    indices_.resize(index_count);
    std::iota(indices_.begin(), indices_.end(), 0);
    alloc_ibo(index_data_size);
    upload_ibo(indices_.data(), 0, index_data_size);

    set_uniform_data("glow", 0.0F);
}

void Scene::present_imgui()
{
    ImGui::Begin("Configuration");
    ImGui::Text("Application average %.1f FPS", ImGui::GetIO().Framerate);

    auto mouse = mouse_position();
    ImGui::Text("Mouse position: %d  %d", mouse.x, mouse.y);
    ImGui::Text("Camera position: %f  %f  %f", camera_position_.x, camera_position_.y, camera_position_.z);
    ImGui::Text("Camera rotation: %f  %f", camera_rotation_.x, camera_rotation_.y);
    ImGui::Text("World rotation: %f  %f", world_rotation_.x, world_rotation_.y);

    ImGui::SliderFloat("Zoom", &camera_zoom_, glm::half_pi<float>(), glm::quarter_pi<float>());
    ImGui::SliderFloat("Lens shift", &lens_shift_, -1.0F, 1.0F);

    if (ImGui::SliderFloat3("Light Position", glm::value_ptr(light_position_), -5, 5)) {
        light_.set_position(light_position_);
        light_.update();
        upload_vbo(light_.vbo_data(), light_.vbo_offset_bytes(), light_.vertex_count() * sizeof(Vertex));
    }

    if (ImGui::SliderFloat("Scale", &cube_size_, 0.0F, 2.0F)) {
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

void Scene::update()
{
    auto model = glm::mat4(1.0F);
    set_uniform_data("model", model);

    auto view = glm::mat4(1.0F);
    view = glm::rotate(view, glm::radians(camera_rotation_.x), {1.0F, 0.0F, 0.0F});
    view = glm::rotate(view, glm::radians(camera_rotation_.y), {0.0F, 1.0F, 0.0F});
    view = glm::translate(view, {-camera_position_.x, -camera_position_.y, -camera_position_.z});
    view = glm::rotate(view, glm::radians(world_rotation_.x), {1.0F, 0.0F, 0.0F});
    view = glm::rotate(view, glm::radians(world_rotation_.y), {0.0F, 1.0F, 0.0F});
    set_uniform_data("view", view);

    static float const znear = 1.00F;
    static float const zfar = 100.0F;
    auto size = static_cast<glm::vec2>(window_size());
    auto const aspect = size.x / size.y;
    auto height = glm::tan(0.5F * camera_zoom_) * znear;
    auto width = height * aspect;
    auto proj = glm::frustum(-width, width, -height + lens_shift_, height + lens_shift_, znear, zfar);
    set_uniform_data("proj", proj);

    // Undo camera transformation
    auto camera_position = glm::inverse(view) * glm::vec4{0.0F, 0.0F, 0.0F, 1.0F};
    set_uniform_data("camera_position", glm::vec3(camera_position));

    set_uniform_data("light_position", light_position_);
}

void Scene::render()
{
    // Light
    set_uniform_data("glow", 1.0F);
    draw_indices(light_.vertex_count());

    // Objects
    set_uniform_data("glow", 0.0F);
    draw_indices(indices_.size(), Triangles, light_.vertex_count());
}

void Scene::drag_mouse(glm::ivec2 offset, KeyModifiers modifiers)
{
    // Dragging the mouse along x causes rotation about y and vice versa

    float const x_rotation = static_cast<float>(offset.y) / 5.0F;
    float const y_rotation = static_cast<float>(offset.x) / 5.0F;

    switch (modifiers) {
    case None:
        world_rotation_.y += y_rotation;
        world_rotation_.x += x_rotation;
        break;
    case Ctrl:
        camera_rotation_.y += y_rotation;
        camera_rotation_.x += x_rotation;
    }
}

void Scene::scroll_mouse(int val)
{
    camera_position_.z += static_cast<float>(val) * 0.1F;
}