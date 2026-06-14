#pragma once
#include <string>
#include <vector>
#include "../Entities/Entity.h"
#include "../Entities/Pitch.h"
#include <algorithm>
#include <fstream>


/**
 * @struct CareerPlayer
 * @brief Represents an individual footballer within the career mode.
 * Tracks their RPG-style progression, market value, and season statistics.
 */
struct CareerPlayer {
    std::string name;
    EntityStats stats;
    int cost;
    bool isStarter = false;
    int goalsScored = 0;

    // Dynamically calculates the player's market value based on their overall stat pool
    void calculateCost() {
        float totalStats = stats.speed + stats.shooting + stats.passing + stats.tackling + stats.maxStamina;
        cost = static_cast<int>(totalStats * 0.5f);
    }
};

/**
 * @struct LeagueTeam
 * @brief Represents an opposing team in the street league.
 * Maintains their standings on the leaderboard as well as their specific AI roster.
 */
struct LeagueTeam {
    int id; // 0 is reserved for the Player's Team
    std::string name;
    EntityStats teamAverageStats;

    std::vector<CareerPlayer> roster;

    PitchType homePitch;
    int logoId;

    // Seasonal standings data
    int points = 0;
    int wins = 0;
    int draws = 0;
    int losses = 0;
    int goalsFor = 0;
    int goalsAgainst = 0;

    int goalDifference() const { return goalsFor - goalsAgainst; }
};

/**
 * @struct MatchFixture
 * @brief Defines a single scheduled game within the season timeline.
 */
struct MatchFixture {
    int week;
    int homeTeamId;
    int awayTeamId;
    int homeScore = -1;
    int awayScore = -1;
    bool isPlayed = false;
};

/**
 * @class CareerData
 * @brief Central data hub for the single-player campaign.
 * Handles state management for the player's team, the league table, and the schedule.
 * Also responsible for serializing this state to and from plain text files.
 */
class CareerData {
public:
    std::string teamName;
    int streetCred;
    int currentWeek;
    std::vector<CareerPlayer> roster;

    std::vector<LeagueTeam> leagueTable;
    std::vector<MatchFixture> schedule;

    PitchType homePitch;
    int logoId;

    /**
     * Generates a safe file path for the save file by sanitizing the team name.
     */
    std::string getSaveFileName() const {
        std::string safeName = teamName;
        std::replace(safeName.begin(), safeName.end(), ' ', '_');
        return "Saves/" + safeName + ".txt";
    }

    bool saveToFile();
    bool loadFromFile(const std::string& filepath);

private:
    void savePlayer(std::ofstream& out, const CareerPlayer& p);
    void loadPlayer(std::ifstream& in, CareerPlayer& p);
};