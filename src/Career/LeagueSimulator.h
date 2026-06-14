#pragma once
#include "../Career/CareerData.h"
#include <memory>


/**
 * @struct ScorerRecord
 * @brief A lightweight data structure used for populating the Top Scorers UI leaderboard.
 */
struct ScorerRecord {
    std::string playerName;
    std::string teamName;
    int goals;
};

/**
 * @class LeagueSimulator
 * @brief A static utility class that handles the background math and logic of the league.
 * Responsible for generating the season schedule (round-robin), simulating background
 * matches, calculating points, and distributing stats based on team power levels.
 */
class LeagueSimulator {
public:
    static void initializeLeague(std::shared_ptr<CareerData> career, int numTeams);

    static void simulateWeek(std::shared_ptr<CareerData> career, int week);

    static void sortStandings(std::shared_ptr<CareerData> career);

    static void assignGoals(std::vector<CareerPlayer>& roster, int goalsToAssign);

    static std::vector<ScorerRecord> getTopScorers(std::shared_ptr<CareerData> career, int limit = 10);
};