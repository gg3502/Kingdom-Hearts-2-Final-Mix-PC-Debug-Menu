#pragma once
#include "EffectBase.hpp"
#include <OBJENRTY_0.hpp>

class ChangePlayerForm : public EffectBase {
public:
    ChangePlayerForm(OBJENTRY_ID formId);

    void Activate() override;
    std::string GetName() const override;

private:
    int m_formId;
};


