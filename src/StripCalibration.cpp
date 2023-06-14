#include "StripCalibration.h"

#include <map>

#include "Strip.h"

void StripCalibration::correctStrip(StripPtr strip, bool corrOffset, bool corrLR, bool corrTA) const
{
    if (corrOffset)
        correctStripOffset(strip);

    if (corrLR)
        correctStripLR(strip);

    if (corrTA)
        correctStripTA(strip);
}

void StripCalibration::correctStripOffset(StripPtr strip) const
{
    strip->timeL += stripOffset;
    strip->timeR += stripOffset;
}

void StripCalibration::correctStripLR(StripPtr strip) const
{
    strip->timeL -= 0.5 * corrLR;
    strip->timeR += 0.5 * corrLR;
}

void StripCalibration::correctStripTA(StripPtr strip) const
{
    if (!calibTA.has_value())
        return;

    const auto func = calibTA.value();

    const auto tot = strip->tot();

    double corr = 0;

    const auto xMin = func.GetXmin();
    const auto xMax = func.GetXmax();

    if (tot <= xMin)
        corr = func.Eval(xMin) - func.Eval(xMax);
    else if (tot > xMin && tot < xMax)
        corr = func.Eval(tot) - func.Eval(xMax);

    strip->timeL -= corr;
    strip->timeR -= corr;
}
