cmake_minimum_required(VERSION 3.24)

set(CMAKE_POLICY_VERSION_MINIMUM "3.5" CACHE STRING "" FORCE)

project(StreetFootballManager)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.0.0
)
FetchContent_MakeAvailable(SFML)

include_directories(src)

add_executable(StreetFootballManager
        src/main.cpp
        src/Core/Game.h
        src/Core/Game.cpp
        src/Entities/GameObject.cpp
        src/Entities/GameObject.h
        src/States/GameState.h
        src/Entities/Entity.cpp
        src/Entities/Entity.h
        src/States/MatchState.cpp
        src/States/MatchState.h
        src/Entities/Ball.cpp
        src/Entities/Ball.h
        src/Entities/Pitch.cpp
        src/Entities/Pitch.h
        src/Core/Config.h
        src/UI/HUD.cpp
        src/UI/HUD.h
        src/Entities/Footballer.cpp
        src/Entities/Footballer.h
        src/States/MenuState.cpp
        src/States/MenuState.h
)

target_link_libraries(StreetFootballManager PRIVATE
        SFML::Graphics
        SFML::Audio
        SFML::Network
)