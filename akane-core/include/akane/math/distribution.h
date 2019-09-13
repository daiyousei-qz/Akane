#pragma once
#include "akane/math/math.h"
#include <vector>

namespace akane
{
    class DiscrateDistribution
    {
    public:
        DiscrateDistribution() = default;
        DiscrateDistribution(const float* weight_begin, const float* weight_end) : thresholds_{}
        {
            Reset(weight_begin, weight_end);
        }

        int Sample(float u, float& pdf_out) const
        {
            if (u < thresholds_.front())
            {
                pdf_out = thresholds_.front();
                return 0;
            }

            for (int i = 1; i < thresholds_.size(); ++i)
            {
                if (u >= thresholds_[i - 1] && u < thresholds_[i])
                {
                    pdf_out = thresholds_[i] - thresholds_[i - 1];
                    return i;
                }
            }

            // error
            pdf_out = 0;
            return -1;
        }

        void Reset(const float* weight_begin, const float* weight_end)
        {
            thresholds_.clear();
            thresholds_.reserve(std::distance(weight_begin, weight_end));

            float acc = 0.f;
            for (auto p = weight_begin; p != weight_end; ++p)
            {
                acc += *p;
                thresholds_.push_back(acc);
            }

            if (acc != kZero)
            {
                for (auto& x : thresholds_)
                {
                    x /= acc;
                }
            }
            else
            {
                thresholds_.push_back(1.f);
            }
        }

    private:
        std::vector<float> thresholds_{1.f};
    };

} // namespace akane