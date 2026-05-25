#include "LeagueSimulator.h"
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <random>
#include <chrono>

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

        int arrayIndex = (i - 1) % aiNames.size();

        aiTeam.name = aiNames[arrayIndex];

        aiTeam.homePitch = static_cast<PitchType>(rand() % 3);

        aiTeam.logoId = i - 1;

        float power = 80.f + (rand() % 50);
        aiTeam.teamAverageStats = {power, power, power, power, power};

        for (int p = 0; p < 3; ++p) { // Generate 3 starters for the AI
            CareerPlayer aiPlayer;
            // Name them after their team so you recognize them! (e.g., "Viper 1")
            aiPlayer.name = aiTeam.name.substr(0, aiTeam.name.find(' ')) + " " + std::to_string(p + 1);

            // Randomize their specific stats around the team's average power
            aiPlayer.stats.speed = power + (rand() % 30 - 15);
            aiPlayer.stats.shooting = power + (rand() % 30 - 15);
            aiPlayer.stats.passing = power + (rand() % 30 - 15);
            aiPlayer.stats.tackling = power + (rand() % 30 - 15);
            aiPlayer.stats.maxStamina = power + (rand() % 30 - 15);

            aiPlayer.isStarter = true; // AI players always start
            aiTeam.roster.push_back(aiPlayer);
        }

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
        int temp = reverseFixture.homeTeamId;
        reverseFixture.homeTeamId = reverseFixture.awayTeamId;
        reverseFixture.awayTeamId = temp;
        career->schedule.push_back(reverseFixture);
    }
    // SHUFFLE THE WEEKS
    int totalWeeks = (numTeams - 1) * 2;
    std::vector<std::vector<MatchFixture>> groupedWeeks(totalWeeks);
    for (const auto& match : career->schedule) {
        groupedWeeks[match.week - 1].push_back(match);
    }

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(groupedWeeks.begin(), groupedWeeks.end(), std::default_random_engine(seed));

    career->schedule.clear();
    for (int w = 0; w < groupedWeeks.size(); ++w) {
        for (auto& match : groupedWeeks[w]) {
            match.week = w + 1;
            career->schedule.push_back(match);
        }
    }
}

void LeagueSimulator::simulateWeek(std::shared_ptr<CareerData> career, int week) {
    for (auto& match : career->schedule) {
        // Find matches for the current week that are NOT played by the player (ID 0)
        if (match.week == week && !match.isPlayed && match.homeTeamId != 0 && match.awayTeamId != 0) {

            auto& homeTeam = career->leagueTable[match.homeTeamId];
            auto& awayTeam = career->leagueTable[match.awayTeamId];

            int homeAdvantage = 15;
            float homePower = homeTeam.teamAverageStats.speed + homeAdvantage + (rand() % 50);
            float awayPower = awayTeam.teamAverageStats.speed + (rand() % 50);

            match.homeScore = (rand() % 3) + 1;
            match.awayScore = (rand() % 3) + 1;

            if (homePower > awayPower) {
                match.homeScore += (rand() % 3) + 1;
            } else if (awayPower > homePower) {
                match.awayScore += (rand() % 3) + 1;
            } else {
                match.homeScore += 1;
                match.awayScore += 1;
            }

            match.isPlayed = true;

            assignGoals(homeTeam.roster, match.homeScore);
            assignGoals(awayTeam.roster, match.awayScore);

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

void LeagueSimulator::assignGoals(std::vector<CareerPlayer>& roster, int goalsToAssign) {
    if (goalsToAssign <= 0) return;

    // 1. Find only the players who actually played (Starters)
    std::vector<CareerPlayer*> starters;
    float totalShootingWeight = 0.f;

    for (auto& player : roster) {
        if (player.isStarter) {
            starters.push_back(&player);
            totalShootingWeight += player.stats.shooting;
        }
    }

    if (starters.empty()) return; // Safety check

    // 2. Assign each goal using a weighted random selection!
    for (int i = 0; i < goalsToAssign; ++i) {
        // Pick a random number between 0 and the total shooting stats of the team
        float randomRoll = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * totalShootingWeight;

        float currentWeight = 0.f;
        for (auto* player : starters) {
            currentWeight += player->stats.shooting;
            if (randomRoll <= currentWeight) {
                player->goalsScored += 1;
                break; // Goal assigned, move to the next goal!
            }
        }
    }
}

std::vector<ScorerRecord> LeagueSimulator::getTopScorers(std::shared_ptr<CareerData> career, int limit) {
    std::vector<ScorerRecord> allScorers;

    // 1. Grab the Player's team
    for (const auto& p : career->roster) {
        if (p.goalsScored > 0) {
            allScorers.push_back({p.name, career->teamName, p.goalsScored});
        }
    }

    // 2. Grab all the AI teams
    for (const auto& team : career->leagueTable) {
        if (team.id == 0) continue; // Skip player team (already did it above)
        for (const auto& p : team.roster) {
            if (p.goalsScored > 0) {
                allScorers.push_back({p.name, team.name, p.goalsScored});
            }
        }
    }

    // 3. Sort by goals descending
    std::sort(allScorers.begin(), allScorers.end(), [](const ScorerRecord& a, const ScorerRecord& b) {
        return a.goals > b.goals;
    });

    // 4. Trim to the limit (e.g., Top 10)
    if (allScorers.size() > limit) {
        allScorers.resize(limit);
    }

    return allScorers;
}