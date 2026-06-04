# ⚽ Retro Street Football Manager

A highly customizable, retro-styled street football game featuring both an active physics-based Match Engine and a deep, multi-week Career Simulation. Built completely from scratch in C++ using the SFML graphics library.

## 🚀 How to Play (No Installation Required)

You don't need to install any compilers or IDEs to play the game! Just follow these steps:

1. Go to the [Releases](https://github.com/kejmil4/StreetFootballManager/releases/tag/v1.0.0) tab on the right side of this page.
2. Download `StreetFootballManager_v1.0_Windows.zip`.
3. Extract the `.zip` file anywhere on your computer.
4. Double-click `StreetFootballManager.exe` and enjoy!

## 🌟 Game Modes
* **Career Mode:** Build your crew from the ground up. Buy/sell players, manage your starting lineup, and advance through a fully simulated league complete with standings, top scorer tracking, and a persistent save/load system.
* **Custom Match (PvE & PvP):** Play a quick match against the AI or a friend (Local Multiplayer). Fully parameterized match settings allow you to customize the Pitch (Asphalt, Mud, Grass), Weather (Clear, Rainy, Snowy), Match Length, and Difficulty.

## 🛠️ Technical Architecture

This project adheres to strict Object-Oriented Programming (OOP) principles and the following C++ features:

* **Polymorphism & Data Structures:** The core game loop operates on a `std::vector<std::unique_ptr<GameObject>>`. All interactive entities (`Footballer`, `Ball`, `Pitch`) inherit from `GameObject`, allowing the `MatchState` to update and render everything polymorphically.
* **Dynamic Memory & Type Casting:** Heavy utilization of modern smart pointers (`std::unique_ptr`, `std::shared_ptr`) ensures zero memory leaks. `dynamic_cast` is utilized by the AI to identify teammates and opponents from the generic GameObject pool.
* **State Machine Design:** The flow of the program is supervised by a central `Game` class that transitions between isolated states ( e.g. `MenuState`, `MatchState`, `CareerHubState`) via a robust Game State pattern.
* **Delta-Time Physics:** Object movement, cooldowns, and the custom "Fake 3D" ball physics (simulating Z-axis gravity, friction, and bouncing over a 2D shadow) are completely decoupled from framerate using a precise `dt` clock passed down from the base loop.
* **Complex AI FSM:** The `AIBrain` utilizes a Finite State Machine (using states like `Loose_Chasing`, `Loose_Supporting`, `Attacking_OnBall`, `Attacking_Supporting`, `Defending_Covering`, `Defending_Pressing`) to dynamically calculate positioning and pass weight based on the ball carrier's location and momentum.
* **Parameterization & File I/O:** The game features dynamic key-binding and volume settings saved to `settings.txt`, alongside a robust file serialization system that encodes complex career data (Rosters, League Tables, Match Schedules) to local storage.

## 🎮 Controls (Fully Rebindable)
* **Player 1:** `WASD` to move, `E` to Pass/Switch, `SPACE` to Shoot/Tackle, `LEFT SHIFT` for Lob modifier.
* **Player 2:** `Arrow Keys` to move, `.` to Pass/Switch, `ENTER` to Shoot/Tackle, `RIGHT SHIFT` for Lob modifier.
*(Controls can be altered dynamically in the Main Menu -> Settings -> Controls).*

### Prerequisites
* **C++17** compatible compiler (GCC, Clang, MSVC)
* **SFML 3.0+** (Simple and Fast Multimedia Library)

### Building the Project
1. Clone the repository:
   ```bash
   git clone [https://github.com/YourUsername/RetroStreetFootball.git](https://github.com/YourUsername/RetroStreetFootball.git)
