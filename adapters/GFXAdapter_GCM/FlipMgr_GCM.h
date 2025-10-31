#ifndef ITF_FLIPMGR_GCM_H_
#define ITF_FLIPMGR_GCM_H_

#include <sysutil/sysutil_sysparam.h>

#include "Surface_GCM.h"
#include "RenderContext_GCM.h"

namespace ITF
{
namespace GCM
{
    // class handling what has to be done at flip,
    // including resolution, aspect ratio or PAL 50Hz conversion
    
    // The idea is to use the following policy:
    // 1080p/i or 720p -> not using libresc
    // All other resolutions -> using libresc to convert from 720p
    // All resolution with aspect ratio 4/3 -> using libresc to convert to letter box mode (black area up and down)
    // 576p/i (PAL) -> using libresc to convert 60Hz to 50Hz (depending on AutoAdaptFrequency)

    class FlipMgr
    {
    public:
        enum 
        {
            AutoAdaptFrequency = 0
        };

        enum Type
        {
            FLIPMGR_USING_GCM,
            FLIPMGR_USING_RESC
        };
        static FlipMgr * create(Type _type);

        FlipMgr() {}
        virtual ~FlipMgr() {}

        virtual bool init(const CellVideoOutState & videoOutState) = 0;
        virtual bool setDisplayBuffers(GCM::Surface * _displayBuffers[], ux _nbDisplayBufffer) = 0;

        virtual void close() = 0;

        virtual void setFlipHandler(void (*onFlipFct)(uint32_t arg)) = 0;

        virtual void setFlip(RenderContext & _rdrCtxt, ux _idx) = 0;
        virtual void setWaitFlip(RenderContext & _rdrCtxt) = 0;

        virtual u64 getLastFlipTime() const = 0;
    };

}

}

#endif // ITF_FLIPMGR_GCM_H_
