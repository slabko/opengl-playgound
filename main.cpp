#include "examples/game_of_life/game_of_life.hpp"
#include "examples/quad/quad.hpp"
#include "examples/scene/scene.hpp"


int main()
{
    Scene q{};
    q.init();
    q.start();

    return EXIT_SUCCESS;
}
