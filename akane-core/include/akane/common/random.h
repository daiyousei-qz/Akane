#pragma once
#include "akane/math/float_type.h"
#include <random>

namespace akane
{
    // This is an implementation of xoshiro256** an all-purpose, rock-solid
    // PRNG with sub-ns speed and 256-bit state that is large enough for any
    // parallel application. NOTE the state must be seeded so that it is not
    // everywhere zero.
    //
    // Reference:http://xoshiro.di.unimi.it/
    class RandomEngine final
    {
    public:
        RandomEngine()
        {
            Seed(1);
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
            const uint64_t result_starstar = RotateLeft(s[1] * 5, 7) * 9;

            const uint64_t t = s[1] << 17;

            s[2] ^= s[0];
            s[3] ^= s[1];
            s[1] ^= s[2];
            s[0] ^= s[3];

            s[2] ^= t;

            s[3] = RotateLeft(s[3], 45);

            return result_starstar;
        }

    private:
        uint64_t RotateLeft(const uint64_t x, int k) noexcept
        {
            return (x << k) | (x >> (64 - k));
        }

        uint64_t s[4]; // engine state
    };

    class RandomSource
    {
    public:
        bool SampleBool(akFloat prob)
        {
            return UniformReal() < prob;
        }

        // int UniformInt(int min = 0, int max = 1) { return 0; }

        akFloat UniformReal(akFloat min = kFloatZero, akFloat max = kFloatOne)
        {
            return static_cast<akFloat>(rand()) / (RAND_MAX + 1);
        }

    private:
        RandomEngine engine_;
    };
} // namespace akane