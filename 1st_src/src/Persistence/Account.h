#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <string>
#include <map>

class Account {
private:
    std::string username;
    std::string passwordHash;
    int currentLevel;
    int highScore;
    std::map<std::string, int> keySettings;
    std::map<int, int> levelHighScores;

public:
    Account();
    Account(const std::string& user, const std::string& passHash);

    std::string getUsername() const { return username; }
    std::string getPasswordHash() const { return passwordHash; }

    int getCurrentLevel() const { return currentLevel; }
    void setCurrentLevel(int level) { currentLevel = level; }

    int getHighScore() const { return highScore; }
    void setHighScore(int score) { highScore = score; }

    int getLevelHighScore(int levelIndex) const {
        auto it = levelHighScores.find(levelIndex);
        if (it != levelHighScores.end()) {
            return it->second;
        }
        return 0;
    }
    void setLevelHighScore(int levelIndex, int score) {
        levelHighScores[levelIndex] = score;
    }
    const std::map<int, int>& getLevelHighScores() const { return levelHighScores; }
    void setLevelHighScores(const std::map<int, int>& scores) { levelHighScores = scores; }

    void setKeySetting(const std::string& actionName, int keyCode);
    int getKeySetting(const std::string& actionName) const;
    const std::map<std::string, int>& getKeySettings() const { return keySettings; }
    void loadKeySettings(const std::map<std::string, int>& settings);
};

#endif // ACCOUNT_H
