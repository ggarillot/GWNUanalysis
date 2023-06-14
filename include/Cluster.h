#pragma once

#include "Strip.h"
#include <memory>

class Cluster
{
  public:
    Cluster(const StripVec& vec);
    ~Cluster() = default;

    const auto& getStrips() const { return strips; };

    StripVec orderedStripsByTimeAsc() const;
    StripVec orderedStripsByTimeDesc() const;

    StripVec orderedStripsByTotAsc() const;
    StripVec orderedStripsByTotDesc() const;

    auto time() const { return highestTotStrip->time(); }
    auto tot() const { return highestTotStrip->tot(); }

  protected:
    const StripVec strips{};

    std::shared_ptr<Stripp> highestTotStrip{};
};

typedef std::shared_ptr<Cluster> ClusterPtr;
typedef std::vector<ClusterPtr>  ClusterVec;