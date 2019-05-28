#pragma once
#include "akane/core.h"
#include "akane/bsdf.h"
#include <optional>

namespace akane
{
    class Material : public Object
    {
    public:
        using Ptr = std::unique_ptr<Material>;
        using SharedPtr = std::shared_ptr<Material>;

		// compute bsdf allocated at workspace given
        virtual const Bsdf* ComputeBsdf(Workspace& workspace,
                                        const IntersectionInfo& isect) const = 0;
    };

} // namespace akane