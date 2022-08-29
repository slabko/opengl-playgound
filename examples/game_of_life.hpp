#ifndef PLAYGROUND_GAME_OF_LIFE_HPP
#define PLAYGROUND_GAME_OF_LIFE_HPP

#include <Eigen/Eigen>
#include <memory>

#include "../playground/program.hpp"
#include "../playground/texture.hpp"

static int const BOARD_SIZE = 1000;

class GameOfLife : public playground::Program {
public:
    GameOfLife();

protected:
    void render() override;
    void update() override;
    void present_imgui() override;

private:
    playground::Texture texture_{};

    Eigen::Matrix<uint8_t, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> generation_;
    Eigen::Matrix<uint8_t, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> neighbors_;

    bool continue_{true};

    void update_generation();
    void reset();
};

#endif // PLAYGROUND_GAME_OF_LIFE_HPP
