#pragma once
#include "../Career/CareerData.h"
#include <memory>

struct ScorerRecord {
    std::string playerName;
    std::string teamName;
    int goals;
};

class LeagueSimulator {
public:
    static void initializeLeague(std::shared_ptr<CareerData> career, int numTeams);

    static void simulateWeek(std::shared_ptr<CareerData> career, int week);

    static void sortStandings(std::shared_ptr<CareerData> career);

    static void assignGoals(std::vector<CareerPlayer>& roster, int goalsToAssign);

    static std::vector<ScorerRecord> getTopScorers(std::shared_ptr<CareerData> career, int limit = 10);
};