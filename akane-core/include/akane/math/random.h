#pragma once
#include "akane/math/math.h"
#include <random>

namespace akane
{
    // This is an implementation of xoshiro256+ an all-purpose, rock-solid
    // PRNG with sub-ns speed and 256-bit state that is large enough for any
    // parallel application. NOTE the state must be seeded so that it is not
    // everywhere zero.
    //
    // Reference: http://xoshiro.di.unimi.it/
    class RandomEngine final
    {
    public:
        RandomEngine(uint64_t seed = 13579u)
        {
            Seed(seed);
        }

        void Seed(uint64_t value) noexcept
        {
            auto s0 = static_cast<uint32_t>(value >> 32);
            auto s1 = static_cast<uint32_t>(value);

            std::seed_seq seq{s0, s1};
            auto s_begin = reinterpret_cast<uint32_t*>(s);
            auto s_end   = s_begin + 8;
            seq.generate(s_begin, s_end);
        }

        uint64_t Next() noexcept
        {
            // const uint64_t result_starstar = RotateLeft(s[1] * 5, 7) * 9;
            const uint64_t result_plus = s[0] + s[3];

            const uint64_t t = s[1] << 17;

            s[2] ^= s[0];
            s[3] ^= s[1];
            s[1] ^= s[2];
            s[0] ^= s[3];

            s[2] ^= t;

            s[3] = RotateLeft(s[3], 45);

            return result_plus;
        }

    private:
        uint64_t RotateLeft(const uint64_t x, int k) noexcept
        {
            return (x << k) | (x >> (64 - k));
        }

        uint64_t s[4]; // engine state
    };

    /**
     * Uniformly samples a float in [0, 1)
     *
     * @param engine PRNG that sources entropy
     */
    inline float SampleUniformFloat(RandomEngine& engine)
    {
        uint32_t u = static_cast<uint32_t>(engine.Next() >> (64 - 23));
        u |= 0x7fu << 23;

        // bit_cast here assume the same layout and alignments between **uint32_t** and
        // **float**
        return *reinterpret_cast<float*>(&u) - 1.f;

        // constexpr uint64_t mask = 0x7ffff;
        // constexpr float unit = 1 / static_cast<float>(1 + mask);

        // return static_cast<float>(engine.Next() & mask) * unit;
    }

    /**
     * Uniformly samples a point in unit squere where both x and y are in [0, 1)
     *
     * @param engine PRNG that sources entropy
     */
    inline Point2f SampleUniformFloat2D(RandomEngine& engine)
    {
        return Point2f{SampleUniformFloat(engine), SampleUniformFloat(engine)};
    }

    /**
     * Samples a boolean value
     *
     * @param engine PRNG that sources entropy
     * @param prob Probability that **true** is returned
     */
    inline float SampleBool(RandomEngine& engine, float prob)
    {
        return SampleUniformFloat(engine) < prob;
    }

} // namespace akane