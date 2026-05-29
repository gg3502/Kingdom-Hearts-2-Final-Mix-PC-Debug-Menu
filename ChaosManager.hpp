#pragma once
#include <vector>
#include <string>

class ChaosManager {
public:
    void Init();
    void Update();

    void EnableChaos(bool enable);
    bool IsChaosEnabled() const;

    bool IsInitialized() const;
    void SetInitialized(bool initialized);

private:
    bool chaosEnabled = false;
    bool chaosInitialized = false;
    float timeSinceLastVote = 0.0f;
    float voteCooldown = 5.0f;         // Seconds between effects
    float voteDuration = 15.0f;        // Vote duration in seconds

    void StartVote();
    bool ShouldStartNewVote() const;
};

struct VoteOption {
    std::string name;
    int effectIndex;
    bool requiresPlayer;
    bool requiresNormalForm;
};

extern ChaosManager chaosManager;

extern bool isPlayerInArea;