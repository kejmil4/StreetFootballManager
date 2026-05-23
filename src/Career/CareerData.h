#pragma once
#include <string>
#include <vector>
#include "../Entities/Entity.h"

struct CareerPlayer {
    std::string name;
    EntityStats stats;
    int cost;
    bool isStarter = false;

    void calculateCost() {
        float totalStats = stats.speed + stats.shooting + stats.passing + stats.tackling + stats.maxStamina;
        cost = static_cast<int>(totalStats * 0.5f);
    }
};

struct LeagueTeam {
    int id; // 0 - the Player's Team
    std::string name;
    EntityStats teamAverageStats;

    // Standings tracking
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

    void generateNewCareer() {
        teamName = "The Rookies";
        streetCred = 500;
        currentWeek = 1;
        roster.clear();

        EntityStats baseline = {100.f, 100.f, 100.f, 100.f, 100.f};
        roster.push_back({"Street Kid A", baseline, 250, true});
        roster.push_back({"Street Kid B", baseline, 250, true});
        roster.push_back({"Street Kid C", baseline, 250, true});
    }
};