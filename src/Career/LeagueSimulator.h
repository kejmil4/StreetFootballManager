#pragma once
#include "../Career/CareerData.h"
#include <memory>

class LeagueSimulator {
public:
    static void initializeLeague(std::shared_ptr<CareerData> career, int numTeams);

    static void simulateWeek(std::shared_ptr<CareerData> career, int week);

    static void sortStandings(std::shared_ptr<CareerData> career);
};