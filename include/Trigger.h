#pragma once

#include <array>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <vector>

struct Trigger
{
    Trigger()
    {
        timeLimits[0] = std::numeric_limits<uint32_t>::min();
        timeLimits[1] = std::numeric_limits<uint32_t>::max();
    }

    std::optional<double> getTime() const
    {
        if (times.size() != nChannels || formula.size() != nChannels || checkFor.size() != nChannels ||
            flags.size() != nChannels || tots.size() != nChannels)
        {
            std::cout << "Warning : trigger problem" << std::endl;
            return {};
        }

        // check times
        for (auto i = 0U; i < nChannels; ++i)
        {
            if (checkFor[i] == false)
                continue;
            if (flags[i] < 2)
                return {};
        }

        // TODO check delta time between scintillators
        // int32_t deltaTime = times[1] - times[3];
        // if (deltaTime < 0 || deltaTime > 45)
        //     return {};

        // compute time
        double       time = 0.;
        unsigned int n = 0;
        for (auto i = 0U; i < nChannels; ++i)
        {
            if (formula[i] == false)
                continue;

            time += times[i];
            n++;
        }

        if (n == 0)
            return 0;

        return time / n;
    }

    std::array<uint32_t, 2> timeLimits{};

    unsigned int nChannels{};

    std::vector<short> checkFor{};
    std::vector<short> formula{};

    std::vector<unsigned int> times{};
    std::vector<unsigned int> tots{};
    std::vector<short>        flags{};
};