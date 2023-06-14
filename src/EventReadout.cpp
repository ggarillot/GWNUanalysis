#include "EventReadout.h"

EventReadout::EventReadout(unsigned short nTDC)
    : nExpectedTDC(nTDC)
{
}

EventReadout::Status EventReadout::addTDCReadout(short tdcID, TDCReadout tdcReadout)
{
    tdcReadouts[tdcID] = tdcReadout;

    if (tdcReadouts.size() >= nExpectedTDC)
        return kOK;

    return kIncomplete;
}
