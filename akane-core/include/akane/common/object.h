#pragma once

namespace akane
{
    // common base for heap-allocated no-copy no-move objects
    class Object
    {
    public:
        Object()          = default;
        virtual ~Object() = default;

        Object(const Object&) = delete;
        Object(Object&&)      = delete;

        Object& operator=(const Object&) = delete;
        Object& operator=(Object&&) = delete;
    };
} // namespace akane