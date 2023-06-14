#pragma once

#include "DetectorDescription.h"
#include "EventReadout.h"
#include "TDCReadout.h"
#include "json.hpp"

#include "TTree.h"

#include <CLHEP/Vector/ThreeVector.h>
#include <cstring>
#include <iostream>
#include <map>
#include <set>
#include <vector>

class DataFileReader
{
  public:
    DataFileReader(std::ostream* stream = nullptr);

    void openOutputFile(std::string name);
    void closeOutputFile();

    void readDataFile(std::fstream& fileStream);

    void readTDCConfig(const nlohmann::json& jsonConfig);

  protected:
    void readHeader(std::vector<char> chars);
    void readTDC(std::vector<char> chars, uint32_t tdcID);

    void writeEvent(uint32_t eventNumber);

  protected:
    std::map<int, CLHEP::Hep3Vector> geometry{};

    std::map<short, DetectorDescription> detectorDescriptionMap{};
    int                                  triggerID = -1;
    unsigned int                         nTriggerChannels{};

    uint32_t nTDC = 0;

    std::map<uint32_t, EventReadout> eventReadouts{};

    std::ostream* debugStream = nullptr;

    TFile* outputFile = nullptr;
    TTree* tree = nullptr;

    int          eventID{};
    unsigned int eventTime{};
    unsigned int sync{};

    std::vector<unsigned int> triggerTimes{};
    std::vector<unsigned int> triggerTots{};
    std::vector<short>        triggerFlags{};

    std::vector<short>        detector{};
    std::vector<short>        strip{};
    std::vector<short>        flagVec{};
    std::vector<unsigned int> timeL{};
    std::vector<unsigned int> timeR{};
    std::vector<unsigned int> totL{};
    std::vector<unsigned int> totR{};
    std::vector<bool>         isHigh{};

  public:
    // 1100 0000 0000 0000 0000 0000 0000 0000
    static const uint32_t GLOBAL_MASK = 0xC0000000;

    // 1111 1000 0000 0000 0000 0000 0000 0000
    static const uint32_t DATA_TYPE_MASK = 0xF8000000;

    // 0000 0100 0000 0000 0000 0000 0000 0000
    static const uint32_t DATA_LEADING_TRAILING_MASK = 0x04000000;

    // 0000 0011 1110 0000 0000 0000 0000 0000
    static const uint32_t DATA_CHANNEL_MASK = 0x03E00000;

    // 0000 0000 0001 1111 1111 1111 1111 1111
    static const uint32_t DATA_VALUE_MASK = 0x001FFFFF;

    // 0000 0111 1111 1111 1111 1111 1110 0000
    static const uint32_t GLOBAL_HEADER_EVENT_COUNT = 0x07FFFFE0;

    // 0000 0000 0000 0000 0000 0000 0001 1111
    static const uint32_t GLOBAL_HEADER_GEO = 0x0000001F;

    // 0000 0000 0001 1111 1111 1111 1110 0000
    static const uint32_t GLOBAL_TRAILER_WORD_COUNT = 0x001FFFE0;

    // 0000 0000 0000 0000 0000 1111 1111 1111
    static const uint32_t TDC_TRAILER_WORD_COUNT = 0x00000FFF;

    // 0000 0011 0000 0000 0000 0000 0000 0000
    static const uint32_t TDC_ID_MASK = 0x03000000;

    // 0000 0000 1111 1111 1111 0000 0000 0000
    static const uint32_t EVENT_ID_MASK = 0x00FFF000;

    // 0000 0000 0000 0000 0000 1111 1111 1111
    static const uint32_t BUNCH_ID_MASK = 0x00000FFF;

    // 0000 0111 1111 1111 1111 1111 1111 1111
    static const uint32_t EXTENDED_TRIGGER_TIME = 0x07FFFFFF;

    // 0100 0000 0000 0000 0000 0000 0000 0000
    static const uint32_t GlobalHeader = 0x40000000;

    // 1000 0000 0000 0000 0000 0000 0000 0000
    static const uint32_t GlobalTrailer = 0x80000000;

    // 1100 0000 0000 0000 0000 0000 0000 0000
    static const uint32_t Filler = 0xB0000000;

    // 1000 1000 0000 0000 0000 0000 0000 0000
    static const uint32_t GlobalTriggerTimeTag = 0x88000000;

    // 0000 1000 0000 0000 0000 0000 0000 0000
    static const uint32_t TdcHeader = 0x08000000;

    // 0000 0000 0000 0000 0000 0000 0000 0000
    static const uint32_t TdcMeasurement = 0x00000000;

    // 0001 1000 0000 0000 0000 0000 0000 0000
    static const uint32_t TdcTrailer = 0x18000000;

    // 0010 0000 0000 0000 0000 0000 0000 0000
    static const uint32_t TdcError = 0x20000000;
};