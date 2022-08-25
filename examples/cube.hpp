#ifndef CUBE_HPP
#define CUBE_HPP

#include <memory>

#include "../playground/program.hpp"
#include "../playground/texture.hpp"

class Cube : public playground::Program {
public:
    Cube();
    void render() override;
    void update() override;
    void present_imgui() override;
private:
    std::unique_ptr<playground::Texture> texture_{};

    bool show_cube_{true};
    bool animate_{false};
    Eigen::Vector2f rotation_{0.0F, 0.0F};
};

#endif // CUBE_HPP
