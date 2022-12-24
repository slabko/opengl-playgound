#ifndef PLAYGROUND_PROGRAM_HPP
#define PLAYGROUND_PROGRAM_HPP

#include <string>
#include <unordered_set>

#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <glm/mat4x4.hpp>

namespace playground {

class Program {
public:
    enum DrawType {
        Points = GL_POINTS,
        Triangles = GL_TRIANGLES,
        Lines = GL_LINES
    };

    enum KeyModifiers {
        None = 0,
        Ctrl = 1 << 0,
    };

    Program(std::string const& vertex_shader, std::string const& fragment_shader, int width = 1920, int height = 1080);

    Program(Program const&) = delete;
    Program(Program&&) = delete;

    Program& operator=(Program const&) = delete;
    Program& operator=(Program&&) = delete;

    virtual ~Program();

    void start();

protected:
    virtual void update() {}

    virtual void render() {}

    virtual void present_imgui() {}

    virtual void resize(glm::ivec2 /*new_size*/) {}

    virtual void drag_mouse(glm::ivec2 /*offset*/, KeyModifiers /*modifiers*/){};

    virtual void scroll_mouse(int /*val*/){};

    glm::ivec2 window_size() { return window_size_; }

    glm::ivec2 mouse_position() { return mouse_position_; };

    void set_uniform_data(std::string const& name, float const& data);

    void set_uniform_data(std::string const& name, glm::mat4 const& data);

    void set_uniform_data(std::string const& name, glm::vec3 const& data);

    void alloc_vbo(size_t size);

    void upload_vbo(void const* data, size_t offset, size_t size);

    void assign_vbo(std::string const& name, int components, size_t stride, size_t offset);

    void alloc_ibo(size_t size);

    void upload_ibo(void const* data, size_t offset, size_t size_bytes);

    void draw_simple_vertices(size_t vertex_count, DrawType draw_type = Triangles);

    void draw_indices(size_t vertex_count, DrawType draw_type = Triangles, size_t offset_count = 0);

private:
    bool keep_running_{true};

    std::string vertex_shader_{};
    std::string fragment_shader_{};

    glm::ivec2 window_size_{};

    glm::ivec2 mouse_position_{};

    bool ctrl_is_pressed_{false};

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

    void process_window_resize(int width, int height);
};

} // namespace playground

#endif // PLAYGROUND_PROGRAM_HPP
