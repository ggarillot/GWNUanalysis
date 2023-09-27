#pragma once

#include <CLHEP/Vector/ThreeVector.h>
#include <json.hpp>

#include "Cluster.h"
#include "DetectorCalibration.h"
#include "DetectorGeometry.h"
#include "Hit.h"
#include "Strip.h"
#include "json.hpp"

class DetectorDescription
{
  public:
    DetectorDescription(const std::string& _name = "");

    HitPtr getHit(const StripPtr strip) const;
    HitPtr getHit(const ClusterPtr cluster) const;

    inline const std::string& getName() const { return name; }
    inline const auto&        getTimeLimits() const { return timeLimits; }

    inline const DetectorGeometry&    getGeometry() const { return detectorGeometry; }
    inline const DetectorCalibration& getCalibration() const { return detectorCalibration; }

    inline void setTimeLimits(const std::array<uint32_t, 2>& tl) { timeLimits = tl; }

    inline void setGeometry(const DetectorGeometry& detGeom) { detectorGeometry = detGeom; }
    inline void setCalibration(const DetectorCalibration& detCalib) { detectorCalibration = detCalib; }

  protected:
    std::string             name{};
    std::array<uint32_t, 2> timeLimits{};

    DetectorGeometry    detectorGeometry{};
    DetectorCalibration detectorCalibration{};
};