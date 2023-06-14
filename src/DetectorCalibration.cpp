#include "DetectorCalibration.h"
#include "Globals.h"
#include "StripCalibration.h"

#include <Strip.h>
#include <stdexcept>

std::map<std::string, DetectorCalibration> DetectorCalibration::initFromJson(const nlohmann::json& jsonCalibration)
{
    std::map<std::string, DetectorCalibration> detectorCalibrationMap{};

    const auto detectorsCalibrationJson = jsonCalibration.at("calib");

    for (const auto& [name, detectorCalibrationJson] : detectorsCalibrationJson.items())
    {
        std::cout << "Reading calibration for detector " << name << "..." << std::endl;

        const auto detectorCalibration = DetectorCalibration::initCalibFromJson(detectorCalibrationJson);
        detectorCalibrationMap.insert({name, detectorCalibration});

        std::cout << std::endl;
    }

    return detectorCalibrationMap;
}

DetectorCalibration DetectorCalibration::initCalibFromJson(const nlohmann::json& jsonCalibration)
{
    DetectorCalibration detectorCalibration{};

    if (!jsonCalibration.contains("slope"))
        std::cout << "WARNING : no slope calibration" << std::endl;
    else
        detectorCalibration.diffLRToPosition = jsonCalibration.at("slope").get<double>();

    if (!jsonCalibration.contains("offset"))
        std::cout << "WARNING : no global offset calibration" << std::endl;
    else
        detectorCalibration.globalOffset = jsonCalibration.at("offset").get<double>();

    if (!jsonCalibration.contains("precision"))
    {
        std::cout << "WARNING : no global precision, will put 11mm as default" << std::endl;
        detectorCalibration.stripPrecisionMap.insert({-1, 11.});
    }
    else
        detectorCalibration.stripPrecisionMap.insert({-1, jsonCalibration.at("precision").get<double>()});

    if (!jsonCalibration.contains("strips"))
    {
        std::cout << "WARNING : no strips calibration" << std::endl;
        return detectorCalibration;
    }

    const auto jsonStrips = jsonCalibration.at("strips");

    for (const auto& [stripIDstr, jsonStripCalibration] : jsonStrips.items())
    {
        const short stripID = std::stoi(stripIDstr);

        StripCalibration stripCalibration{};

        if (jsonStripCalibration.contains("TAcorr"))
        {
            const auto taCorrJson = jsonStripCalibration.at("TAcorr");

            const auto formula = taCorrJson.at("formula").get<std::string>();
            const auto params = taCorrJson.at("params").get<std::vector<double>>();
            const auto range = taCorrJson.at("range").get<std::array<double, 2>>();

            auto func = TF1(Globals::getNewName().c_str(), formula.c_str(), range[0], range[1]);

            for (auto i = 0U; i < params.size(); ++i)
                func.SetParameter(i, params[i]);

            func.SetLineColor(kMagenta);

            stripCalibration.setCalibTA(func);
        }
        else
            std::cout << "WARNING : no TA correction for strip " << stripID << std::endl;

        if (jsonStripCalibration.contains("offset"))
            stripCalibration.setStripOffset(jsonStripCalibration.at("offset").get<double>());
        else
            std::cout << "WARNING : no offset for strip " << stripID << std::endl;

        if (jsonStripCalibration.contains("LRoffset"))
            stripCalibration.setCorrLR(jsonStripCalibration.at("LRoffset").get<double>());
        else
            std::cout << "WARNING : no LR offset for strip " << stripID << std::endl;

        detectorCalibration.stripCalibrationMap.insert({stripID, stripCalibration});

        if (!jsonStripCalibration.contains("precision"))
        {
            std::cout << "WARNING : no strips precision for strip " << stripID << ", will use detector default "
                      << detectorCalibration.stripPrecisionMap.at(-1) << "mm" << std::endl;
        }
        else
            detectorCalibration.stripPrecisionMap.insert({stripID, jsonStripCalibration.at("precision").get<double>()});
    }

    return detectorCalibration;
}

void DetectorCalibration::correctStrip(StripPtr strip, bool corrOffset, bool corrLR, bool corrTA) const
{
    const auto it = stripCalibrationMap.find(strip->ID);
    if (it == stripCalibrationMap.end())
        return;

    it->second.correctStrip(strip, corrOffset, corrLR, corrTA);

    if (corrOffset)
    {
        strip->timeL += globalOffset;
        strip->timeR += globalOffset;
    }
}

void DetectorCalibration::correctCluster(ClusterPtr cluster, bool corrOffset, bool corrLR, bool corrTA) const
{
    for (auto& strip : cluster->getStrips())
        correctStrip(strip, corrOffset, corrLR, false);

    auto bestStrip = cluster->orderedStripsByTotDesc().front();
    correctStrip(bestStrip, false, false, corrTA);
}

std::array<double, 2> DetectorCalibration::getPositionAlongStrip(const StripPtr strip) const
{
    double     error = 0.;
    const auto it = stripPrecisionMap.find(strip->ID);

    if (it == stripPrecisionMap.end())
        error = stripPrecisionMap.at(-1);
    else
        error = it->second;

    const double posAlongStrip = strip->deltaTime() * diffLRToPosition;

    return {posAlongStrip, error};
}

const StripCalibration* DetectorCalibration::getStripCalibration(unsigned short stripID) const
{
    const StripCalibration* stripCalibration = nullptr;

    if (stripCalibrationMap.find(stripID) != stripCalibrationMap.end())
        stripCalibration = &stripCalibrationMap.at(stripID);

    return stripCalibration;
}