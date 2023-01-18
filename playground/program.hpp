#ifndef PLAYGROUND_PROGRAM_HPP
#define PLAYGROUND_PROGRAM_HPP

#include <string>
#include <unordered_set>

#include <glad/glad.h>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

namespace playground {

class Program {

public:
    Program(std::string const& vertex_shader, std::string const& fragment_shader);
    Program(Program const&) = delete;
    Program(Program&&) = default;

    Program& operator=(Program const&) = delete;
    Program& operator=(Program&&) = default;

    ~Program();

    void compile();

    [[nodiscard]] GLuint get_id() const;

private:
    void compile_shader(std::string const& source_code, GLuint shader_id);
    void link_program();


    std::string vertex_shader_{};
    std::string fragment_shader_{};

    bool is_compiled_{false};

    GLuint program_id_{};
    GLuint vertex_shader_id_{};
    GLuint fragment_shader_id_{};

};

} // namespace playground

#endif // PLAYGROUND_PROGRAM_HPP
