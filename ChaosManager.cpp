#include "ChaosManager.hpp"
#include "VoteManager.hpp"
#include "EffectManager.hpp"
#include <chrono>
#include "imgui.h"
#include <random>
#include <EffectRegistry.hpp>
#include <helper.hpp>
#include <hook_directx12.hpp>

//VoteManager voteManager;
EffectManager effectManager;
//SChaosManager chaosManager;
std::vector<std::shared_ptr<EffectBase>> effects;
std::vector<VoteOption> currentVoteOptions;

void ChaosManager::Init() {
    chaosEnabled = true;
    timeSinceLastVote = 0.0f;
    //effects = RegisterAllEffects();
}

void ChaosManager::EnableChaos(bool enable) {
    chaosEnabled = enable;
}

bool ChaosManager::IsChaosEnabled() const {
    return chaosEnabled;
}



void ChaosManager::Update() {
    if (!chaosEnabled) return;

    voteManager.Update();

    if (voteManager.IsVotingActive()) {
        if (voteManager.IsVoteComplete()) {
            int winner = voteManager.GetWinningIndex();
            if (winner != -1 && winner < currentVoteOptions.size()) {
                int effectIdx = currentVoteOptions[winner].effectIndex;
                if (effectIdx != -1) {
                    effectManager.TriggerEffect(effectIdx);
                }
                else {
                    LOG("No spawn effect triggered because player is not in area.");
                }
                //effectManager.TriggerEffect(effectIdx);
            }
            voteManager.EndVote();
            timeSinceLastVote = 0.0f;
        }

    }
    else {
        timeSinceLastVote += ImGui::GetIO().DeltaTime;
        if (ShouldStartNewVote()) {
            StartVote();
            timeSinceLastVote = 0.0f;
        }
    }
}

bool ChaosManager::ShouldStartNewVote() const {
    return timeSinceLastVote >= voteCooldown;
}

void ChaosManager::StartVote() {
    std::vector<VoteOption> allOptions = {
        { "Spawn Terra Boss", 0, true, false },
        { "Spawn Sephiroth Boss", 1, true, false },
        { "Spawn Mickey Ally", 2, true, false },
        { "Spawn Pete Boss", 3, true, false },
        { "Spawn 20 Shadow Heartless", 4, true, false },
        { "Spawn Cloud Ally", 5, true, false },
        { "Spawn Tifa Ally", 6, true, false },
        { "Spawn Yuffie Ally", 7, true, false },
        { "Spawn Leon Ally", 8, true, false },
        { "Spawn Cloud Boss", 9, true, false },
        { "Spawn Tifa Boss", 10, true, false },
        { "Spawn Yuffie Boss", 11, true, false },
        { "Spawn Leon Boss", 12, true, false },

        /*{"Change Sora to Lion Form", 5, true, true},
        { "Change Sora to PrideLands Donald Form", 6, true, true },
        { "Change Sora to PrideLands Goofy Form", 7, true, true },
        { "Change Sora to Donald Form", 8, true, true },
        { "Change Sora to Goofy Form", 9, true, true },
        { "Change Sora to Ping Form", 10, true, true },
        { "Change Sora to Jack Sparrow Form", 11, true, true },
        { "Change Sora to Mickey Coat Form", 12, true, true },
        { "Change Sora to Mickey Form", 13, true, true }*/
    };

    std::vector<VoteOption> filteredOptions;

    for (auto& opt : allOptions) {
        // Block ALL effects if Sora is currently in a form
        if (g_currentFormTime > 0.0f) {
            continue;
        }

        // Optional: If you still want to check for player presence later
        // if (opt.requiresPlayer && !g_playerIsInArea) {
        //     continue;
        // }

        filteredOptions.push_back(opt);
    }

    if (filteredOptions.empty()) {
        filteredOptions.push_back({ "No spawn options available (currently in Form)", -1, false });
    }

    // Shuffle and reduce to 3 options
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(filteredOptions.begin(), filteredOptions.end(), g);

    if (filteredOptions.size() > 3) {
        filteredOptions.resize(3);
    }

    std::vector<std::string> optionNames;
    for (const auto& opt : filteredOptions) {
        optionNames.push_back(opt.name);
    }

    currentVoteOptions = filteredOptions;
    voteManager.BeginVote(optionNames, static_cast<int>(voteDuration));
}



bool ChaosManager::IsInitialized() const {
    return chaosInitialized;
}

void ChaosManager::SetInitialized(bool initialized) {
    chaosInitialized = initialized;
}
