#ifndef QUAD_HPP
#define QUAD_HPP

#include <memory>

#include "../playground/program.hpp"
#include "../playground/texture.hpp"

class Quad : public playground::Program {
public:
    Quad();
    void update() override;
    void render() override;
    void present_imgui() override;
private:
    std::unique_ptr<playground::Texture> texture_{};
    float scale_{0.75};
};

#endif // QUAD_HPP
