#pragma once

#if !defined(ITF_ENABLE_NVN_GRAPHICS_DEBUGGING)
    #if defined(ITF_DEBUG) || defined(ITF_RELEASE)
        #define ITF_ENABLE_NVN_GRAPHICS_DEBUGGING 1
    #else
        #define ITF_ENABLE_NVN_GRAPHICS_DEBUGGING 0
    #endif
#endif

#if defined(ITF_FINAL) && !ITF_ENABLE_NVN_GRAPHICS_DEBUGGING
    // force use of fastpath version of some nvn methods
    #define NVN_FORCE_FASTPATH
#endif // ITF_FINAL

#include <nvn/nvn_Cpp.h>
#include <nvn/nvn_CppMethods.h>

#include <memory>

namespace ITF::NVN
{

    inline int getDeviceProperty(nvn::Device & device, nvn::DeviceInfo _info)
    {
        int value;
        device.GetInteger(_info, &value);
        return value;
    }


    template <typename, typename = void>
    struct HasFinalizeMethod : std::false_type {};

    template <typename T>
    struct HasFinalizeMethod<T, std::void_t<decltype(std::declval<T>().Finalize())>> : std::true_type {};

    template <typename T>
    struct NvnObjectDeleter
    {
        void operator ()(T* t)
        {
            if constexpr (HasFinalizeMethod<T>::value)
                t->Finalize();
            delete t;
        }
    };

    template <typename T>
    using UniquePtr = std::unique_ptr<T, NvnObjectDeleter<T>>;


    template <typename T, typename... Args>
    UniquePtr<T> makeUnique(Args&&... args)
    {
        return UniquePtr<T>(new (MemoryId::mId_GfxAdapter) T(std::forward<Args>(args)...));
    }

    template <typename T, typename Builder>
    UniquePtr<T> makeBuilderInitialized(Builder & builder)
    {
        auto p = makeUnique<T>();
        if (!p->Initialize(&builder))
        {
            return {};
        }
        return p;
    }

    template <typename T, typename... Args>
    UniquePtr<T> makeInitialized(Args&&... args)
    {
        auto p = makeUnique<T>();
        if (!p->Initialize(std::forward<Args>(args)...))
        {
            return {};
        }
        return p;
    }

    inline void setMarker(nvn::CommandBuffer* _ctxt, const char* _markerName)
    {
    #if ITF_ENABLE_NVN_GRAPHICS_DEBUGGING
        #ifdef ITF_NX
            _ctxt->InsertDebugMarkerDynamic(0, _markerName);
        #else
            _ctxt->InsertDebugMarker(0, _markerName);
        #endif
    #endif
    }

    inline void pushMarker(nvn::CommandBuffer* _ctxt, const char* _markerName)
    {
    #if ITF_ENABLE_NVN_GRAPHICS_DEBUGGING
        #ifdef ITF_NX
            _ctxt->PushDebugGroupDynamic(0, _markerName);
        #else
            _ctxt->PushDebugGroup(0, _markerName);
        #endif
    #endif
    }

    inline void popMarker(nvn::CommandBuffer* _ctxt)
    {
    #if ITF_ENABLE_NVN_GRAPHICS_DEBUGGING
        #ifdef ITF_NX
            _ctxt->PopDebugGroupId(0);
        #else
            _ctxt->PopDebugGroup(0);
        #endif
    #endif
    }

    // Various GPU-GPU sync methods
    inline void syncRenderTargetToTexture(nvn::CommandBuffer* _ctxt)
    {
        _ctxt->Barrier(nvn::BarrierBits::ORDER_FRAGMENTS
                    | nvn::BarrierBits::INVALIDATE_TEXTURE);
    }

    inline void syncAll(nvn::CommandBuffer* _ctxt)
    {
        _ctxt->Barrier(nvn::BarrierBits::ORDER_FRAGMENTS
                    | nvn::BarrierBits::ORDER_PRIMITIVES
                    | nvn::BarrierBits::INVALIDATE_TEXTURE
                    | nvn::BarrierBits::INVALIDATE_TEXTURE_DESCRIPTOR
                    | nvn::BarrierBits::INVALIDATE_SHADER);
    }

    class BlendStateCache
    {
    public:
        // This class is used to cache the last set blend state and avoid to set it at every draw call.
        BlendStateCache() = default;

        union BlendDescKey
        {
            u64 value;
            struct
            {
                u64 blendEnable : 1;
                u64 srcBlend : 8;
                u64 destBlend : 8;
                u64 blendEquation : 5;
                u64 srcBlendAlpha : 8;
                u64 destBlendAlpha : 8;
                u64 blendEquationAlpha : 5;
                u64 renderTargetWriteMask : 4;
            };
        };
        static_assert(sizeof(BlendDescKey) == sizeof(u64), "BlendDescKey must be 8 bytes to be used as a key.");

        void setIfChanged(nvn::CommandBuffer& _rdrCtxt, BlendDescKey _key)
        {
            if (m_lastKey.value != _key.value)
            {
                m_lastKey = _key;
                nvn::ColorState colorState;
                colorState.SetDefaults()
                    .SetBlendEnable(0, _key.blendEnable);
                _rdrCtxt.BindColorState(&colorState);
                if (_key.blendEnable)
                {
                    auto blendState = buildBlendState(_key);
                    _rdrCtxt.BindBlendState(&blendState);
                }
            }
        }

        static nvn::BlendState buildBlendState(BlendDescKey _key)
        {
            nvn::BlendState blendState;
            blendState.SetDefaults()
                .SetBlendEquation(nvn::BlendEquation::Enum(_key.blendEquation), nvn::BlendEquation::Enum(_key.blendEquationAlpha))
                .SetBlendFunc(
                    nvn::BlendFunc::Enum(_key.srcBlend),
                    nvn::BlendFunc::Enum(_key.destBlend),
                    nvn::BlendFunc::Enum(_key.srcBlendAlpha),
                    nvn::BlendFunc::Enum(_key.destBlendAlpha));
            return blendState;
        }


        // reset the cache so that the next call to setIfChanged will reassign the blend state in the cmd buffer.
        // Call reset each time a blend state is changed without using this cache.
        void reset()
        {
            m_lastKey = { U32_INVALID };
        }

    private:

        BlendDescKey m_lastKey = { U32_INVALID };
    };

    class PolygonStateCache
    {
    public:
        // This class is used to cache the PolygonState and avoid to set it at every draw call.

        PolygonStateCache()
        {
            reset();
        }

        PolygonStateCache(const PolygonStateCache&) = delete;
        PolygonStateCache& operator = (const PolygonStateCache&) = delete;

        void setIfChanged(nvn::CommandBuffer* _rdrCtxt, nvn::PolygonMode::Enum mode)
        {
            if (m_reseted || m_state.GetPolygonMode() != mode)
            {
                _rdrCtxt->BindPolygonState(&m_state);
            }
        }

        // reset the cache so that the next call to setIfChanged will reassign polygon state in the cmd buffer.
        // Call reset each time the state is changed without using this cache and at frame start.
        void reset()
        {
            m_state.SetDefaults();
            m_reseted = true;
        }

    private:

        bool m_reseted = true;
        nvn::PolygonState m_state;
    };

}

