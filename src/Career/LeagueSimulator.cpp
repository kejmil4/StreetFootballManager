#include "LeagueSimulator.h"
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <random>
#include <chrono>

void LeagueSimulator::initializeLeague(std::shared_ptr<CareerData> career, int numTeams) {
    career->leagueTable.clear();
    career->schedule.clear();

    // --- Phase 1: Team Generation ---

    // 1. Setup the Player's Team (ID 0 is exclusively reserved for the human player)
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

        // Establish a base power level for the team to scale player stats around
        float power = 80.f + (rand() % 50);
        aiTeam.teamAverageStats = {power, power, power, power, power};

        // Generate the 3-man starting roster
        for (int p = 0; p < 3; ++p) {
            CareerPlayer aiPlayer;
            aiPlayer.name = aiTeam.name.substr(0, aiTeam.name.find(' ')) + " " + std::to_string(p + 1);

            // Add minor variance (+/- 15) to stats so players aren't clones of each other
            aiPlayer.stats.speed = power + (rand() % 30 - 15);
            aiPlayer.stats.shooting = power + (rand() % 30 - 15);
            aiPlayer.stats.passing = power + (rand() % 30 - 15);
            aiPlayer.stats.tackling = power + (rand() % 30 - 15);
            aiPlayer.stats.maxStamina = power + (rand() % 30 - 15);

            aiPlayer.isStarter = true;
            aiTeam.roster.push_back(aiPlayer);
        }

        career->leagueTable.push_back(aiTeam);
    }

    // --- Phase 2: Schedule Generation ---
    // Uses the "Circle Method" algorithm for standard Round-Robin scheduling
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
        // Rotate the teams array (keeping the first team fixed) to generate the next round
        int lastTeam = teamIds.back();
        teamIds.pop_back();
        teamIds.insert(teamIds.begin() + 1, lastTeam);

        currentWeek++;
    }

    // Second Half of Season (Mirror the first half, but swap Home/Away advantage)
    int firstHalfWeeks = currentWeek - 1;
    for (int i = 0; i < (totalRounds * matchesPerRound); ++i) {
        MatchFixture reverseFixture = career->schedule[i];
        reverseFixture.week = career->schedule[i].week + firstHalfWeeks;
        int temp = reverseFixture.homeTeamId;
        reverseFixture.homeTeamId = reverseFixture.awayTeamId;
        reverseFixture.awayTeamId = temp;
        career->schedule.push_back(reverseFixture);
    }

    /// --- Phase 3: Schedule Shuffling ---
    // Group the matches by week, then shuffle the order of the weeks themselves.
    // This prevents the player from always playing teams in the exact same order every season.
    int totalWeeks = (numTeams - 1) * 2;
    std::vector<std::vector<MatchFixture>> groupedWeeks(totalWeeks);
    for (const auto& match : career->schedule) {
        groupedWeeks[match.week - 1].push_back(match);
    }

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(groupedWeeks.begin(), groupedWeeks.end(), std::default_random_engine(seed));

    // Rebuild the final flat schedule
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

            auto homeIt = std::find_if(career->leagueTable.begin(), career->leagueTable.end(),
                [&](const LeagueTeam& t) { return t.id == match.homeTeamId; });
            auto awayIt = std::find_if(career->leagueTable.begin(), career->leagueTable.end(),
                [&](const LeagueTeam& t) { return t.id == match.awayTeamId; });

            // Ensure both teams were actually found before simulating
            if (homeIt != career->leagueTable.end() && awayIt != career->leagueTable.end()) {
                auto& homeTeam = *homeIt;
                auto& awayTeam = *awayIt;

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
    }

    sortStandings(career);
}

void LeagueSimulator::sortStandings(std::shared_ptr<CareerData> career) {
    // Sort primarily by points. If tied, resolve via Goal Difference.
    std::sort(career->leagueTable.begin(), career->leagueTable.end(), [](const LeagueTeam& a, const LeagueTeam& b) {
        if (a.points != b.points) return a.points > b.points;
        return a.goalDifference() > b.goalDifference();
    });
}

void LeagueSimulator::assignGoals(std::vector<CareerPlayer>& roster, int goalsToAssign) {
    if (goalsToAssign <= 0) return;

    // 1. Filter out bench players; only starters can score in simulations
    std::vector<CareerPlayer*> starters;
    float totalShootingWeight = 0.f;

    for (auto& player : roster) {
        if (player.isStarter) {
            starters.push_back(&player);
            totalShootingWeight += player.stats.shooting;
        }
    }

    if (starters.empty()) return;

    // 2. Weighted Lottery System for Goal Distribution
    // Better shooters mathematically claim a larger "slice" of the randomization pie.
    for (int i = 0; i < goalsToAssign; ++i) {
        float randomRoll = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * totalShootingWeight;

        float currentWeight = 0.f;
        for (auto* player : starters) {
            currentWeight += player->stats.shooting;
            if (randomRoll <= currentWeight) {
                player->goalsScored += 1;
                break; // Lottery winner found, break and assign the next goal
            }
        }
    }
}

std::vector<ScorerRecord> LeagueSimulator::getTopScorers(std::shared_ptr<CareerData> career, int limit) {
    std::vector<ScorerRecord> allScorers;

    // 1. Fetch player goals
    for (const auto& p : career->roster) {
        if (p.goalsScored > 0) {
            allScorers.push_back({p.name, career->teamName, p.goalsScored});
        }
    }

    // 2. Fetch AI goals
    for (const auto& team : career->leagueTable) {
        if (team.id == 0) continue; // Skip player team (already did it above)
        for (const auto& p : team.roster) {
            if (p.goalsScored > 0) {
                allScorers.push_back({p.name, team.name, p.goalsScored});
            }
        }
    }

    // 3. Sort descending by goal count
    std::sort(allScorers.begin(), allScorers.end(), [](const ScorerRecord& a, const ScorerRecord& b) {
        return a.goals > b.goals;
    });

    // 4. Trim leaderboard to requested UI limit
    if (allScorers.size() > limit) {
        allScorers.resize(limit);
    }

    return allScorers;
}