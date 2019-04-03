#include "akane/math/geometry.h"

using namespace akane;
using namespace std;

constexpr Vec2f foo(Vec2f x)
{
    Vec2f y = x + x;
    y += 1.f;

    return y - x;
}

int main()
{
    constexpr Vec2f v = Vec2f{1.f, 1.f};
    constexpr Vec2f z = foo(v);

    constexpr auto x0 = z[0];
    constexpr auto x1 = z[1];
    auto sum          = z.Sum();
    auto len          = z.Length();
    auto z_           = z.Normalized();

    return 0;
}
