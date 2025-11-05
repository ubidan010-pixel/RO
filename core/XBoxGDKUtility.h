#pragma once

#include "core/Types.h"
#include "core/memory/UniquePtr.h"

namespace ITF
{
    // helper, see below
    namespace MethodToXAsyncBlock_private
    {
        template <typename M>
        class Helper;

        template <typename T>
        class Helper<void (T::*)(XAsyncBlock)>
        {
        public:

            using Class = T;

            template <auto Method>
            static constexpr auto getFunctionPtr()
            {
                return &staticMethod<Method>;
            }

            template <auto Method>
            static void staticMethod(XAsyncBlock* _asyncBlock)
            {
                UniquePtr<XAsyncBlock> ownedAsyncBlock = UniquePtr<XAsyncBlock>(_asyncBlock);
                auto instance = static_cast<T*>(ownedAsyncBlock->context);
                (instance->*Method)(*ownedAsyncBlock);
            }
        };
    }

    // XAsyncBlockHandler is a helper to manage XAsyncBlock for asynchronous calls.
    // Usage:
    // 1) Create an instance of XAsyncBlockHandler with the method to call when the async operation completes.
    // 2) Call get() to retrieve the XAsyncBlock to pass to the async API.
    // 3) Call setResult() with the HRESULT from the async API when it completes.
    // it is important to call setResult() to avoid memory double delete.
    // A macro is provided if you just want to assert on HResult failure: ITF_VERIFY_HR_CALL_WITH_XASYNC
    // In this case, setResult code is not required

    template <auto Method>
    class XAsyncBlockHandler
    {
    public:

        using Helper = MethodToXAsyncBlock_private::Helper<decltype(Method)>;

        XAsyncBlockHandler() = default;
        XAsyncBlockHandler(XAsyncBlockHandler&&) = default;

        ~XAsyncBlockHandler()
        {
            ITF_ASSERT_MSG(m_temporarilyOwnedAsyncBlock == nullptr, "Be sure to call setResult before destruction");
        }

        XAsyncBlockHandler(XAsyncBlockHandler&&) = default;

        XAsyncBlock* get()
        {
            m_temporarilyOwnedAsyncBlock = makeUnique<MemoryId::mId_System, XAsyncBlock>();

            XAsyncBlock* asyncBlock = temporarilyOwnedAsyncBlock.get();
            *asyncBlock = {};
            asyncBlock->queue = nullptr; // default queue
            asyncBlock->context = this;
            asyncBlock->callback = Helper::template getFunctionPtr<Method>();
            return asyncBlock;
        }

        void setResult(HRESULT _hr)
        {
            ITF_ASSERT_MSG(m_temporarilyOwnedAsyncBlock != nullptr, "You must have called getAsyncBlock before");
            if (SUCCEEDED(_hr))
            {
                // Release the unique-ptr so that callback can own it.
                m_temporarilyOwnedAsyncBlock.release();
            }
            else
            {
                // Error => free it
                m_temporarilyOwnedAsyncBlock.reset();
            }
        }

    private:
        UniquePtr<XAsyncBlock> temporarilyOwnedAsyncBlock;
    };

    #define ITF_VERIFY_HR_CALL_WITH_XASYNC(asyncHandler_, call_) \
        do { \
            HRESULT hr_ = (call_); \
            ITF_ASSERT_HR_RESULT_NAMED(hr_, #call_); \
            asyncHandler.setResult(hr_); \
        } while(0)
}
