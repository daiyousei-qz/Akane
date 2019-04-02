#include "akane/integrator.h"

using namespace std;

namespace akane
{
    class PathTracingIntegrator : public Integrator
    {
    };

    unique_ptr<Integrator> CreatePathTracingIntegrator()
    {
        return make_unique<PathTracingIntegrator>();
    }
} // namespace akane