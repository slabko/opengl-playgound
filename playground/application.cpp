#include <fmt/core.h>
#include <glm/gtc/type_ptr.hpp>
#include <gsl/narrow>
#include <spdlog/spdlog.h>

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"

#include "application.hpp"

namespace playground {

Application::Application(int width, int height) :
  window_size_{width, height}
{
    spdlog::info("initializing");

    auto sdl_init_res = SDL_Init(SDL_INIT_EVERYTHING);
    if (sdl_init_res == -1) {
        throw std::runtime_error("Error initializing SDL");
    }

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

    window_ = SDL_CreateWindow(
      "Graphics Engine",
      SDL_WINDOWPOS_UNDEFINED,
      SDL_WINDOWPOS_UNDEFINED,
      window_size_.x, window_size_.y,
      SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!window_) {
        throw std::runtime_error("Error creating SDL window");
    }

    context_ = SDL_GL_CreateContext(window_);
    if (!context_) {
        throw std::runtime_error("Error creating OpenGL context");
    }

    auto glad_load_res = gladLoadGL();
    if (!glad_load_res) {
        throw std::runtime_error("Error loading OpenGL extension");
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // glClearColor(1.0F, 1.0F, 1.0F, 1.0F);
    glClearColor(0.1F, 0.1F, 0.1F, 1.0F);

    /* ImGui Initialize */
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO const& io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsLight();

    ImGui_ImplSDL2_InitForOpenGL(window_, context_);
    ImGui_ImplOpenGL3_Init("#version 460");

    /** Buffers **/
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ibo_);
    glGenVertexArrays(1, &vao_);
}

Application::~Application()
{
    spdlog::info("shutting down");

    glDeleteBuffers(1, &vbo_);
    glDeleteVertexArrays(1, &vao_);

    for (auto const& attribute_location : created_attributes_) {
        glDisableVertexAttribArray(attribute_location);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(context_);
    SDL_DestroyWindow(window_);
    SDL_Quit();
}

void Application::start()
{
    while (keep_running_) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            ImGui_ImplSDL2_ProcessEvent(&e);

            // Ignore events that are captured by ImGUI
            auto& io = ImGui::GetIO();
            if (io.WantCaptureMouse || io.WantCaptureKeyboard) {
                continue;
            }

            switch (e.type) {
            case SDL_QUIT:
                keep_running_ = false;
                break;
            case SDL_WINDOWEVENT:
                if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    process_window_resize(e.window.data1, e.window.data2);
                }
                break;
            case SDL_MOUSEMOTION:
                mouse_position_ = {e.motion.x, e.motion.y};
                if (e.motion.state == SDL_BUTTON_LMASK) {
                    drag_mouse({e.motion.xrel, e.motion.yrel}, ctrl_is_pressed_ ? Ctrl : None);
                }
                break;
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                if (e.key.keysym.sym == SDLK_LCTRL) {
                    ctrl_is_pressed_ = e.type == SDL_KEYDOWN;
                }
                break;
            case SDL_MOUSEWHEEL:
                scroll_mouse(e.wheel.y);
            }
        }

        present_imgui();

        update();

        glBindVertexArray(vao_);

        render();

        glBindVertexArray(0);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(window_);
    }
}

std::unique_ptr<Program> Application::create_program(std::string const& vertex_shader, std::string const& fragment_shader)
{
    auto p = std::make_unique<Program>(vertex_shader, fragment_shader);
    p->compile();
    return p;
}

void Application::use_program(Program const& p)
{
    current_program_id_ = p.get_id();
    glUseProgram(current_program_id_);
}

void Application::alloc_vbo(size_t size)
{
    glBindVertexArray(vao_);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, gsl::narrow<GLsizei>(size), nullptr, GL_DYNAMIC_DRAW);

    glBindVertexArray(0);
}

void Application::upload_vbo(void const* data, size_t offset, size_t size)
{
    glBindVertexArray(vao_);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferSubData(
      GL_ARRAY_BUFFER,
      gsl::narrow<GLintptr>(offset),
      gsl::narrow<GLintptr>(size),
      data);

    glBindVertexArray(0);
}

void Application::assign_vbo(std::string const& name, int components, size_t stride, size_t offset)
{
    auto attribute_location = glGetAttribLocation(current_program_id_, name.c_str());
    assign_vbo(attribute_location, components, stride, offset);
}

void Application::assign_vbo(GLint attribute_location, int components, size_t stride, size_t offset)
{
    created_attributes_.insert(attribute_location);

    glBindVertexArray(vao_);

    glVertexAttribPointer(
      attribute_location,
      components,
      GL_FLOAT,
      GL_FALSE,
      gsl::narrow<GLsizei>(stride),
      reinterpret_cast<GLvoid const*>(offset)); // NOLINT(performance-no-int-to-ptr)

    glEnableVertexAttribArray(attribute_location);

    glBindVertexArray(0);
}

void Application::alloc_ibo(size_t size)
{
    glBindVertexArray(vao_);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, gsl::narrow<GLsizei>(size), nullptr, GL_DYNAMIC_DRAW);

    glBindVertexArray(0);
}

void Application::upload_ibo(void const* data, size_t offset, size_t size_bytes)
{
    glBindVertexArray(vao_);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_);
    glBufferSubData(
      GL_ELEMENT_ARRAY_BUFFER,
      gsl::narrow<GLintptr>(offset),
      gsl::narrow<GLintptr>(size_bytes),
      data);

    glBindVertexArray(0);
}

void Application::set_uniform_data(std::string const& name, float const& data)
{
    auto id = get_uniform_location(name);
    glUniform1f(id, data);
}

void Application::set_uniform_data(std::string const& name, GLuint const& data)
{
    auto id = get_uniform_location(name);
    glUniform1ui(id, data);
}

void Application::set_uniform_data(std::string const& name, GLint const& data)
{
    auto id = get_uniform_location(name);
    glUniform1i(id, data);
}

void Application::set_uniform_data(std::string const& name, glm::mat4 const& data)
{
    auto id = get_uniform_location(name);
    glUniformMatrix4fv(id, 1, GL_FALSE, glm::value_ptr(data));
}

void Application::set_uniform_data(std::string const& name, glm::vec3 const& data)
{
    auto id = get_uniform_location(name);
    glUniform3fv(id, 1, glm::value_ptr(data));
}

void Application::draw_simple_vertices(size_t vertex_count, DrawType draw_type)
{
    glDrawArrays(draw_type, 0, gsl::narrow<GLsizei>(vertex_count));
}

void Application::draw_indices(size_t vertex_count, DrawType draw_type, size_t offset_count)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_);
    // NOLINTNEXTLINE(performance-no-int-to-ptr): has to be a pointer for glDrawElements
    auto const* offset_bytes_ptr = reinterpret_cast<void*>(offset_count * sizeof(GLuint));
    glDrawElements(draw_type, gsl::narrow<GLsizei>(vertex_count), GL_UNSIGNED_INT, offset_bytes_ptr);
}

void Application::process_window_resize(int width, int height)
{
    window_size_ = {width, height};
    resize(window_size_);
    glViewport(0, 0, width, height);
}

GLint Application::get_uniform_location(std::string const& name)
{
    GLint const id = glGetUniformLocation(current_program_id_, name.c_str());
    if (id < 0) {
        throw std::runtime_error(fmt::format("Uniform name `{}` is not found", name));
    }
    return id;
}

} // namespace playground
