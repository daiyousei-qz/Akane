#pragma once
#include "akane/common/language_utils.h"
#include "akane/common/ptr_arithmetic.h"
#include <memory>
#include <vector>
#include <type_traits>

namespace akane
{
    constexpr size_t kWorkspaceItemAlignSize = 16;
    constexpr size_t kWorkspaceItemAlignMask = kWorkspaceItemAlignSize - 1;

    constexpr size_t kMinWorkspaceSize     = 1024;
    constexpr size_t kDefaultWorkspaceSize = 8 * 1024;

    class Workspace final
    {
    public:
        using Ptr = std::unique_ptr<Workspace>;

        Workspace(size_t size = kDefaultWorkspaceSize)
        {
            auto alloc_size = std::min(size, kMinWorkspaceSize);

            buffer_ = malloc(alloc_size);
            size_   = alloc_size;
            offset_ = 0;
        }
        ~Workspace()
        {
            Clear();
            free(buffer_);
        }

        Workspace(const Workspace&) = delete;
        Workspace(Workspace&&)      = delete;

        Workspace& operator=(const Workspace&) = delete;
        Workspace& operator=(Workspace&&) = delete;

        bool Idle() const noexcept
        {
            return offset_ == 0;
        }

        size_t Size() const noexcept
        {
            return size_;
        }

        void* Allocate(size_t sz) noexcept
        {
            return AllocateAux(AlignSize(sz));
        }

        template <typename T, typename... TArgs> T* New(TArgs&&... args)
        {
            static_assert(alignof(T) <= kWorkspaceItemAlignSize);

            auto alloc_size = AlignSize(sizeof(T));
            auto ptr        = AllocateAux(alloc_size);

            if (ptr == nullptr)
            {
                return nullptr;
            }

            if constexpr (std::is_nothrow_constructible_v<T, TArgs...>)
            {
                return PrepareItem<T>(ptr, std::forward<TArgs>(args)...);
            }
            else
            {
                try
                {
                    return PrepareItem<T>(ptr, std::forward<TArgs>(args)...);
                }
                catch (...)
                {
                    offset_ -= alloc_size;
                    throw;
                }
            }
        }

        void Clear()
        {
            for (const auto& record : destructors_)
            {
                record.disposer(record.object);
            }

            offset_ = 0;
        }

    private:
        template <typename T, typename... TArgs> T* PrepareItem(void* ptr, TArgs&&... args)
        {
            new (ptr) T(std::forward<TArgs>(args)...);

            if constexpr (!std::is_trivially_destructible_v<T>)
            {
                destructors_.push_back(
                    DestructionRecord{ptr, [](void* p) { reinterpret_cast<T*>(p)->~T(); }});
            }

            return reinterpret_cast<T*>(ptr);
        }

        void* AllocateAux(size_t alloc_size) noexcept
        {
            if (offset_ + alloc_size > size_)
            {
                return nullptr;
            }

            auto result = AdvancePtr(buffer_, offset_);
            offset_ += alloc_size;

            return result;
        }

        size_t AlignSize(size_t sz) noexcept
        {
            auto align_extra = kWorkspaceItemAlignSize - sz & kWorkspaceItemAlignMask;
            return sz + align_extra;
        }

        struct DestructionRecord
        {
            void* object;
            void (*disposer)(void*);
        };

        void* buffer_;
        size_t size_;
        size_t offset_;

        std::vector<DestructionRecord> destructors_;
    };
} // namespace akane