#pragma once

#include "DetectorDescription.h"
#include "Trigger.h"
#include "json.hpp"
#include <cstdint>

class TDCConfigReader
{
  public:
    TDCConfigReader() = default;

    void readTDCConfig(const nlohmann::json& jsonConfig);

    inline auto getDetectorDescriptionMap() const { return detectorDescriptionMap; }

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

    short   triggerID{-1};
    Trigger trigger{};
};