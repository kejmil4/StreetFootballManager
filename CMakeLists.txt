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
        src/AI/AIBrain.cpp
        src/AI/AIBrain.h
        src/Visuals/FootballerAnimator.cpp
        src/Visuals/FootballerAnimator.h
        src/Controllers/HumanController.cpp
        src/Controllers/HumanController.h
        src/Managers/Referee.cpp
        src/Managers/Referee.h
        src/Managers/TeamManager.cpp
        src/Managers/TeamManager.h
        src/States/gameOverState.cpp
        src/States/gameOverState.h
        src/Managers/EnvironmentManager.cpp
        src/Managers/EnvironmentManager.h
        src/UI/PauseMenu.cpp
        src/UI/PauseMenu.h
        src/Career/CareerData.h
        src/States/CareerStates/CareerMenuState.cpp
        src/States/CareerStates/CareerMenuState.h
        src/States/CareerStates/CareerHubState.cpp
        src/States/CareerStates/CareerHubState.h
        src/States/CareerStates/PostMatchState.cpp
        src/States/CareerStates/PostMatchState.h
        src/States/CareerStates/ShopState.cpp
        src/States/CareerStates/ShopState.h
        src/States/CareerStates/ClubState.cpp
        src/States/CareerStates/ClubState.h
        src/Career/LeagueSimulator.cpp
        src/Career/LeagueSimulator.h
        src/States/CareerStates/StandingsState.cpp
        src/States/CareerStates/StandingsState.h
        src/States/CareerStates/TopScorersState.cpp
        src/States/CareerStates/TopScorersState.h
        src/States/CareerStates/CareerSetupState.cpp
        src/States/CareerStates/CareerSetupState.h
        src/States/CareerStates/CareerOverState.cpp
        src/States/CareerStates/CareerOverState.h
        src/Career/CareerData.cpp
        src/States/SettingsState.cpp
        src/States/SettingsState.h
        src/Managers/AudioManager.cpp
        src/Managers/AudioManager.h
        src/States/AudioSettingsState.cpp
        src/States/AudioSettingsState.h
        src/States/CareerStates/LoadCareerState.cpp
        src/States/CareerStates/LoadCareerState.h
)

target_link_libraries(StreetFootballManager PRIVATE
        SFML::Graphics
        SFML::Audio
        SFML::Network
)