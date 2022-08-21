#ifndef QUAD_HPP
#define QUAD_HPP

#include <memory>

#include "../playground/program.hpp"
#include "../playground/texture.hpp"

class Quad : public playground::Program {
public:
    Quad();
    void render() override;
private:
    std::unique_ptr<playground::Texture> texture_{};
};

#endif // QUAD_HPP
