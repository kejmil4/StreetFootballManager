#include "Core/Game.h"
#include <iostream>
#include <ctime>
#include <cstdlib>

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    try {
        Game game;

        game.run();
    }
    catch (const std::exception& e) {
        // If anything completely crashes (like missing SFML libraries),
        // it will catch it here and print the error to your console.
        std::cerr << "FATAL ERROR: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}