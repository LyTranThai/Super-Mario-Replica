#ifndef SAVE_MANAGER_H
#define SAVE_MANAGER_H

#include <string>

class Account;

class SaveManager {
private:
    std::string saveDirectory;

    std::string getFilePath(const std::string& username) const;
    std::string simpleHash(const std::string& password) const;

public:
    SaveManager();

    bool registerAccount(const std::string& username, const std::string& password);
    bool verifyCredentials(const std::string& username, const std::string& password);
    bool loadAccount(const std::string& username, Account& outAccount);
    bool saveAccount(const Account& account);
};

#endif // SAVE_MANAGER_H
