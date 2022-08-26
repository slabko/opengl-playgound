#include <execution>
#include <iostream>
#include <memory>
#include <vector>

#include <Eigen/Eigen>

#include "examples/cube.hpp"
#include "examples/quad.hpp"
#include "examples/game_of_life.hpp"

int main()
{
    GameOfLife p{};
    p.start();

    return EXIT_SUCCESS;
}
