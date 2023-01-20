#include <cmath>
#include <fstream>
#include <numeric>
#include <sstream>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
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
  shapes_{&floor_, &sphere1_, &sphere2_, &cube_, &bunny_, &light_} {}

void Scene::init()
{
    program_ = create_program(
      read_file("GLSL/vertex.glsl"),
      read_file("GLSL/fragment.glsl"));

    light_program_ = create_program(
      read_file("GLSL/light_vertex.glsl"),
      read_file("GLSL/light_fragment.glsl"));

    bunny_prototype_ = std::make_unique<StaticShape const>(load_model<StaticShape>("resources/bunny.obj"));
    bunny_ = *bunny_prototype_;
    bunny_.update();

    light_.set_size(0.1F);
    light_.update();

    sphere1_.set_size(0.5F);
    sphere1_.set_position({-2.0, 0.5, 0.0});
    sphere1_.update();

    cube_.set_dimensions(1.0F, 1.0F, 1.0F);
    cube_.set_position({0.0F, 0.5F, -1.5F});
    cube_.update();

    floor_.set_dimensions(10.0F, 0.5F, 10.0F);
    floor_.set_position({0.0F, -0.25F, 0.0F});
    floor_.update();

    sphere2_.set_size(0.5);
    sphere2_.set_position({2.0, 0.5, 0.0});
    sphere2_.update();

    size_t const vertex_count = std::accumulate(shapes_.begin(), shapes_.end(), 0UL, [](auto sum, auto& s) {
        return sum + s->vertex_count();
    });

    /////// VBO ////////
    use_program(*program_);
    size_t const vertex_data_size = vertex_count * sizeof(Vertex);
    alloc_vbo(vertex_data_size);
    size_t vbo_offset = 0;
    for (auto& s : shapes_) {
        auto const vbo_chunk_size = s->vertex_count();
        upload_vbo(s->vbo_data(), vbo_offset * sizeof(Vertex), vbo_chunk_size * sizeof(Vertex));
        s->set_vbo_offset(vbo_offset);
        vbo_offset += vbo_chunk_size;
    }
    assign_vbo("position", decltype(Vertex::position)::length(), sizeof(Vertex), offsetof(Vertex, position));
    assign_vbo("normal", decltype(Vertex::normal)::length(), sizeof(Vertex), offsetof(Vertex, normal));
    assign_vbo("uv", decltype(Vertex::uv)::length(), sizeof(Vertex), offsetof(Vertex, uv));

    //////// IBO ////////
    // We use universal index buffer where each polygon is represented by three sequential vertices in the vertex buffer
    // so index buffer is just a sequence of integers for 0 to total count of vertices.
    // We could use glDrawArrays, but for demonstration and future flexibility
    // I decided to introduce the index buffer to the model
    assert(vertex_count % 3 == 0);
    size_t const index_count = vertex_count;
    size_t const index_data_size = index_count * sizeof(uint32_t);
    indices_.resize(index_count);
    std::iota(indices_.begin(), indices_.end(), 0);
    alloc_ibo(index_data_size);
    upload_ibo(indices_.data(), 0, index_data_size);

    png::RgbPixel const pixel_diffuse{255, 255, 255};
    white_pixel_diffuse_.upload(&pixel_diffuse, 0, 0, 1, 1);
    png::RedPixel const pixel_specular{255};
    white_pixel_specular_.upload(&pixel_specular, 0, 0, 1, 1);

    auto const cube_texture_image = png::read_png<png::RgbPixel>("textures/crate.png");
    cube_diffuse_.upload(cube_texture_image.pixels, 0, 0, cube_texture_image.width, cube_texture_image.height);
    auto const cube_specular_texture_image = png::read_png<png::RedPixel>("textures/crate_specular.png");
    cube_specular_.upload(cube_specular_texture_image.pixels, 0, 0, cube_specular_texture_image.width, cube_specular_texture_image.height);
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

    ImGui::SliderFloat3("Light Position", glm::value_ptr(light_position_), -5, 5);

    if (ImGui::SliderFloat("Scale", &scale_, 0.0F, 2.0F)) {
        sphere1_.set_size(scale_ * 0.5F);
        sphere1_.update();

        sphere2_.set_size(scale_ * 0.5F);
        sphere2_.update();

        bunny_.set_scale(scale_);
        bunny_.update();

        upload_vbo(sphere1_.vbo_data(), sphere1_.vbo_offset() * sizeof(Vertex), sphere1_.vertex_count() * sizeof(Vertex));
        upload_vbo(sphere2_.vbo_data(), sphere2_.vbo_offset() * sizeof(Vertex), sphere2_.vertex_count() * sizeof(Vertex));
        upload_vbo(bunny_.vbo_data(), bunny_.vbo_offset() * sizeof(Vertex), bunny_.vertex_count() * sizeof(Vertex));
    }

    ImGui::End();
}

void Scene::update()
{
}

void Scene::render()
{
    auto view = view_matrix();
    auto proj = proj_matrix();

    // find the camera position from the view matrix
    auto camera_position = glm::inverse(view) * glm::vec4{0.0F, 0.0F, 0.0F, 1.0F};

    // Objects
    use_program(*program_);
    set_uniform_data("view", view);
    set_uniform_data("proj", proj);
    set_uniform_data("model", glm::mat4(1.0F));
    set_uniform_data("light_position", light_position_);
    set_uniform_data("camera_position", glm::vec3(camera_position));

    set_uniform_data("diffuse_texture", 0);
    set_uniform_data("specular_texture", 1);

    white_pixel_diffuse_.bind();
    white_pixel_specular_.bind();

    set_material(materials::WhiteRubber);
    draw_indices(cube_.vbo_offset(), Triangles);

    set_material(materials::Wood);
    cube_diffuse_.bind();
    cube_specular_.bind();
    draw_indices(cube_.vertex_count(), Triangles, cube_.vbo_offset());
    white_pixel_diffuse_.bind();
    white_pixel_specular_.bind();

    set_material(materials::Gold);
    draw_indices(bunny_.vertex_count(), Triangles, bunny_.vbo_offset());

    // Light
    use_program(*light_program_);
    set_uniform_data("view", view);
    set_uniform_data("proj", proj);
    set_uniform_data("model", glm::translate(glm::mat4(1.0F), light_position_));
    draw_indices(light_.vertex_count(), Triangles, light_.vbo_offset());
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

glm::mat4 Scene::view_matrix()
{
    auto view = glm::mat4(1.0F);

    // 3. rotate the camera
    view = glm::rotate(view, glm::radians(camera_rotation_.x), {1.0F, 0.0F, 0.0F});
    view = glm::rotate(view, glm::radians(camera_rotation_.y), {0.0F, 1.0F, 0.0F});

    // 2. move the camera
    view = glm::translate(view, {-camera_position_.x, -camera_position_.y, -camera_position_.z});

    // 1. rotate the world around the center
    view = glm::rotate(view, glm::radians(world_rotation_.x), {1.0F, 0.0F, 0.0F});
    view = glm::rotate(view, glm::radians(world_rotation_.y), {0.0F, 1.0F, 0.0F});

    return view;
}

glm::mat4 Scene::proj_matrix()
{
    static float const znear = 1.00F;
    static float const zfar = 100.0F;
    auto size = static_cast<glm::vec2>(window_size());
    auto const aspect = size.x / size.y;
    auto height = glm::tan(0.5F * camera_zoom_) * znear;
    auto width = height * aspect;
    auto proj = glm::frustum(-width, width, -height + lens_shift_, height + lens_shift_, znear, zfar);

    return proj;
}

void Scene::set_material(materials::Material const& material)
{
    set_uniform_data("material.ambient", material.ambient);
    set_uniform_data("material.diffuse", material.diffuse);
    set_uniform_data("material.specular", material.specular);
    set_uniform_data("material.shininess", material.shininess);
}
