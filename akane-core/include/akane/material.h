#pragma once
#include "akane/core.h"
#include "akane/bsdf.h"
#include "akane/spectrum.h"
#include "akane/texture.h"
#include "akane/math/sampling.h"
#include <optional>

namespace akane
{
    class Material : public Object
    {
    public:
        using Ptr = std::unique_ptr<Material>;

        virtual const Bsdf* ComputeBsdf(Workspace& workspace,
                                        const IntersectionInfo& isect) const = 0;
    };

    Material::Ptr CreateLambertian(const Texture* texture);

} // namespace akane