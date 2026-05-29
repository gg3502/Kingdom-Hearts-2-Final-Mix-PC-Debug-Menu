
#include "SpawnEnemy.hpp"
#include <helper.hpp>
#include "kh2-dmenu.hpp"


SpawnEnemy::SpawnEnemy(uint32_t entityId, int amount)
    : m_entityId(entityId), m_amount(amount) {}

void SpawnEnemy::Activate() {
    LOG(L"Spawning enemy: %08X x%d", m_entityId, m_amount);
    NewObj(m_entityId, m_amount); // Your own object spawning function
}

std::string SpawnEnemy::GetName() const {
    return "Spawn Enemy: ID " + std::to_string(m_entityId) + " x" + std::to_string(m_amount);
}
