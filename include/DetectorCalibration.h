#pragma once

#include <map>

#include <CLHEP/Vector/ThreeVector.h>

#include "Cluster.h"
#include "Hit.h"
#include "Strip.h"
#include "StripCalibration.h"
#include "json.hpp"

class DetectorCalibration
{
  public:
    static std::map<std::string, DetectorCalibration> initFromJson(const nlohmann::json& jsonCalibration);
    static DetectorCalibration                        initCalibFromJson(const nlohmann::json& jsonCalibration);

  public:
    DetectorCalibration() = default;

    void correctStrip(StripPtr strip, bool corrOffset = true, bool corrLR = true, bool corrTA = true) const;
    void correctCluster(ClusterPtr cluster, bool corrOffset = true, bool corrLR = true, bool corrTA = true) const;

    std::array<double, 2> getPositionAlongStrip(const StripPtr strip) const;

    inline double getdiffLRToPosition() const { return diffLRToPosition; }

    const StripCalibration* getStripCalibration(unsigned short stripID) const;

  protected:
    double globalOffset{};

    double diffLRToPosition{};

    std::map<unsigned short, StripCalibration> stripCalibrationMap{};

    std::map<short, double> stripPrecisionMap{};
};