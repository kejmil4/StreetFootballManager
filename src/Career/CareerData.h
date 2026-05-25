#pragma once
#include <string>
#include <vector>
#include "../Entities/Entity.h"
#include "../Entities/Pitch.h"
#include <algorithm>
#include <fstream>

struct CareerPlayer {
    std::string name;
    EntityStats stats;
    int cost;
    bool isStarter = false;
    int goalsScored = 0;

    void calculateCost() {
        float totalStats = stats.speed + stats.shooting + stats.passing + stats.tackling + stats.maxStamina;
        cost = static_cast<int>(totalStats * 0.5f);
    }
};

struct LeagueTeam {
    int id; // 0 - the Player's Team
    std::string name;
    EntityStats teamAverageStats;

    std::vector<CareerPlayer> roster;

    PitchType homePitch;
    int logoId;

    int points = 0;
    int wins = 0;
    int draws = 0;
    int losses = 0;
    int goalsFor = 0;
    int goalsAgainst = 0;
    int goalDifference() const { return goalsFor - goalsAgainst; }
};

struct MatchFixture {
    int week;
    int homeTeamId;
    int awayTeamId;
    int homeScore = -1;
    int awayScore = -1;
    bool isPlayed = false;
};

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