#pragma once
#include "akane/common/basic.h"
#include "akane/bsdf.h"
#include "akane/ray.h"
#include "edslib/memory/arena.h"
#include <memory>

namespace akane
{
    class Material : public Object
    {
    public:
        // compute bsdf allocated at workspace given
        virtual const Bsdf* ComputeBsdf(Workspace& workspace,
                                        const IntersectionInfo& isect) const = 0;

        virtual Spectrum ComputePreviewColor(Vec2 uv) const
        {
            return Spectrum{uv[0], uv[1], 0.f};
        }
    };
} // namespace akane