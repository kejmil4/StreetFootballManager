#pragma once

namespace Config {
    constexpr unsigned int WINDOW_WIDTH = 1920;
    constexpr unsigned int WINDOW_HEIGHT = 1080;

    constexpr float CENTER_X = WINDOW_WIDTH / 2.0f;
    constexpr float CENTER_Y = WINDOW_HEIGHT / 2.0f;

    constexpr float GOAL_TOP_Y = CENTER_Y - 150.f;
    constexpr float GOAL_BOTTOM_Y = CENTER_Y + 150.f;
}