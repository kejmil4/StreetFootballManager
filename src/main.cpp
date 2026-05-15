#include "Core/Game.h"
#include <iostream>

int main() {
    try {
        // Instantiate our engine
        Game game;

        // Start the infinite loop
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