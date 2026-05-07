cmake_minimum_required(VERSION 3.24)
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
)

target_link_libraries(StreetFootballManager PRIVATE
        SFML::Graphics
        SFML::Audio
        SFML::Network
)