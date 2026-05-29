#include "EffectRegistry.hpp"
#include "SpawnEnemy.hpp"
#include "ChangePlayerForm.hpp"
//#include "Effects/ExplodePlayer.h"
// Include more effects as you add them

std::vector<std::shared_ptr<EffectBase>> RegisterAllEffects() {
    std::vector<std::shared_ptr<EffectBase>> effects;

    // Example effects
    effects.push_back(std::make_shared<SpawnEnemy>(0x96F, 1)); // Terra
    effects.push_back(std::make_shared<SpawnEnemy>(0x8B6, 1)); // Sephiroth
    effects.push_back(std::make_shared<SpawnEnemy>(0x764, 1)); // Mickey ally
    effects.push_back(std::make_shared<SpawnEnemy>(0x90F, 1)); // Pete coliseum boss
    effects.push_back(std::make_shared<SpawnEnemy>(0x12E, 20)); // 20 Shadow Heartless
    effects.push_back(std::make_shared<SpawnEnemy>(0x688, 1)); // Ally Cloud
    effects.push_back(std::make_shared<SpawnEnemy>(0x6B3, 1)); // Tifa ally
    effects.push_back(std::make_shared<SpawnEnemy>(0x6B0, 1)); // Yuffie ally
    effects.push_back(std::make_shared<SpawnEnemy>(0x61C, 1)); // Leon ally
    effects.push_back(std::make_shared<SpawnEnemy>(0x8F9, 1)); // cloud boss
    effects.push_back(std::make_shared<SpawnEnemy>(0x8FA, 1)); // tifa boss
    effects.push_back(std::make_shared<SpawnEnemy>(0x8FB, 1)); // yuffie boss
    effects.push_back(std::make_shared<SpawnEnemy>(0x8F8, 1)); // leon boss

    /*effects.push_back(std::make_shared<ChangePlayerForm>(0x28A)); // Lion Sora
    effects.push_back(std::make_shared<ChangePlayerForm>(0x5EF)); // PrideLands Donald
    effects.push_back(std::make_shared<ChangePlayerForm>(0x61B)); // PrideLands Goofy
    effects.push_back(std::make_shared<ChangePlayerForm>(0x5C));  // Donald Form
    effects.push_back(std::make_shared<ChangePlayerForm>(0x5D));  // Goofy Form
    effects.push_back(std::make_shared<ChangePlayerForm>(0x64));  // Ping Form
    effects.push_back(std::make_shared<ChangePlayerForm>(0x66));  // Jack Sparrow Form
    effects.push_back(std::make_shared<ChangePlayerForm>(0x5B));  // Mickey Coat Form
    effects.push_back(std::make_shared<ChangePlayerForm>(0x318)); // Mickey Form*/

   // effects.push_back(std::make_shared<ChangePlayerForm>(2)); // Wisdom
   // effects.push_back(std::make_shared<ChangePlayerForm>(3)); // Limit
  //  effects.push_back(std::make_shared<ChangePlayerForm>(4)); // Master
  //  effects.push_back(std::make_shared<ChangePlayerForm>(5)); // Final
 //   effects.push_back(std::make_shared<ChangePlayerForm>(6)); // Anti-Form?

    //effects.push_back(std::make_shared<ExplodePlayer>());

    // Add more effect variants here...

    return effects;
}
