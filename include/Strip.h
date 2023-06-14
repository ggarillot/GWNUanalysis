#pragma once

#include <cmath>
#include <memory>
#include <vector>

#include <CLHEP/Vector/ThreeVector.h>

class Cluster;

class Stripp
{
  public:
    Stripp() = default;

    Stripp(short id)
        : ID(id)
    {
    }

    Stripp(short id, short dId, int tL, int tR, int totl = 0, int totr = 0);

    void setTimesAndTots(double _timeL, double _timeR, double _totL, double _totR);

    double deltaTime() const { return timeL - timeR; }
    double time() const { return 0.5 * (timeL + timeR); }
    double minTime() const { return std::min(timeL, timeR); }
    double maxTime() const { return std::max(timeL, timeR); }

    double deltaTot() const { return totL - totR; }
    double tot() const { return 0.5 * (totL + totR); }
    double minTot() const { return std::min(totL, totR); }
    double maxTot() const { return std::max(totL, totR); }

    Stripp(const Stripp& toCopy) = delete;
    void operator=(const Stripp& toCopy) = delete;

  public:
    short ID = -1;
    // short  detectorID = -1;
    double timeL{};
    double timeR{};
    double totL{};
    double totR{};

    CLHEP::Hep3Vector position{};

  protected:
};

typedef std::shared_ptr<Stripp> StripPtr;
typedef std::vector<StripPtr>   StripVec;