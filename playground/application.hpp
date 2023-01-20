#ifndef PLAYGROUND_APPLICATION_HPP
#define PLAYGROUND_APPLICATION_HPP

#include <string>
#include <unordered_set>
#include <memory>

#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <glm/mat4x4.hpp>

#include "program.hpp"

namespace playground {

class Application {
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

    Application(int width = 1920, int height = 1080);

    Application(Application const&) = delete;
    Application(Application&&) = delete;

    Application& operator=(Application const&) = delete;
    Application& operator=(Application&&) = delete;

    virtual ~Application();

    void start();

    std::unique_ptr<Program> create_program(std::string const& vertex_shader, std::string const& fragment_shader);

    void use_program(Program const& p);

protected:
    virtual void update() {}

    virtual void render() {}

    virtual void present_imgui() {}

    virtual void resize(glm::ivec2 /*new_size*/) {}

    virtual void drag_mouse(glm::ivec2 /*offset*/, KeyModifiers /*modifiers*/){};

    virtual void scroll_mouse(int /*val*/){};

    glm::ivec2 window_size() { return window_size_; }

    glm::ivec2 mouse_position() { return mouse_position_; };

    void alloc_vbo(size_t size);

    void upload_vbo(void const* data, size_t offset, size_t size);

    void assign_vbo(GLint attribute_location, int components, size_t stride, size_t offset);

    /*
     * **Note:** OpenGL uses attribute location (GLint) to assign VBOs,
     * `name` is only used to retrieve that attribute location from a current program.
     * If we intended to use the same buffer between two programs, we need to make sure that
     * locations match in all programs.
     *
     * For example, if I want to use the same VBO with attribute `positions`, I declare it as
     * ```
     * layout (location = 0) in vec3 position;
     * ```
     * we then use this program and call `assign_vbo`, now this vbo will be associated
     * with attribute location 0, we can now use the same VBO in other programs
     * by binding it to location 0:
     * ```
     * layout (location = 0) in vec3 position; // can be called differently
     * ```
     * The attribute name can be different in other programs, but location should be the same.
     * However, it makes sense to use the same name for consistency, plus this approach allows us
     * to use any program to upload VBO
     */
    void assign_vbo(std::string const& name, int components, size_t stride, size_t offset);

    void alloc_ibo(size_t size);

    void upload_ibo(void const* data, size_t offset, size_t size_bytes);

    [[maybe_unused]] void set_uniform_data(std::string const& name, float const& data);

    [[maybe_unused]] void set_uniform_data(std::string const& name, GLuint const& data);

    [[maybe_unused]] void set_uniform_data(std::string const& name, GLint const& data);

    [[maybe_unused]] void set_uniform_data(std::string const& name, glm::mat4 const& data);

    [[maybe_unused]] void set_uniform_data(std::string const& name, glm::vec3 const& data);

    [[maybe_unused]] void draw_simple_vertices(size_t vertex_count, DrawType draw_type = Triangles);

    [[maybe_unused]] void draw_indices(size_t vertex_count, DrawType draw_type = Triangles, size_t offset_count = 0);

private:
    bool keep_running_{true};

    glm::ivec2 window_size_{};

    glm::ivec2 mouse_position_{};

    bool ctrl_is_pressed_{false};

    SDL_Window* window_{};
    SDL_GLContext context_{};

    uint32_t vao_{};
    uint32_t vbo_{};
    uint32_t ibo_{};

    std::unordered_set<GLint> created_attributes_;

    GLuint current_program_id_{};

    void process_window_resize(int width, int height);

    GLint get_uniform_location(std::string const& name);
};

} // namespace playground

#endif // PLAYGROUND_APPLICATION_HPP
