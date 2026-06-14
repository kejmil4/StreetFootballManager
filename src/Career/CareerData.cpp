#include "CareerData.h"
#include <iostream>

// --- Serialization Helpers ---

/**
 * Serializes a single player's data into the output stream.
 * Format:
 * [Name]
 * [Speed] [Shooting] [Passing] [Tackling] [Stamina]
 * [Cost] [IsStarter] [GoalsScored]
 */
void CareerData::savePlayer(std::ofstream& out, const CareerPlayer& p) {
    out << p.name << "\n";
    out << p.stats.speed << " " << p.stats.shooting << " " << p.stats.passing << " "
        << p.stats.tackling << " " << p.stats.maxStamina << "\n";
    out << p.cost << " " << p.isStarter << " " << p.goalsScored << "\n";
}

/**
 * Deserializes a single player's data from the input stream.
 */
void CareerData::loadPlayer(std::ifstream& in, CareerPlayer& p) {
    std::getline(in, p.name);
    in >> p.stats.speed >> p.stats.shooting >> p.stats.passing >> p.stats.tackling >> p.stats.maxStamina;
    in >> p.cost >> p.isStarter >> p.goalsScored;

    // Note: When mixing stream extraction (>>) with std::getline, extraction leaves
    // the trailing newline character (\n) in the buffer. We must consume it here,
    // otherwise the next std::getline call will immediately read an empty string.
    std::string dummy;
    std::getline(in, dummy);
}

// --- Main I/O Operations ---

/**
 * Saves the current campaign state to a plain text file.
 * We use a custom sequential text format to keep saves lightweight and easily modifiable.
 */
bool CareerData::saveToFile() {
    std::ofstream out(getSaveFileName());
    if (!out.is_open()) return false;

    // Block 1: Core Team Data
    out << teamName << "\n";
    out << streetCred << " " << currentWeek << " " << static_cast<int>(homePitch) << " " << logoId << "\n";

    // Block 2: Player Roster
    out << roster.size() << "\n";
    for (const auto& p : roster) {
        savePlayer(out, p);
    }

    // Block 3: League Table (Standings & AI Rosters)
    out << leagueTable.size() << "\n";
    for (const auto& team : leagueTable) {
        out << team.id << "\n";
        out << team.name << "\n";

        // Team Stats & Identity
        out << team.teamAverageStats.speed << "\n";
        out << static_cast<int>(team.homePitch) << " " << team.logoId << "\n";

        // Season Standings
        out << team.points << " " << team.wins << " " << team.draws << " " << team.losses << " "
            << team.goalsFor << " " << team.goalsAgainst << "\n";

        // AI Roster
        out << team.roster.size() << "\n";
        for (const auto& p : team.roster) savePlayer(out, p);
    }

    // Block 4: Match Schedule
    out << schedule.size() << "\n";
    for (const auto& f : schedule) {
        out << f.week << " " << f.homeTeamId << " " << f.awayTeamId << " "
            << f.homeScore << " " << f.awayScore << " " << f.isPlayed << "\n";
    }

    out.close();
    return true;
}

/**
 * Reconstructs the campaign state by parsing the formatted text file.
 */
bool CareerData::loadFromFile(const std::string& filepath) {
    std::ifstream in(filepath);
    if (!in.is_open()) return false;

    std::string dummy;

    // Block 1: Core Team Data
    std::getline(in, teamName);

    int pitchInt;
    in >> streetCred >> currentWeek >> pitchInt >> logoId;
    homePitch = static_cast<PitchType>(pitchInt);
    std::getline(in, dummy);// Consume trailing newline

    // Block 2: Player Roster
    int rosterSize;
    in >> rosterSize;
    std::getline(in, dummy);
    roster.clear();
    for (int i = 0; i < rosterSize; ++i) {
        CareerPlayer p;
        loadPlayer(in, p);
        roster.push_back(p);
    }

    // Block 3: League Table
    int tableSize;
    in >> tableSize;
    std::getline(in, dummy);
    leagueTable.clear();
    for (int i = 0; i < tableSize; ++i) {
        LeagueTeam team;
        in >> team.id;
        std::getline(in, dummy);

        std::getline(in, team.name);

        // Load simplified average stats for simulated matches
        float avgStat;
        in >> avgStat;
        team.teamAverageStats = {avgStat, avgStat, avgStat, avgStat, avgStat};

        int tPitch;
        in >> tPitch >> team.logoId;
        team.homePitch = static_cast<PitchType>(tPitch);

        in >> team.points >> team.wins >> team.draws >> team.losses >> team.goalsFor >> team.goalsAgainst;
        std::getline(in, dummy);

        int aiRosterSize;
        in >> aiRosterSize;
        std::getline(in, dummy);
        for (int pIdx = 0; pIdx < aiRosterSize; ++pIdx) {
            CareerPlayer p;
            loadPlayer(in, p);
            team.roster.push_back(p);
        }
        leagueTable.push_back(team);
    }

    // Block 4: Match Schedule
    int scheduleSize;
    in >> scheduleSize;
    std::getline(in, dummy);
    schedule.clear();
    for (int i = 0; i < scheduleSize; ++i) {
        MatchFixture f;
        in >> f.week >> f.homeTeamId >> f.awayTeamId >> f.homeScore >> f.awayScore >> f.isPlayed;
        schedule.push_back(f);
    }

    in.close();
    return true;
}