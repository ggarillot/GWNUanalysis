#include "Cluster.h"

#include <algorithm>

Cluster::Cluster(const StripVec& vec)
    : strips(vec)
{
    unsigned int highTot = 0;
    for (const auto& strip : strips)
    {
        if (strip->tot() > highTot)
        {
            highTot = strip->tot();
            highestTotStrip = strip;
        }
    }
}

StripVec Cluster::orderedStripsByTimeAsc() const
{
    StripVec stripsSorted = strips;

    auto sort = [](const std::shared_ptr<Stripp> a, const std::shared_ptr<Stripp> b)
    { return std::forward_as_tuple(a->time(), -a->tot()) < std::forward_as_tuple(b->time(), -b->tot()); };

    std::sort(stripsSorted.begin(), stripsSorted.end(), sort);

    return stripsSorted;
}

StripVec Cluster::orderedStripsByTimeDesc() const
{
    StripVec stripsSorted = strips;

    auto sort = [](const std::shared_ptr<Stripp> a, const std::shared_ptr<Stripp> b) { return a->time() > b->time(); };

    std::sort(stripsSorted.begin(), stripsSorted.end(), sort);

    return stripsSorted;
}

StripVec Cluster::orderedStripsByTotAsc() const
{
    StripVec stripsSorted = strips;

    auto sort = [](const std::shared_ptr<Stripp> a, const std::shared_ptr<Stripp> b) { return a->tot() < b->tot(); };

    std::sort(stripsSorted.begin(), stripsSorted.end(), sort);

    return stripsSorted;
}

StripVec Cluster::orderedStripsByTotDesc() const
{
    StripVec stripsSorted = strips;

    auto sort = [](const std::shared_ptr<Stripp> a, const std::shared_ptr<Stripp> b)
    { return std::forward_as_tuple(a->tot(), -a->time()) > std::forward_as_tuple(b->tot(), -b->time()); };

    std::sort(stripsSorted.begin(), stripsSorted.end(), sort);

    return stripsSorted;
}