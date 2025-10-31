#ifndef _ITF_ANIMATIONPATCHES_H_
#define _ITF_ANIMATIONPATCHES_H_

#ifndef _ITF_CORETYPES_H_
    #include    "core/types.h"
#endif

#ifndef _ITF_SAFEARRAY_H_
    #include "core/container/SafeArray.h"
#endif //_ITF_SAFEARRAY_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif // _ITF_GFX_ADAPTER_H_

#ifndef _ITF_ANIMATIONBONES_H_
#include    "engine/animation/AnimationBones.h"
#endif // _ITF_ANIMATIONBONES_H_

namespace ITF
{    
    class AnimPatch;
    struct AnimBMLInfo;

#define MAX_MESHES              64
#define SUPPORT_LASTBML

//#define USEPCT
  
    class AnimPatchZ
    {
    public:
        AnimPatchZ()                  : m_patch(NULL),      m_z(0.f) {};
        AnimPatchZ(AnimPatch *_patch) : m_patch(_patch),    m_z(0.f) {};

        AnimPatch * m_patch;
        f32         m_z;
    };

    class AnimPatches
    {
    public:        
        /// processing data
        SafeArray<AnimPatchZ>     m_allPatchs;    //patches that are to be displayed in the current view
#ifdef SUPPORT_LASTBML
        ITF_VECTOR<AnimBMLInfo>   m_lastBMLTab;   //Avoids repeating filling m_allPatchs
        u32                       m_lastState;
#endif //SUPPORT_LASTBML

        AnimPatches();
        ~AnimPatches();

        void                        setPatchs(ITF_VECTOR<AnimBMLInfo> & _bmlTab, const SafeArray<const class AnimTrackExt*> & _trackExtList, u32 _state);
        void                        processPatchs(AnimMeshScene* _pAnimMeshScene, bbool _onWorking = btrue);
    };

} // namespace ITF



#endif //_ITF_ANIMATIONPATCHES_H_

