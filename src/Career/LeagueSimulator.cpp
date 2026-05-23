#include "LeagueSimulator.h"
#include <cstdlib>
#include <iostream>
#include <algorithm>

void LeagueSimulator::initializeLeague(std::shared_ptr<CareerData> career, int numTeams) {
    career->leagueTable.clear();
    career->schedule.clear();

    // 1. Setup the Player's Team (ID 0)
    LeagueTeam playerTeam;
    playerTeam.id = 0;
    playerTeam.name = career->teamName;
    playerTeam.teamAverageStats = {100.f, 100.f, 100.f, 100.f, 100.f}; // Baseline start
    career->leagueTable.push_back(playerTeam);

    // 2. Generate AI Teams
    std::vector<std::string> aiNames = {
        "Downtown Vipers", "Southside Kings", "Neon Nomads", "Concrete Crew",
        "Alley Cats", "Metro United", "Riot Squad", "Brickyard Boys",
        "Subway Syndicate", "Asphalt Aces", "Gridiron Gang"
    };

    for (int i = 1; i < numTeams; ++i) {
        LeagueTeam aiTeam;
        aiTeam.id = i;
        aiTeam.name = aiNames[i % aiNames.size()];

        // Randomize how good this AI team is (80 is weak, 130 is very strong)
        float power = 80.f + (rand() % 50);
        aiTeam.teamAverageStats = {power, power, power, power, power};

        career->leagueTable.push_back(aiTeam);
    }

    // 3. Generate Round-Robin Schedule (Home & Away)
    std::vector<int> teamIds;
    for (int i = 0; i < numTeams; ++i) teamIds.push_back(i);

    int totalRounds = numTeams - 1;
    int matchesPerRound = numTeams / 2;
    int currentWeek = 1;

    // First Half of Season
    for (int round = 0; round < totalRounds; ++round) {
        for (int match = 0; match < matchesPerRound; ++match) {
            int home = teamIds[match];
            int away = teamIds[numTeams - 1 - match];
            career->schedule.push_back({currentWeek, home, away});
        }

        // Rotate teams (keep index 0 fixed)
        int lastTeam = teamIds.back();
        teamIds.pop_back();
        teamIds.insert(teamIds.begin() + 1, lastTeam);

        currentWeek++;
    }

    // Second Half of Season (Reverse Home/Away)
    int firstHalfWeeks = currentWeek - 1;
    for (int i = 0; i < (totalRounds * matchesPerRound); ++i) {
        MatchFixture reverseFixture = career->schedule[i];
        reverseFixture.week = career->schedule[i].week + firstHalfWeeks;
        // Swap Home and Away
        int temp = reverseFixture.homeTeamId;
        reverseFixture.homeTeamId = reverseFixture.awayTeamId;
        reverseFixture.awayTeamId = temp;
        career->schedule.push_back(reverseFixture);
    }
}

void LeagueSimulator::simulateWeek(std::shared_ptr<CareerData> career, int week) {
    for (auto& match : career->schedule) {
        // Find matches for the current week that are NOT played by the player (ID 0)
        if (match.week == week && !match.isPlayed && match.homeTeamId != 0 && match.awayTeamId != 0) {

            // Grab references to the teams
            auto& homeTeam = career->leagueTable[match.homeTeamId];
            auto& awayTeam = career->leagueTable[match.awayTeamId];

            // Simple Simulation Math based on Team Stats + RNG
            int homeAdvantage = 10; // Home teams get a slight boost
            float homePower = homeTeam.teamAverageStats.speed + homeAdvantage + (rand() % 40);
            float awayPower = awayTeam.teamAverageStats.speed + (rand() % 40);

            // Calculate goals
            match.homeScore = (homePower > awayPower) ? (rand() % 4) + 1 : (rand() % 2);
            match.awayScore = (awayPower > homePower) ? (rand() % 4) + 1 : (rand() % 2);
            match.isPlayed = true;

            // Update Standings Logic
            homeTeam.goalsFor += match.homeScore;
            homeTeam.goalsAgainst += match.awayScore;
            awayTeam.goalsFor += match.awayScore;
            awayTeam.goalsAgainst += match.homeScore;

            if (match.homeScore > match.awayScore) {
                homeTeam.points += 3;
                homeTeam.wins++;
                awayTeam.losses++;
            } else if (match.awayScore > match.homeScore) {
                awayTeam.points += 3;
                awayTeam.wins++;
                homeTeam.losses++;
            } else {
                homeTeam.points += 1;
                awayTeam.points += 1;
                homeTeam.draws++;
                awayTeam.draws++;
            }
        }
    }

    sortStandings(career);
}

void LeagueSimulator::sortStandings(std::shared_ptr<CareerData> career) {
    std::sort(career->leagueTable.begin(), career->leagueTable.end(), [](const LeagueTeam& a, const LeagueTeam& b) {
        if (a.points != b.points) return a.points > b.points;
        return a.goalDifference() > b.goalDifference();
    });
}