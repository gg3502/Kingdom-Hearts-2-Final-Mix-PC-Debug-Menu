#pragma once
#include <string>

struct Caption {
    std::string entityName;       // Actual in-game name of the entity
    std::string displayEntityName; // Custom display name for the caption
    std::string message;
    float displayTime;
    float timeRemaining;
    int animID;
};