#include "DataFileReader.h"
#include "EventReadout.h"
#include "TDCConfigReader.h"
#include "TDCReadout.h"

#include "json.hpp"

#include <TClass.h>
#include <TFile.h>
#include <bitset>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <limits>
#include <ostream>
#include <stdexcept>
#include <type_traits>

DataFileReader::DataFileReader(std::ostream* stream)
    : debugStream(stream)
{
}

void DataFileReader::openOutputFile(std::string name)
{
    if (outputFile)
        throw std::logic_error("ERROR : file already opened");

    outputFile = TFile::Open(name.c_str(), "RECREATE");

    tree = new TTree("tree", "tree");
    tree->Branch("event", &eventID);
    tree->Branch("sync", &sync);

    tree->Branch("eventTime", &eventTime);

    tree->Branch("triggerTimes", &triggerTimes);
    tree->Branch("triggerTots", &triggerTots);
    tree->Branch("triggerFlags", &triggerFlags);

    tree->Branch("detector", &detector);
    tree->Branch("strip", &strip);
    tree->Branch("flag", &flagVec);
    tree->Branch("timeL", &timeL);
    tree->Branch("timeR", &timeR);
    tree->Branch("totL", &totL);
    tree->Branch("totR", &totR);
    tree->Branch("isHigh", &isHigh);
}

void DataFileReader::closeOutputFile()
{
    if (!outputFile)
        throw std::logic_error("ERROR : no file curently opened");

    tree->Write("tree");
    outputFile->Close();
    delete outputFile;
    outputFile = nullptr;
}

void DataFileReader::readTDCConfig(const nlohmann::json& jsonConfig)
{
    TDCConfigReader reader{};
    reader.readTDCConfig(jsonConfig);

    detectorDescriptionMap = reader.getDetectorDescriptionMap();
    triggerID = reader.getTriggerID();
    nTriggerChannels = reader.getTrigger().nChannels;

    // nTDC = reader.nTDC();
}

void DataFileReader::readDataFile(std::fstream& dataFile)
{
    if (!dataFile.is_open())
        throw std::logic_error("ERROR : file not open");

    auto buffer = std::vector<char>();
    buffer.resize(4);

    dataFile.read(buffer.data(), buffer.size());

    std::string str1290(buffer.rbegin(), buffer.rend());
    // std::cout << str1290 << std::endl;

    if (str1290 != "1290")
        throw std::logic_error("ERROR : not 1290");

    dataFile.read(buffer.data(), buffer.size());

    std::string strData(buffer.rbegin(), buffer.rend());
    // std::cout << strData << std::endl;

    if (strData != "Data")
        throw std::logic_error("ERROR : not Data");

    while (!dataFile.eof())
    {
        buffer.resize(4);
        uint32_t lenBlock;
        dataFile.read(buffer.data(), buffer.size());
        std::memcpy(&lenBlock, buffer.data(), sizeof(int));

        if (debugStream)
            *debugStream << "lenBlock : " << lenBlock << std::endl;

        uint32_t tdcType;
        dataFile.read(buffer.data(), buffer.size());
        std::memcpy(&tdcType, buffer.data(), sizeof(int));

        if (debugStream)
            *debugStream << "tdcType : " << tdcType << std::endl;

        if (lenBlock == 0)
            continue;

        buffer.resize(4 * (lenBlock - 2));
        dataFile.read(buffer.data(), buffer.size());

        if (tdcType == 0)
        {
            readHeader(buffer);
        }
        else if (tdcType >= 5)
        {
            tdcType -= 5;

            if (detectorDescriptionMap.find(tdcType) == detectorDescriptionMap.end() && tdcType != triggerID)
                continue;

            readTDC(buffer, tdcType);
        }

        if (debugStream)
            *debugStream << std::endl;
    }

    eventReadouts.clear();
}

void DataFileReader::readHeader(std::vector<char> chars)
{
    uint32_t hour, minute, sec;

    std::memcpy(&hour, chars.data(), 4);
    std::memcpy(&minute, chars.data() + 4, 4);
    std::memcpy(&sec, chars.data() + 8, 4);

    if (debugStream)
        *debugStream << "Time : " << hour << "h " << minute << "m " << sec << "s" << std::endl;

    uint32_t edgeDetectionMode, triggerTimeSubtraction;
    int32_t  windowWidth, windowOffset, searchMargin, rejectMargin, nAnnouncedTDC;

    std::memcpy(&edgeDetectionMode, chars.data() + 12, 4);
    std::memcpy(&windowWidth, chars.data() + 16, 4);
    std::memcpy(&windowOffset, chars.data() + 20, 4);
    std::memcpy(&searchMargin, chars.data() + 24, 4);
    std::memcpy(&rejectMargin, chars.data() + 28, 4);
    std::memcpy(&triggerTimeSubtraction, chars.data() + 32, 4);
    std::memcpy(&nAnnouncedTDC, chars.data() + 36, 4);

    if (debugStream)
        *debugStream << "Window width : " << windowWidth << ", Window offset : " << windowOffset << "\n"
                     << "Search margin : " << searchMargin << ", Reject margin : " << rejectMargin << "\n"
                     << "Edge detection mode : " << edgeDetectionMode << "\n"
                     << "Trigger time substraction : " << triggerTimeSubtraction << "\n"
                     << "No. of announced TDC : " << nAnnouncedTDC << std::endl;

    uint32_t nWorkingTDC = 0;

    for (auto i = 0; i < nAnnouncedTDC; i++)
    {
        int32_t isTDC;
        std::memcpy(&isTDC, chars.data() + 40 + i * 4, 4);
        if (debugStream)
            *debugStream << "TDC-" << i << " : " << isTDC << std::endl;

        if (!isTDC)
            continue;

        nWorkingTDC++;

        if (detectorDescriptionMap.find(i) == detectorDescriptionMap.end() && i != triggerID)
            std::cout << "WARNING : TDC in file not present in geometry : TDC " << i << std::endl;
    }

    if (nWorkingTDC != (detectorDescriptionMap.size() + (triggerID != -1)))
        std::cout << "WARNING : TDC in file and TDC in config are not the same" << std::endl;

    unsigned int nLeftLines = chars.size() / 4 - 11 - nAnnouncedTDC;

    for (auto i = 0U; i < nLeftLines + 1; ++i)
    {
        for (int j = 3; j >= 0; --j)
        {
            if (debugStream)
                *debugStream << chars[40 + nAnnouncedTDC * 4 + i * 4 + j];
        }
    }
    if (debugStream)
        *debugStream << std::endl;
}

void DataFileReader::readTDC(std::vector<char> chars, uint32_t tdcID)
{
    if (chars.empty())
        return;

    TDCReadout tdcReadout;
    uint32_t   eventNumber = -1;

    for (auto i = 0U; i < chars.size() / 4; ++i)
    {
        int32_t value;
        std::memcpy(&value, chars.data() + 4 * i, sizeof(value));

        if (debugStream)
            *debugStream << std::bitset<32>(value) << " : ";

        uint32_t globalType = (value & GLOBAL_MASK) >> 30;
        uint32_t dataType = (value & DATA_TYPE_MASK);

        if (globalType != 0)
        {
            if (dataType == GlobalHeader)
            {
                eventNumber = (value & GLOBAL_HEADER_EVENT_COUNT) >> 5;
                uint32_t global_header_geo = value & GLOBAL_HEADER_GEO;

                if (debugStream)
                    *debugStream << "GlobalHeader : event " << eventNumber << ", headerGeo : " << global_header_geo
                                 << std::endl;
            }
            else if (dataType == GlobalTriggerTimeTag)
            {
                uint32_t triggerTimeTag = (value & EXTENDED_TRIGGER_TIME);
                if (debugStream)
                    *debugStream << "GlobalTriggerTimeTag : " << triggerTimeTag << std::endl;
                tdcReadout.setTriggerTime(triggerTimeTag);
            }
            else if (dataType == GlobalTrailer)
            {
                uint32_t global_word_count = (value & GLOBAL_TRAILER_WORD_COUNT) >> 5;
                uint32_t global_trailer_geo = value & GLOBAL_HEADER_GEO;

                if (debugStream)
                    *debugStream << "GlobalTrailer : " << global_word_count << ", headerGeo : " << global_trailer_geo
                                 << std::endl;
            }
            else if (dataType == Filler)
            {
                if (debugStream)
                    *debugStream << "FILLER" << std::endl;
            }
        }
        else
        {
            if (dataType == TdcHeader)
            {
                uint32_t tdc = (value & TDC_ID_MASK) >> 24;
                uint32_t eventID = (value & EVENT_ID_MASK) >> 12;
                uint32_t bunchID = value & BUNCH_ID_MASK;

                if (debugStream)
                    *debugStream << "TDCHeader : tdc : " << tdc << ", eventID : " << eventID
                                 << ", bunchID : " << bunchID << std::endl;
            }
            else if (dataType == TdcMeasurement)
            {
                uint32_t flagLeadingTrailing = (value & DATA_LEADING_TRAILING_MASK) >> 26;
                uint32_t time = value & DATA_VALUE_MASK; // 0x001FFFFF 21 bits time data
                uint32_t channelID = (value & DATA_CHANNEL_MASK) >> 21;

                if (tdcID == 0)
                    time += 1000;

                if (debugStream)
                    *debugStream << "TDCMeasurement : leadTrail : " << flagLeadingTrailing << ", Chan : " << channelID
                                 << ", Value : " << time << std::endl;

                if (flagLeadingTrailing == 0)
                    tdcReadout.fill(channelID, time, TDCReadout::kLeading);
                else if (flagLeadingTrailing == 1)
                    tdcReadout.fill(channelID, time, TDCReadout::kTrailing);
            }
            else if (dataType == TdcError)
            {
                if (debugStream)
                    *debugStream << "TDCError" << std::endl;
            }
            else if (dataType == TdcTrailer)
            {
                uint32_t tdc_trailer_word_count = (value & TDC_TRAILER_WORD_COUNT);
                if (debugStream)
                    *debugStream << "TDCTrailer : " << tdc_trailer_word_count << std::endl;
            }
            else
            {
                if (debugStream)
                    *debugStream << "wrong! dataType=" << std::hex << dataType << std::dec << std::endl;
            }
        }
    }

    // TODO
    if (eventReadouts.find(eventNumber) == eventReadouts.end())
        eventReadouts[eventNumber] = EventReadout(detectorDescriptionMap.size() + (triggerID != -1));

    const auto status = eventReadouts[eventNumber].addTDCReadout(tdcID, tdcReadout);

    if (status == EventReadout::kOK)
        writeEvent(eventNumber);
}

void DataFileReader::writeEvent(uint32_t eventNumber)
{
    if (eventReadouts.find(eventNumber) == eventReadouts.end())
        throw std::logic_error("ERROR cannot write event : event not found");

    auto event = eventReadouts[eventNumber];
    eventReadouts.erase(eventNumber);

    eventID = eventNumber;

    sync = 0;

    triggerTimes.clear();
    triggerTots.clear();
    triggerFlags.clear();

    detector.clear();
    strip.clear();
    flagVec.clear();
    timeL.clear();
    timeR.clear();
    totL.clear();
    totR.clear();
    isHigh.clear();

    auto tdcReadouts = event.getTDCReadouts();

    // to get track of TDC unsynchronisation
    uint32_t minTDCTime = std::numeric_limits<uint32_t>::max();
    uint32_t maxTDCTime = std::numeric_limits<uint32_t>::min();

    for (const auto& [tdcID, tdcReadout] : tdcReadouts)
    {
        const auto thisTDCTime = tdcReadout.getTriggerTime();

        minTDCTime = std::min(minTDCTime, thisTDCTime);
        maxTDCTime = std::max(maxTDCTime, thisTDCTime);

        // handle trigger
        if (tdcID == triggerID)
        {
            triggerFlags = std::vector<short>(nTriggerChannels, 0);
            triggerTimes = std::vector<uint32_t>(nTriggerChannels, 0);
            triggerTots = std::vector<uint32_t>(nTriggerChannels, 0);

            for (auto iChan = 0U; iChan < nTriggerChannels; ++iChan)
            {
                uint32_t leadingTime{};
                uint32_t tot{};
                auto     flag = tdcReadout.readChannel(iChan, leadingTime, tot);

                triggerFlags[iChan] = flag;
                triggerTimes[iChan] = leadingTime;
                triggerTots[iChan] = tot;
            }
            continue;
        }

        const auto id = detectorDescriptionMap.find(tdcID);
        if (id == detectorDescriptionMap.end())
        {
            std::cout << "err : tdcID not present in geometry : " << tdcID << std::endl;
            continue;
        }

        // TODO maybe
        const auto nStrips = id->second.getGeometry().getNStrips();

        unsigned int maxTot = 0;
        short        posOfMax = -1;

        for (auto iStrip = 0U; iStrip < nStrips; iStrip++)
        {
            const auto chanL = iStrip;
            const auto chanR = iStrip + nStrips;

            unsigned int leadingTimeL = 0;
            unsigned int leadingTimeR = 0;
            unsigned int wL = 0;
            unsigned int wR = 0;

            auto flagL = tdcReadout.readChannel(chanL, leadingTimeL, wL);
            auto flagR = tdcReadout.readChannel(chanR, leadingTimeR, wR);

            if (flagL == 0 && flagR == 0)
                continue;

            short flag = std::min(flagL, flagR);

            detector.push_back(tdcID);
            strip.push_back(iStrip);
            flagVec.push_back(flag);
            timeL.push_back(leadingTimeL);
            totL.push_back(wL);
            timeR.push_back(leadingTimeR);
            totR.push_back(wR);

            // get info of max ToT Strip
            isHigh.push_back(false);
            if (flag != 4)
                continue;
            const auto tot = wL + wR;
            if (tot > maxTot)
            {
                maxTot = tot;
                posOfMax = isHigh.size() - 1;
            }
        } // loop on strips

        if (posOfMax > -1)
            isHigh[posOfMax] = true;

    } // loop on TDC

    eventTime = minTDCTime;
    sync = maxTDCTime - minTDCTime;

    tree->Fill();
}
