#include "EffectManager.hpp"
#include "EffectRegistry.hpp"
#include <helper.hpp>

void EffectManager::Init() {
    effects = RegisterAllEffects(); // Get everything in one call

}

void EffectManager::TriggerEffect(int effectIndex) {
    LOG(L"EffectManager::TriggerEffect - Index: %d\n", effectIndex);
    if (effectIndex >= 0 && effectIndex < effects.size()) {
        effects[effectIndex]->Activate();
    }
    else {
        LOG(L"EffectManager::TriggerEffect - Invalid effect index!\n");
    }
}

void EffectManager::RegisterEffect(std::shared_ptr<EffectBase> effect) {
    effects.push_back(effect);
}
