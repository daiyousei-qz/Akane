#pragma once

#include "akane/common/language_utils.h"
#include "akane/core.h"
#include "akane/primitive.h"
#include <vector>

namespace akane
{
    class NaiveComposite : public Composite
    {
    public:
		void AddPrimitive(Primitive* body)
		{
			parts_.push_back(body);
		}

        bool Intersect(const Ray& ray, akFloat t_min, akFloat t_max,
                       IntersectionInfo& info) const override
        {
            bool any_hit = false;
            akFloat t    = t_max;

            IntersectionInfo info_buf;
            for (auto child : parts_)
            {
                if (child->Intersect(ray, t_min, t, info_buf))
                {
                    any_hit = true;
                    t       = info_buf.t;
                    info    = info_buf;
                }
            }

            return any_hit;
        }

    private:
        std::vector<Primitive*> parts_;
    };
} // namespace akane