#pragma once
//#include "SpawnEnemy.hpp"
#include "EffectBase.hpp"

class SpawnEnemy : public EffectBase {
public:
    SpawnEnemy(uint32_t entityId, int amount);

    void Activate() override;
    std::string GetName() const override;

private:
    uint32_t m_entityId;
    int m_amount;
};

