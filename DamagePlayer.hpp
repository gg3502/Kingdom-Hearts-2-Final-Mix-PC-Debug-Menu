#include "EffectBase.hpp"

class ExplodePlayer : public EffectBase {
public:
    void Activate() override;
    std::string GetName() const override { return "Explode Player"; }
};

