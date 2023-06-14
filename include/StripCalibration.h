#pragma once

#include <TF1.h>

#include <optional>

#include "Strip.h"

class StripCalibration
{
  public:
    void correctStrip(StripPtr strip, bool corrOffset = true, bool corrLR = true, bool corrTA = true) const;

    inline void setCalibTA(TF1 func) { calibTA = func; }
    inline void setStripOffset(double o) { stripOffset = o; }
    inline void setCorrLR(double c) { corrLR = c; }

    inline auto getCalibTA() const { return calibTA; }
    inline auto getStripOffset() const { return stripOffset; }
    inline auto getCorrLR() const { return corrLR; }

  protected:
    void correctStripOffset(StripPtr strip) const;
    void correctStripLR(StripPtr strip) const;
    void correctStripTA(StripPtr strip) const;

  protected:
    std::optional<TF1> calibTA = {};
    double             stripOffset = 0.;
    double             corrLR = 0.;
};