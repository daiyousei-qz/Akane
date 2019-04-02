#include "akane/sampler.h"
#include "akane/common/random.h"
#include <memory>

using namespace std;

namespace akane
{
    class RandomSampler : public Sampler
    {
    public:
        akFloat Get1D() override
        {
            return rnd_.UniformReal();
        }
        Point2f Get2D() override
        {
            return {rnd_.UniformReal(), rnd_.UniformReal()};
        }

    private:
        RandomSource rnd_;
    };

    unique_ptr<Sampler> CreateRandomSampler()
    {
        return make_unique<RandomSampler>();
    }
} // namespace akane