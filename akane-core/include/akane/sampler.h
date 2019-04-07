#pragma once
#include "akane/math/float_type.h"
#include "akane/math/vector.h"

namespace akane
{
    class Sampler
    {
    public:
        using Ptr = std::unique_ptr<Sampler>;

        Sampler()          = default;
        virtual ~Sampler() = default;

        // get a sample of x where x is in [0, 1)
        virtual akFloat Get1D() = 0;

        // get a sample of (x, y) where both x, y are in [0, 1)
        virtual Point2f Get2D() = 0;
    };

    Sampler::Ptr CreateRandomSampler();
} // namespace akane