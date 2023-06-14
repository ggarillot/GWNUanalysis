#pragma once

#include <memory>
#include <vector>

#include <CLHEP/Vector/ThreeVector.h>

class Hit
{
  public:
    Hit(const CLHEP::Hep3Vector& pos, const CLHEP::Hep3Vector& posErr, const double e, const double t)
        : position(pos)
        , positionError(posErr)
        , energy(e)
        , time(t)
    {
    }

    inline CLHEP::Hep3Vector getPosition() const { return position; }
    inline CLHEP::Hep3Vector getPositionError() const { return positionError; }
    inline double            getEnergy() const { return energy; }
    inline double            getTime() const { return time; }

    inline std::tuple<double, double, double> getPositionXYZ() const
    {
        return {position.x(), position.y(), position.z()};
    }
    inline std::tuple<double, double, double> getPositionErrorXYZ() const
    {
        return {positionError.x(), positionError.y(), positionError.z()};
    }

    void print() const
    {
        std::cout << "Hit : "
                  << "pos: " << position << ", posErr: " << positionError << std::endl;
    }

  protected:
    const CLHEP::Hep3Vector position{};
    const CLHEP::Hep3Vector positionError{};

    const double energy{};
    const double time{};
};

typedef std::shared_ptr<Hit> HitPtr;
typedef std::vector<HitPtr>  HitVec;