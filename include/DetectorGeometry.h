#pragma once

#include <CLHEP/Vector/ThreeVector.h>

#include "Strip.h"
#include "json.hpp"

class DetectorGeometry
{
  public:
    DetectorGeometry() = default;

    DetectorGeometry(const nlohmann::json& jsonGeometry);

    void initFromJson(const nlohmann::json& jsonGeometry);

    CLHEP::Hep3Vector getStripPosition(const short iStrip) const;

    StripPtr constructStrip(const short iStrip) const;

    inline CLHEP::Hep3Vector getPosition() const { return position; }
    inline CLHEP::Hep3Vector getOrientation() const { return orientation; }

    inline double       getStripWidth() const { return stripWidth; }
    inline unsigned int getNStrips() const { return nStrips; }

  protected:
    CLHEP::Hep3Vector position{};
    CLHEP::Hep3Vector orientation{1, 0, 0};

    double       stripWidth{};
    unsigned int nStrips{};
};