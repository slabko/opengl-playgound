#include <stdexcept>
#include <vector>

#include <fmt/core.h>
#include <glm/gtc/type_ptr.hpp>
#include <gsl/narrow>

#include "program.hpp"

namespace playground {

Program::Program(std::string const& vertex_shader, std::string const& fragment_shader) :
  vertex_shader_{vertex_shader}, fragment_shader_{fragment_shader}
{
}

void Program::compile()
{
    program_id_ = glCreateProgram();
    if (!program_id_) {
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
    compile_shader(fragment_shader_, fragment_shader_id_);

    glAttachShader(program_id_, vertex_shader_id_);
    glAttachShader(program_id_, fragment_shader_id_);

    link_program();

    is_compiled_ = true;
}

GLuint Program::get_id() const
{
    if (!is_compiled_) {
        throw std::runtime_error("get_id() cannot be used before the program is compiled");
    }
    return program_id_;
}

Program::~Program()
{
    glDetachShader(program_id_, vertex_shader_id_);
    glDetachShader(program_id_, fragment_shader_id_);
    glDeleteShader(vertex_shader_id_);
    glDeleteShader(fragment_shader_id_);
    glDeleteProgram(program_id_);
}

void Program::compile_shader(std::string const& source_code, GLuint shader_id)
{
    auto const* raw_source_code = source_code.c_str();
    glShaderSource(shader_id, 1, &raw_source_code, nullptr);
    glCompileShader(shader_id);

    GLint error_code{};
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &error_code);

    if (error_code != GL_TRUE) {
        GLint raw_buffer_size{};
        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &raw_buffer_size);
        auto buffer_size = gsl::narrow<size_t>(raw_buffer_size);
        std::vector<GLchar> buffer(buffer_size);
        glGetShaderInfoLog(shader_id, raw_buffer_size, nullptr, buffer.data());
        throw std::runtime_error(fmt::format("Failed to compile shader: {}", buffer.data()));
    }
}

void Program::link_program()
{
    glLinkProgram(program_id_);

    GLint error_code{};
    glGetProgramiv(program_id_, GL_LINK_STATUS, &error_code);

    if (error_code != GL_TRUE) {
        GLsizei raw_buffer_size{};
        glGetProgramiv(program_id_, GL_INFO_LOG_LENGTH, &raw_buffer_size);
        auto buffer_size = gsl::narrow<size_t>(raw_buffer_size);
        std::vector<GLchar> buffer(buffer_size);
        glGetProgramInfoLog(program_id_, raw_buffer_size, nullptr, buffer.data());
        throw std::runtime_error(fmt::format("Failed to link shader: {}", buffer.data()));
    }
}

} // namespace playground
