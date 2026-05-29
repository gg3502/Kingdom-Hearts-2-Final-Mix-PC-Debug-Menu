#pragma once
#include <string>

class EffectBase {
public:
    virtual void Activate() = 0;
    virtual std::string GetName() const = 0;
    virtual ~EffectBase() = default;
};
