#ifndef GAME_STATISTICS_H
#define GAME_STATISTICS_H
#include <ostream>

class GameStatistics {
    int totalKills;
    int totalScore;
    int highestWave;

    GameStatistics();
public:
    GameStatistics(const GameStatistics&) = delete;
    GameStatistics& operator=(const GameStatistics&) = delete;

    static GameStatistics& instance();

    void recordKill(int points);
    void recordWave(int wave);
    void reset();

    int getTotalKills() const;
    int getTotalScore() const;
    int getHighestWave() const;

    friend std::ostream& operator<<(std::ostream& os, const GameStatistics& s);
};

#endif
