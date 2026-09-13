#include "core/game.hpp"

int main(int argc, char* argv[]) {
    Game& game = Game::GetInstance();
    game.init("Ghost Escape", 1280, 720);
    game.run();
    game.clean();
    return 0;
}