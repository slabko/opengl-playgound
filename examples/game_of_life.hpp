#ifndef PLAYGROUND_GAME_OF_LIFE_HPP
#define PLAYGROUND_GAME_OF_LIFE_HPP

#include <memory>
#include <Eigen/Eigen>

#include "../playground/program.hpp"
#include "../playground/texture.hpp"

static const int BOARD_SIZE = 1024;

class GameOfLife : public playground::Program {
public:
    GameOfLife();
protected:
    void render() override;
    void update() override;
    void present_imgui() override;
private:
    std::unique_ptr<playground::Texture> texture_{};

    Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> generation_;
    Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> neighbors_;

    uint32_t size_{BOARD_SIZE};

    bool continue_{true};

    void update_generation();
};

#endif // PLAYGROUND_GAME_OF_LIFE_HPP
