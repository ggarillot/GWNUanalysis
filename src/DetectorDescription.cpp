#include "DetectorDescription.h"
#include <cstdint>
#include <limits>
#include <memory>

DetectorDescription::DetectorDescription(const std::string& _name)
    : name(_name)
{
    timeLimits[0] = std::numeric_limits<uint32_t>::min();
    timeLimits[1] = std::numeric_limits<uint32_t>::max();
}

HitPtr DetectorDescription::getHit(const StripPtr strip) const
{
    // TODO not sure if need to read position directly from strip or from detector geometry

    const auto stripPosition = strip->position;

    const auto detectorPosition = detectorGeometry.getPosition();
    const auto detectorOrientation = detectorGeometry.getOrientation();

    const auto errStripWidth = detectorGeometry.getStripWidth() / std::sqrt(12);

    const auto [positionAlongStrip, errorAlongStrip] = detectorCalibration.getPositionAlongStrip(strip);

    const auto hitPosition = stripPosition + positionAlongStrip * detectorOrientation.orthogonal();

    auto hitError = detectorOrientation * errStripWidth + detectorOrientation.orthogonal() * errorAlongStrip;
    hitError.set(std::abs(hitError.x()), std::abs(hitError.y()), std::abs(hitError.z()));

    auto hit = std::make_shared<Hit>(hitPosition, hitError, strip->tot(), strip->time());

    // std::cout << "GetHit : \n"
    //           << "      det  ID: " << strip->detectorID << "\n"
    //           << "      StripID: " << strip->ID << " , pos: " << stripPosition << "\n"
    //           << "      Hit pos: " << hit->getPosition() << "\n";

    return hit;
}

HitPtr DetectorDescription::getHit(const ClusterPtr cluster) const
{
    const auto strip = cluster->orderedStripsByTotDesc().front();
    return getHit(strip);
}