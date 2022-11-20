#include <iostream>

#include "examples/cube.hpp"
#include "examples/game_of_life.hpp"
#include "examples/quad.hpp"

#include <optional>

int main()
{
    Cube q{};
    q.start();

    return EXIT_SUCCESS;
}
