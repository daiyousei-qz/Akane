#pragma once
#include "akane/common/basic.h"
#include "akane/math/distribution.h"
#include "akane/ray.h"
#include "akane/light.h"
#include "edslib/memory/arena.h"
#include <memory>
#include <vector>

namespace akane
{
    constexpr float kTravelDistanceMin = 0.1f;
    constexpr float kTravelDistanceMax = 10000.f;

    class Scene : public virtual Object
    {
    public:
        using Ptr       = std::unique_ptr<Scene>;
        using SharedPtr = std::shared_ptr<Scene>;

        virtual void Commit()
        {
            UpdataLightDistribution();
        }

        virtual bool Intersect(const Ray& ray, Workspace& workspace,
                               IntersectionInfo& isect) const = 0;

        const auto& GetGlobalLightVec() const
        {
            return global_lights_;
        }
        const auto& GetLightVec() const
        {
            return lights_;
        }

        const Light* SampleLight(float u, float& pdf_out) const
        {
            if (lights_.empty())
            {
                pdf_out = 0.f;
                return nullptr;
            }

            auto index = light_dist_.Sample(u, pdf_out);
            return lights_[index];
        }

    protected:
        void UpdataLightDistribution()
        {
            float total_power = 0.f;
            std::vector<float> power_weights;
            for (auto light : lights_)
            {
                auto power = light->Power();
                total_power += power;
                power_weights.push_back(power);
            }

            total_light_power_ = total_power;
            light_dist_.Reset(power_weights.data(), power_weights.data() + power_weights.size());
        }

        void RegisterGlobalLight(GlobalLight* light)
        {
            global_lights_.push_back(light);
        }

        void RegisterLight(Light* light)
        {
            lights_.push_back(light);
        }

    private:
        std::vector<GlobalLight*> global_lights_; // global lights won't be explicitly sampled
        std::vector<Light*> lights_;

        float total_light_power_;
        DiscrateDistribution light_dist_;
    };
} // namespace akane