#include "Strip.h"

Stripp::Stripp(short id, short dId, int tL, int tR, int totl, int totr)
    : ID(id)
    // , detectorID(dId)
    , timeL(tL)
    , timeR(tR)
    , totL(totl)
    , totR(totr)
{
}

void Stripp::setTimesAndTots(double _timeL, double _timeR, double _totL, double _totR)
{
    timeL = _timeL;
    timeR = _timeR;

    totL = _totL;
    totR = _totR;
}