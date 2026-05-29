#pragma once

#include <vector>
#include <memory>
#include <string>
#include "EffectBase.hpp"

class EffectManager {
public:
    // Initialize and load all effects (call once at startup)
    void Init();

    // Get all effects (useful for UI or voting)
    const std::vector<std::shared_ptr<EffectBase>>& GetEffects() const;

    // Activate a specific effect by index (e.g., from voting)
    void ActivateEffectByIndex(int index);

    // Activate a specific effect by name (optional convenience)
    bool ActivateEffectByName(const std::string& name);

    // Pick a random effect and activate it
    void ActivateRandomEffect();

    void TriggerEffect(int effectIndex);

    void RegisterEffect(std::shared_ptr<EffectBase> effect);

private:
    std::vector<std::shared_ptr<EffectBase>> effects;
};

extern EffectManager effectManager;
