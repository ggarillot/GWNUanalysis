#include "TDCConfigReader.h"

#include <cstdint>
#include <iostream>
#include <limits>
#include <set>

#include "DetectorDescription.h"

void TDCConfigReader::readTDCConfig(const nlohmann::json& configJson)
{
    trigger = {};
    triggerID = -1;

    detectorDescriptionMap.clear();

    std::cout << "Reading TDC config..." << std::endl;

    std::set<short> tdcIDSet{};

    if (configJson.contains("trigger"))
    {
        const auto& triggerJson = configJson.at("trigger");
        triggerID = triggerJson.at("id").get<short>();
        std::cout << "Trigger found : TDC " << triggerID << std::endl;

        const auto channels = triggerJson.at("channels").get<unsigned int>();
        const auto formula = triggerJson.at("formula").get<std::vector<short>>();
        const auto checkFor = triggerJson.at("checkFor").get<std::vector<short>>();

        if (formula.size() != channels || checkFor.size() != channels)
            throw std::logic_error("ERROR : Trigger configuration problem : incorrect number of channels");

        trigger.nChannels = channels;
        trigger.checkFor = checkFor;
        trigger.formula = formula;

        if (triggerJson.contains("timeLimits"))
        {
            const auto timeLimits = triggerJson.at("timeLimits").get<std::array<uint32_t, 2>>();
            trigger.timeLimits = timeLimits;
        }
    }

    tdcIDSet.insert(triggerID);

    std::set<std::string> detNamesSet{};

    const auto detectorsJson = configJson.at("detectors");

    for (const auto& detectorJson : detectorsJson)
    {
        const auto tdcID = detectorJson.at("id").get<short>();

        // insert and check for duplicates TDCs
        if (!tdcIDSet.insert(tdcID).second)
            throw std::logic_error("ERROR : TDC " + std::to_string(tdcID) + " appears two times");

        const auto name = detectorJson.at("name").get<std::string>();

        std::cout << "Detector " << name << " found : TDC " << tdcID << std::endl;

        // insert and check for duplicates detector names
        if (!detNamesSet.insert(name).second)
            throw std::logic_error("ERROR : Detector " + name + " appears two times");

        auto detectorDescription = DetectorDescription(name);

        const auto detectorGeometry = DetectorGeometry(detectorJson.at("geometry"));
        detectorDescription.setGeometry(detectorGeometry);

        if (detectorJson.contains("timeLimits"))
        {
            const auto timeLimits = detectorJson.at("timeLimits").get<std::array<uint32_t, 2>>();
            detectorDescription.setTimeLimits(timeLimits);
        }

        detectorDescriptionMap.insert({tdcID, detectorDescription});
    }
}