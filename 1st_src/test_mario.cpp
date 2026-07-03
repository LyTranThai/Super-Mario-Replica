#include "src/Core/EventSystem.h"
#include "src/Persistence/Account.h"
#include "src/Persistence/SaveManager.h"
#include "src/Entities/Player.h"
#include "src/Physics/CollisionManager.h"
#include <iostream>
#include <cassert>

// Mock event listener for testing
class MockListener : public EventListener {
public:
    int eventCount = 0;
    void onEvent(EventType type, void* data) override {
        (void)data;
        if (type == EventType::CoinCollected) {
            eventCount++;
        }
    }
};

int main() {
    std::cout << "========== RUNNING MARIO OOP TESTS ==========" << std::endl;

    // Test 1: Account Persistence
    {
        std::cout << "Test 1: Testing Save/Load Account Persistence..." << std::endl;
        SaveManager sm;
        
        // Remove file if exists
        std::remove("accounts/Tester.txt");

        bool regOk = sm.registerAccount("Tester", "mypass123");
        assert(regOk);
        
        bool loginFail = sm.verifyCredentials("Tester", "wrongpass");
        assert(!loginFail);

        bool loginOk = sm.verifyCredentials("Tester", "mypass123");
        assert(loginOk);

        Account outAcc;
        bool loadOk = sm.loadAccount("Tester", outAcc);
        assert(loadOk);
        assert(outAcc.getUsername() == "Tester");
        assert(outAcc.getCurrentLevel() == 1);
        
        // Update progress
        outAcc.setCurrentLevel(3);
        outAcc.setHighScore(15000);
        outAcc.setLevelHighScore(0, 5000);
        outAcc.setLevelHighScore(1, 10000);
        outAcc.setKeySetting("Jump", 321); // custom binding
        bool saveOk = sm.saveAccount(outAcc);
        assert(saveOk);

        Account reloaded;
        assert(sm.loadAccount("Tester", reloaded));
        assert(reloaded.getCurrentLevel() == 3);
        assert(reloaded.getHighScore() == 15000);
        assert(reloaded.getLevelHighScore(0) == 5000);
        assert(reloaded.getLevelHighScore(1) == 10000);
        assert(reloaded.getLevelHighScore(2) == 0);
        assert(reloaded.getKeySetting("Jump") == 321);

        // Clean up save file
        std::remove("accounts/Tester.txt");
        std::cout << "-> Test 1 Passed!" << std::endl;
    }

    // Test 2: Double Jump Logic
    {
        std::cout << "Test 2: Testing Double Jump Mechanics..." << std::endl;
        Player player(Vector2{ 100.0f, 100.0f });
        
        // Starts grounded
        player.setOnGround(true);
        assert(player.isOnGround());

        // First Jump
        player.jump();
        assert(!player.isOnGround());
        assert(player.getVelocity().y < 0.0f); // moving up

        // Second Jump (mid-air)
        float prevVelY = player.getVelocity().y;
        player.jump();
        assert(player.getVelocity().y < 0.0f); 

        // Third Jump should do nothing (locked until grounded)
        float velAfterSecond = player.getVelocity().y;
        player.jump();
        assert(player.getVelocity().y == velAfterSecond); // Velocity remains unchanged

        // Touch ground resets jumps
        player.setOnGround(true);
        player.jump(); // Should work again
        assert(!player.isOnGround());
        std::cout << "-> Test 2 Passed!" << std::endl;
    }

    // Test 3: Event Broadcasting (Observer Pattern)
    {
        std::cout << "Test 3: Testing Event Broadcasting..." << std::endl;
        MockListener mock;
        EventManager::getInstance().subscribe(EventType::CoinCollected, &mock);

        assert(mock.eventCount == 0);
        EventManager::getInstance().broadcast(EventType::CoinCollected);
        assert(mock.eventCount == 1);

        EventManager::getInstance().broadcast(EventType::CoinCollected);
        assert(mock.eventCount == 2);

        EventManager::getInstance().unsubscribe(EventType::CoinCollected, &mock);
        EventManager::getInstance().broadcast(EventType::CoinCollected);
        assert(mock.eventCount == 2); // Unsubscribed, counter should stay 2
        std::cout << "-> Test 3 Passed!" << std::endl;
    }

    std::cout << "========== ALL TESTS COMPLETED SUCCESSFULLY! ==========" << std::endl;
    return 0;
}
