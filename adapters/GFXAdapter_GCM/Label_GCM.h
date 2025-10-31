#ifndef ITF_LABELGCM_H_
#define ITF_LABELGCM_H_

#include "RenderContext_GCM.h"

// manage a label used to synch gpu and ppu (and maybe spu)
namespace ITF
{
namespace GCM
{
    class LabelManager : public TemplateSingleton<LabelManager>
    {
    public:
        enum 
        {
            InvalidLabelIndex = u32(-1),
            FirstLabelIndex = 65u, // indices before this one are reserved
            LastLabelIndex = 254u // indices are 8 bits, label 255 is reserved to Tuner
        };

        LabelManager();

        u32 allocIndex();
        void freeIndex(u32 _index);
    
    private:
        Vector<u32> m_freeIndices;
    };

    struct Label
    {
        Label()
            : labelIdx(LabelManager::InvalidLabelIndex)
            , labelAddr(0)
        {}

        ~Label() { release(); }

        void init(u32 _initialValue = 0); // allocate a label
        void release(); // release the label

        u32 getValue() const { return *labelAddr; }

        // GPU commands
        void setWriteOnCommand(RenderContext & _rdrCtxt, u32 _value)
        {
            _rdrCtxt.gcmCtxt->SetWriteCommandLabel(labelIdx, _value);
        }

        void setWriteOnBackEnd(RenderContext & _rdrCtxt, u32 _value)
        {
            _rdrCtxt.gcmCtxt->SetWriteBackEndLabel(labelIdx, _value);
        }

        void setWriteOnTexture(RenderContext & _rdrCtxt, u32 _value)
        {
            _rdrCtxt.gcmCtxt->SetWriteTextureLabel(labelIdx, _value);
        }

        // Put a GPU command to wait for a specific value
        void setGpuWait(RenderContext & _rdrCtxt, u32 _value)
        {
            _rdrCtxt.gcmCtxt->SetWaitLabel(labelIdx, _value);
        }

        // wait that a label got a specific value (PPU blocking)
        void ppuWait(u32 _value)
        {
            while ((*labelAddr) != _value)
            {
                sys_timer_usleep(300); // value used in samples
            }
        }

        // context rendering version -> flush the command buffer before
        void ppuWait(RenderContext & _rdrCtxt, u32 _value)
        {
            _rdrCtxt.gcmCtxt->Flush();
            ppuWait(_value);
        }

        template<typename F>
        void ppuWaitIf(F _fct) // loop waiting on the condition
        {
            while (_fct(*labelAddr))
            {
                sys_timer_usleep(300); // value used in samples
            }
        }

        // context rendering version -> flush the command buffer before
        template<typename F>
        void ppuWaitIf(RenderContext & _rdrCtxt, F fct)
        {
            _rdrCtxt.gcmCtxt->Flush();
            ppuWaitIf(fct);
        }

        u32 labelIdx;
        u32 * volatile labelAddr;
    };

} // GCM
} // ITF

#endif // ITF_LABELGCM_H_
