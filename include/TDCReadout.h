#pragma once

#include <array>
#include <cstdint>
#include <map>
#include <stdexcept>

class TDCReadout
{
  public:
    enum LeadOrTrail
    {
        kLeading,
        kTrailing
    };

    inline void fill(uint32_t channel, uint32_t time, LeadOrTrail leadorTrail)
    {
        if (channel > 32)
            throw std::logic_error("channel can not be greater than 32");

        channelReadouts[channel][time] = leadorTrail;
    }

    inline const std::array<std::map<uint32_t, LeadOrTrail>, 32>& getChannelReadouts() const
    {
        return channelReadouts;
    };

    short readChannel(short channel, unsigned int& leadingTime, unsigned int& tot) const
    {
        if (channel >= 32)
            return 0;

        auto chanReadout = channelReadouts[channel];

        tot = 0;

        if (chanReadout.empty())
            return 0;

        const auto [lT, leading] = *chanReadout.begin();

        if (leading != LeadOrTrail::kLeading)
            return 1;

        leadingTime = lT;

        chanReadout.erase(chanReadout.begin());

        if (chanReadout.empty())
            return 2;

        const auto [tT, trailing] = *chanReadout.begin();

        if (trailing != LeadOrTrail::kTrailing)
            return 3;

        tot = tT - lT;

        return 4;
    };

    inline uint32_t getTriggerTime() const { return triggerTime; }

    inline void setTriggerTime(const uint32_t time) { triggerTime = time; }

  protected:
    std::array<std::map<uint32_t, LeadOrTrail>, 32> channelReadouts{};
    uint32_t                                        triggerTime{};
};