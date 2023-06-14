#pragma once

#include "DetectorDescription.h"
#include "Trigger.h"
#include "json.hpp"
#include <cstdint>

class TDCConfigReader
{
  public:
    TDCConfigReader();

    void readTDCConfig(const nlohmann::json& jsonConfig);

    inline auto getDetectorDescriptionMap() const { return detectorDescriptionMap; }
    inline auto getTimeLimits() const { return timeLimits; }

    inline auto getTriggerID() const { return triggerID; }
    inline auto getTrigger() const { return trigger; }

    inline unsigned short nTDC() const
    {
        unsigned short toReturn = 0;
        if (triggerID != -1)
            toReturn++;

        return detectorDescriptionMap.size() + toReturn;
    }

  protected:
    std::map<short, DetectorDescription> detectorDescriptionMap{};
    std::array<uint32_t, 2>              timeLimits{};

    short   triggerID{-1};
    Trigger trigger{};
};