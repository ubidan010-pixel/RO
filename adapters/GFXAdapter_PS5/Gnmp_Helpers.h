#ifndef ITF_GNMP_HELPERS_H_
#define ITF_GNMP_HELPERS_H_

#pragma once

#include <agc/gnmp/gnmp.h>
#include <agc/gnmp/gnmx.h>
#include <agc/core/sync.h>
#include "Engine/profiler/profilerMacro_PS5.h"

#ifndef ITF_FINAL
    // Agc resource registering is useful for GPU Performance analysis with Razor GPU
    #ifndef ITF_ENABLE_AGC_RESOURCE_REGISTERING
        #define ITF_ENABLE_AGC_RESOURCE_REGISTERING 1
    #endif
    #ifndef ITF_ENABLE_RAZOR_GPU_MARKERS
        #define ITF_ENABLE_RAZOR_GPU_MARKERS 1
    #endif
#endif

#if defined(ITF_ENABLE_AGC_RESOURCE_REGISTERING) && ITF_ENABLE_AGC_RESOURCE_REGISTERING
#include <agc/core/resourceregistration.h>
#endif

namespace ITF
{
    namespace Gnmp
    {
        using namespace sce::Agc::Gnmp;

#if defined(ITF_ENABLE_AGC_RESOURCE_REGISTERING) && ITF_ENABLE_AGC_RESOURCE_REGISTERING
        enum : u32 { AGC_RESOURCE_REGISTERING_MAX_NUM_OWNERS = 4000 };
        enum : u32 { AGC_RESOURCE_REGISTERING_MAX_STRING_LENGTH = 64 };

        #define ITF_AGC_REGISTER_RESOURCE(res_, ...) ::sce::Agc::Core::registerResource(res_, __VA_ARGS__)
        class ResourceHandler
        {
        public:
            ResourceHandler() = default;

            explicit ResourceHandler(sce::Agc::ResourceRegistration::ResourceHandle hdl)
                : m_hdl(hdl)
            {
            }

            ResourceHandler(const ResourceHandler&) = delete;
            ResourceHandler(ResourceHandler&& other)
                : m_hdl(other.m_hdl)
            {
                other.m_hdl = kInvalidResourceHandle;
            }
            ResourceHandler& operator = (const ResourceHandler&) = delete;
            ResourceHandler& operator = (ResourceHandler&& other)
            {
                if (&other != this)
                {
                    unregister();
                    m_hdl = other.m_hdl;
                    other.m_hdl = kInvalidResourceHandle;
                }
                return *this;
            }

            ~ResourceHandler()
            {
                unregister();
            }

            void set(sce::Agc::ResourceRegistration::ResourceHandle hdl)
            {
                unregister();
                m_hdl = hdl;
            }

            void unregister()
            {
                if (m_hdl != kInvalidResourceHandle)
                {
                    sce::Agc::ResourceRegistration::unregisterResource(m_hdl);
                    m_hdl = kInvalidResourceHandle;
                }
            }

        private:
            sce::Agc::ResourceRegistration::ResourceHandle m_hdl = sce::Agc::Gnmp::kInvalidResourceHandle;
        };

        template <typename R>
        class AutoResourceHandler
        {
        public:
            AutoResourceHandler() = default;
            explicit AutoResourceHandler(R * _this) // keep it with same lifetime as the given resource
                : m_this(_this)
            {
            }

            AutoResourceHandler(const AutoResourceHandler&) = delete;
            AutoResourceHandler& operator = (const AutoResourceHandler&) = delete;

            void set(sce::Agc::ResourceRegistration::ResourceHandle hdl)
            {
                m_handler.set(hdl);
            }

            void setResource(R* _this)
            {
                m_this = _this;
            }

            void set(const char * _resName)
            {
                ITF_ASSERT(m_this != nullptr);
                if (m_this != nullptr)
                {
                    m_handler.set(ITF_AGC_REGISTER_RESOURCE(m_this, "%s", _resName));
                }
            }

            void setFromPath(const char* _path)
            {
                const char* shortFileName = strrchr(_path, '/');
                if (shortFileName == nullptr)
                {
                    shortFileName = _path;
                }
                else
                {
                    shortFileName++;
                }

                // crop at end
                if (strlen(shortFileName) > Gnmp::AGC_RESOURCE_REGISTERING_MAX_STRING_LENGTH)
                {
                    shortFileName = shortFileName + strlen(shortFileName) - Gnmp::AGC_RESOURCE_REGISTERING_MAX_STRING_LENGTH;
                }

                set(shortFileName);
            }

        private:
            ResourceHandler m_handler;
            R* m_this = nullptr;
        };


#else
        #define ITF_AGC_REGISTER_RESOURCE(res_, ...) sce::Agc::Gnmp::kInvalidResourceHandle
        struct ResourceHandler
        {
            ResourceHandler() = default;
            explicit ResourceHandler(sce::Agc::ResourceRegistration::ResourceHandle hdl)
            {
            }
            ResourceHandler(const ResourceHandler&) = delete;
            ResourceHandler(ResourceHandler&& other) = default;
            ResourceHandler& operator = (const ResourceHandler&) = delete;
            ResourceHandler& operator = (ResourceHandler&& other) = default;
            ~ResourceHandler() = default;
            void set(sce::Agc::ResourceRegistration::ResourceHandle hdl) {}
            void unregister() {}
        };

        template <typename R>
        class AutoResourceHandler
        {
        public:
            AutoResourceHandler() = default;
            explicit AutoResourceHandler(R* _this)
            {
            }

            AutoResourceHandler(const AutoResourceHandler&) = delete;
            AutoResourceHandler& operator = (const AutoResourceHandler&) = delete;

            void setResource(R* _this)
            {
            }

            void set(sce::Agc::ResourceRegistration::ResourceHandle hdl)
            {
            }

            void set(const char* resName)
            {
            }

            void setFromPath(const char* _path)
            {
            }
        };
#endif
    }
}


namespace ITF
{
    // forward Gnm, Gnmx and GpuAddress namespace in ITF

    namespace Gnmp
    {
        using namespace sce::Agc::Gnmp;

        // This class is a simple profiler start/stop class to profile using explicit method calls
        class TimerProfiler
        {
        public:
            TimerProfiler() = default;
            TimerProfiler(const TimerProfiler&) = delete;
            TimerProfiler& operator=(const TimerProfiler&) = delete;

            void start()
            {
                m_startTime = PS5HighPerfTimer::GetTimeValue();
            }

            f32 stop()
            {
                u64 currentTime = PS5HighPerfTimer::GetTimeValue();
                f32 duration = PS5HighPerfTimer::GetDeltaInSeconds(m_startTime, currentTime);
                m_startTime = currentTime;
                return duration;
            }

            u64 getStartTime() const
            {
                return m_startTime;
            }

            f64 getDurationSinceStart() const
            {
                return PS5HighPerfTimer::GetDeltaInSeconds(m_startTime, PS5HighPerfTimer::GetTimeValue());
            }

        protected:
            u64 m_startTime;
        };


        // Profile in a scope filling a variable
        class TimerProfilerScope
        {
        public:
            explicit TimerProfilerScope(f32 & _timeInSeconds)
                : m_timeInSeconds(_timeInSeconds)
            {
                m_profiler.start();
            }

            ~TimerProfilerScope()
            {
                m_timeInSeconds = m_profiler.stop();
            }

            u64 getStartTime() const
            {
                return m_profiler.getStartTime();
            }

            f64 getDurationSinceStart() const
            {
                return m_profiler.getDurationSinceStart();
            }

        protected :
            TimerProfiler m_profiler;
            f32& m_timeInSeconds;
        };

        template <size_t N>
        inline void setMarker(GnmxGfxContext* _ctxt, const char(&_literalStrOfMarkerName)[N])
        {
        #if defined(ITF_ENABLE_RAZOR_GPU_MARKERS) && ITF_ENABLE_RAZOR_GPU_MARKERS
            _ctxt->m_agcCtx.m_dcb.setMarkerLiteral(_literalStrOfMarkerName);
        #endif
        }

        inline void setMarker(GnmxGfxContext* _ctxt, const char* _markerName)
        {
        #if defined(ITF_ENABLE_RAZOR_GPU_MARKERS) && ITF_ENABLE_RAZOR_GPU_MARKERS
            _ctxt->m_agcCtx.m_dcb.setMarker(_markerName);
        #endif
        }

        template <size_t N>
        inline void pushMarker(GnmxGfxContext* _ctxt, const char(&_literalStrOfMarkerName)[N])
        {
        #if defined(ITF_ENABLE_RAZOR_GPU_MARKERS) && ITF_ENABLE_RAZOR_GPU_MARKERS
            _ctxt->m_agcCtx.m_dcb.pushMarkerLiteral(_literalStrOfMarkerName);
        #endif
        }

        inline void pushMarker(GnmxGfxContext* _ctxt, const char* _markerName)
        {
        #if defined(ITF_ENABLE_RAZOR_GPU_MARKERS) && ITF_ENABLE_RAZOR_GPU_MARKERS
            _ctxt->m_agcCtx.m_dcb.pushMarker(_markerName);
        #endif
        }

        inline void popMarker(GnmxGfxContext* _ctxt)
        {
        #if defined(ITF_ENABLE_RAZOR_GPU_MARKERS) && ITF_ENABLE_RAZOR_GPU_MARKERS
            _ctxt->m_agcCtx.m_dcb.popMarker();
        #endif
        }

    }

    namespace Gnmx
    {
        using GfxContext = sce::Agc::Gnmp::GnmxGfxContext;
        using DrawCommandBuffer = sce::Agc::Gnmp::GnmxDrawCommandBuffer;
        using DispatchCommandBuffer = sce::Agc::Gnmp::GnmxDispatchCommandBuffer;
    }

    namespace Gnmp
    {
        // Various GPU sync methods
        inline void syncRenderTargetToTexture(Gnmx::GfxContext* _ctxt)
        {
            using namespace ::sce::Agc::Core;
            gpuSyncEvent(&_ctxt->m_agcCtx.m_dcb, SyncWaitMode::kDrainGraphics, SyncCacheOp::kFlushCompressedColorBufferForTexture);
        }

        inline void syncAll(Gnmx::GfxContext* _ctxt)
        {
            using namespace ::sce::Agc::Core;
            gpuSyncEvent(&_ctxt->m_agcCtx.m_dcb, SyncWaitMode::kDrainGraphics, SyncCacheOp::kClearAll);
        }

        class BlendStateCache
        {
        public:
            // This class is used to cache the last set blend state and avoid to set it at every draw call.
            // Changing the blend state will consume one of the 6 hardware context, so should not be changed if not needed.
            BlendStateCache() = default;

            union BlendDescKey
            {
                u32 value;
                struct
                {
                    u32 BlendEnable : 1;
                    u32 SrcBlend : 5;
                    u32 DestBlend : 5;
                    u32 BlendOp : 3;
                    u32 SrcBlendAlpha : 5;
                    u32 DestBlendAlpha : 5;
                    u32 BlendOpAlpha : 3;
                    u32 RenderTargetWriteMask : 4;
                    u32 rest : 1;
                };
            };

            void setIfChanged(Gnmx::GfxContext& _rdrCtxt, BlendDescKey _key)
            {
                if (m_lastKey.value != _key.value)
                {
                    m_lastKey = _key;
                    _rdrCtxt.setBlendControl(0, buildBlendControl(_key));
                }
            }

            static Gnmp::BlendControl buildBlendControl(BlendDescKey _key)
            {
                Gnmp::BlendControl bc;
                bc.init();
                bc.setBlendEnable(_key.BlendEnable);
                bc.setColorEquation((BlendMultiplier)_key.SrcBlend, (BlendFunc)_key.BlendOp, (BlendMultiplier)_key.DestBlend);
                bbool separateAlpha = (_key.SrcBlendAlpha != _key.SrcBlend)
                    || (_key.BlendOpAlpha != _key.BlendOp)
                    || (_key.DestBlendAlpha != _key.DestBlend);
                bc.setSeparateAlphaEnable(separateAlpha);
                bc.setAlphaEquation((BlendMultiplier)_key.SrcBlendAlpha, (BlendFunc)_key.BlendOpAlpha, (BlendMultiplier)_key.DestBlendAlpha);
                return bc;
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

        class PrimitiveTypeCache
        {
        public:
            // This class is used to cache the last primitive cache set.
            // Changing the primitive type is relinking the shaders, so will consume one of the 6 hardware context.
            PrimitiveTypeCache() = default;

            void setIfChanged(Gnmx::GfxContext& _rdrCtxt, Gnmp::PrimitiveType _primType)
            {
                if (m_lastPrimType != _primType)
                {
                    m_lastPrimType = _primType;
                    _rdrCtxt.setPrimitiveType(_primType);
                }
            }

            // reset the cache so that the next call to setIfChanged will reassign the blend state in the cmd buffer.
            // Call reset each time a blend state is changed without using this cache.
            void reset()
            {
                m_lastPrimType = kPrimitiveTypeNone;
            }

        private:
            Gnmp::PrimitiveType m_lastPrimType = kPrimitiveTypeNone;
        };

    }

    namespace GpuAddress
    {
        using namespace sce::AgcGpuAddress;
    }
}

#endif // ITF_GNM_HELPERS_H_
