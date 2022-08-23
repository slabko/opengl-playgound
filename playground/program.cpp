#include <fmt/core.h>
#include <spdlog/spdlog.h>

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"

#include "program.hpp"

namespace playground {

Program::Program(std::string const& vertex_shader, std::string const& fragment_shader) :
  vertex_shader_{vertex_shader}, fragment_shader_{fragment_shader}
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
      height_, width_,
      SDL_WINDOW_OPENGL);
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

    glClearColor(1.0F, 1.0F, 1.0F, 1.0F);

    /* ImGui Initialize */
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsLight();

    ImGui_ImplSDL2_InitForOpenGL(window_, context_);
    ImGui_ImplOpenGL3_Init("#version 460");

    /** Shaders **/
    shader_program_id_ = glCreateProgram();
    if (!shader_program_id_) {
        throw std::runtime_error("Failed to create a shader program");
    }

    vertex_shader_id_ = glCreateShader(GL_VERTEX_SHADER);
    if (!vertex_shader_id_) {
        throw std::runtime_error("Failed to create vertex shader object");
    }

    fragment_shader_id_ = glCreateShader(GL_FRAGMENT_SHADER);
    if (!fragment_shader_id_) {
        throw std::runtime_error("Failed to create fragment shader object");
    }

    compile_shader(vertex_shader_, vertex_shader_id_);
    compile_shader(fragment_shader, fragment_shader_id_);

    glAttachShader(shader_program_id_, vertex_shader_id_);
    glAttachShader(shader_program_id_, fragment_shader_id_);

    link_program();

    /** Buffers **/
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ibo_);
    glGenVertexArrays(1, &vao_);
}

Program::~Program()
{
    spdlog::info("shutting down");

    glDeleteBuffers(1, &vbo_);
    glDeleteVertexArrays(1, &vao_);

    for (auto const& attribute : created_attributes_) {
        auto attribute_id = glGetAttribLocation(shader_program_id_, attribute.c_str());
        glDisableVertexAttribArray(attribute_id);
    }

    glDetachShader(shader_program_id_, vertex_shader_id_);
    glDetachShader(shader_program_id_, fragment_shader_id_);
    glDeleteShader(vertex_shader_id_);
    glDeleteShader(fragment_shader_id_);
    glDeleteProgram(shader_program_id_);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(context_);
    SDL_DestroyWindow(window_);
    SDL_Quit();
}

void Program::start()
{
    while (keep_running_) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            ImGui_ImplSDL2_ProcessEvent(&e);
            if (e.type == SDL_QUIT) {
                keep_running_ = false;
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

void Program::set_uniform_data(std::string const& name, Matrix4f const& data)
{
    auto id = get_uniform_location(name);
    glUniformMatrix4fv(id, 1, GL_FALSE, data.data());
};

void Program::alloc_vbo(size_t size)
{
    glBindVertexArray(vao_);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizei>(size), nullptr, GL_DYNAMIC_DRAW);

    glBindVertexArray(0);
}

void Program::upload_vbo(MatrixXf const& data, size_t offset)
{
    glBindVertexArray(vao_);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferSubData(
      GL_ARRAY_BUFFER,
      static_cast<GLintptr>(offset),
      static_cast<GLintptr>(sizeof(float) * data.size()),
      data.data());

    glBindVertexArray(0);
};

void Program::assign_vbo(std::string const& name, int components, size_t stride, size_t offset)
{
    auto attribute_id = glGetAttribLocation(shader_program_id_, name.c_str());
    created_attributes_.insert(name);

    glBindVertexArray(vao_);

    glVertexAttribPointer(
      attribute_id,
      components,
      GL_FLOAT,
      GL_FALSE,
      static_cast<GLsizei>(stride),
      reinterpret_cast<GLvoid const*>(offset)); // NOLINT(performance-no-int-to-ptr)

    glEnableVertexAttribArray(attribute_id);

    glBindVertexArray(0);
}

void Program::alloc_ibo(size_t size)
{
    glBindVertexArray(vao_);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizei>(size), nullptr, GL_DYNAMIC_DRAW);

    glBindVertexArray(0);
}

void Program::upload_ibo(MatrixXui const& data, size_t offset)
{
    glBindVertexArray(vao_);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_);
    glBufferSubData(
      GL_ELEMENT_ARRAY_BUFFER,
      static_cast<GLintptr>(offset),
      static_cast<GLintptr>(sizeof(GLuint) * data.size()),
      data.data());

    glBindVertexArray(0);
}

void Program::draw_simple_triangles(size_t vertex_count) 
{
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertex_count));
}


void Program::draw_indices(size_t vertex_count)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(vertex_count), GL_UNSIGNED_INT, nullptr);
}


void Program::compile_shader(std::string const& source_code, GLuint shader_id)
{
    auto const* raw_source_code = reinterpret_cast<GLchar const*>(source_code.c_str());
    glShaderSource(shader_id, 1, &raw_source_code, nullptr);
    glCompileShader(shader_id);

    GLint error_code{};
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &error_code);

    if (error_code != GL_TRUE) {
        GLsizei buffer_size{};
        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &buffer_size);
        std::vector<GLchar> buffer(buffer_size);
        glGetShaderInfoLog(shader_id, buffer_size, nullptr, buffer.data());
        throw std::runtime_error(fmt::format("Failed to compile shader: {}", buffer.data()));
    }
}

void Program::link_program()
{
    glLinkProgram(shader_program_id_);
    glUseProgram(shader_program_id_);

    GLint error_code{};
    glGetProgramiv(shader_program_id_, GL_LINK_STATUS, &error_code);

    if (error_code != GL_TRUE) {
        GLsizei buffer_size{};
        glGetProgramiv(shader_program_id_, GL_INFO_LOG_LENGTH, &buffer_size);
        std::vector<GLchar> buffer(buffer_size);
        glGetProgramInfoLog(shader_program_id_, buffer_size, nullptr, buffer.data());
        throw std::runtime_error(fmt::format("Failed to link shader: {}", buffer.data()));
    }
}

GLint Program::get_uniform_location(std::string const& name)
{
    GLint id = glGetUniformLocation(shader_program_id_, name.c_str());
    if (id < 0) {
        throw std::runtime_error(fmt::format("Uniform name `{}` is not found", name));
    }
    return id;
}
} // namespace playground
