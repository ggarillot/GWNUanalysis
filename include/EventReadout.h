#pragma once

#include "TDCReadout.h"

#include <map>
#include <set>

class EventReadout
{
  public:
    enum Status
    {
        kIncomplete,
        kOK
    };

    EventReadout(unsigned short nTDC = 3);

    Status addTDCReadout(short tdcID, TDCReadout tdcReadout);

    inline std::map<short, TDCReadout> getTDCReadouts() const { return tdcReadouts; }

  protected:
    unsigned short nExpectedTDC{};

    std::map<short, TDCReadout> tdcReadouts{};
};