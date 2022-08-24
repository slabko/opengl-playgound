#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include <string>
#include <unordered_set>

#include <Eigen/Eigen>
#include <SDL2/SDL.h>
#include <glad/glad.h>

#include "types.hpp"

namespace playground {

class Program {
public:
    Program(std::string const& vertex_shader, std::string const& fragment_shader);

    Program(Program const&) = delete;
    Program(Program&&) = delete;

    Program& operator=(Program const&) = delete;
    Program& operator=(Program&&) = delete;

    virtual ~Program();

    virtual void update() {}
    virtual void render() {}
    virtual void present_imgui() {}

    void start();

protected:
    void set_uniform_data(std::string const& name, Matrix4f const& data);

    void alloc_vbo(size_t size);

    void upload_vbo(MatrixXf const& data, size_t offset);

    void assign_vbo(std::string const& name, int components, size_t stride, size_t offset);

    void alloc_ibo(size_t size);

    void upload_ibo(MatrixXui const& data, size_t offset);

    void draw_simple_triangles(size_t vertex_count);

    void draw_indices(size_t vertex_count);

private:
    bool keep_running_{true};

    std::string const vertex_shader_{};
    std::string const fragment_shader_{};

    int height_{640};
    int width_{1280};

    SDL_Window* window_{};
    SDL_GLContext context_{};

    GLuint shader_program_id_{};
    GLuint vertex_shader_id_{};
    GLuint fragment_shader_id_{};

    uint32_t vao_{};
    uint32_t vbo_{};
    uint32_t ibo_{}; 

    std::unordered_set<std::string> created_attributes_;

    void compile_shader(std::string const& source_code, GLuint shader_id);
    void link_program();
    GLint get_uniform_location(std::string const& name);
};

} // namespace playground

#endif // PROGRAM_HPP
