#pragma once
#include "akane/math/math.h"
#include "akane/math/random.h"
#include <memory>

namespace akane
{
    class Sampler
    {
    public:
        using Ptr = std::unique_ptr<Sampler>;

        Sampler(uint64_t seed)
        {
            engine_.Seed(seed);
        }
        // virtual ~Sampler() = default;

        // get a sample of x where x is in [0, 1)
        float Get1D()
        {
            return SampleUniformFloat(engine_);
        }
        // get a sample of (x, y) where both x, y are in [0, 1)
        Point2f Get2D()
        {
            return SampleUniformFloat2D(engine_);
        }

    private:
        RandomEngine engine_;
    };

    inline Sampler::Ptr CreateRandomSampler(uint64_t seed)
    {
        return std::make_unique<Sampler>(seed);
    }
} // namespace akane