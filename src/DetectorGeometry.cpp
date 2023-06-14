#include "DetectorGeometry.h"
#include <memory>

DetectorGeometry::DetectorGeometry(const nlohmann::json& jsonGeometry)
{
    initFromJson(jsonGeometry);
}

void DetectorGeometry::initFromJson(const nlohmann::json& jsonGeometry)
{
    if (!jsonGeometry.contains("type"))
        throw std::logic_error("Error : chamber does not have 'type'");

    if (!jsonGeometry.contains("pos"))
        throw std::logic_error("Error : chamber does not have 'pos'");

    if (!jsonGeometry.contains("nStrips"))
        throw std::logic_error("Error : chamber does not have 'nStrips'");

    if (!jsonGeometry.contains("stripWidth"))
        throw std::logic_error("Error : chamber does not have 'stripWidth'");

    const auto type = jsonGeometry.at("type").get<std::string>();

    if (type == "X")
        orientation = {1, 0, 0};
    else if (type == "Y")
        orientation = {0, 1, 0};

    const auto pos = jsonGeometry["pos"].get<std::array<double, 3>>();
    position = {pos[0], pos[1], pos[2]};

    nStrips = jsonGeometry["nStrips"].get<unsigned int>();

    stripWidth = jsonGeometry["stripWidth"].get<double>();
}

CLHEP::Hep3Vector DetectorGeometry::getStripPosition(const short iStrip) const
{
    if (iStrip >= nStrips)
        throw std::logic_error("Error in Detector geometry : strip number exceeds detector number of strips");

    const double stripAtCenter = 0.5 * (nStrips - 1);
    const double positionOfThisStrip = (iStrip - stripAtCenter) * stripWidth;

    return position + positionOfThisStrip * orientation;
}

StripPtr DetectorGeometry::constructStrip(const short iStrip) const
{
    if (iStrip >= nStrips)
        throw std::logic_error("Error in Detector geometry : strip number exceeds detector number of strips");

    auto strip = std::make_shared<Stripp>(iStrip);
    strip->position = getStripPosition(iStrip);

    return strip;
}