#ifndef _ITF_ANIMATIONDRAW_H_
#define _ITF_ANIMATIONDRAW_H_

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
    class AnimMeshScene;
    class Texture;
    class renderTarget;
    class AnimInfo;
    class AnimPatchPointDyn;

//#define USEPCT
  
    class ImpostorMode
    {
    private:
        AnimInfo    * m_animInfo;
        renderTarget* m_renderTarget;
        Vec3d         m_savedPos;

    public:
        ImpostorMode();
        ~ImpostorMode();

        void start(AnimInfo *_animInfo, renderTarget* _renderTarget);
    };


    class AnimDraw
    {
    public:        
        /// processing data
        ColorInteger                m_colorPatchTab[4];

        AnimDraw();
        ~AnimDraw();

        // Impostor
        ITF_INLINE renderTarget*    getRenderTargetImpostor() {return m_renderTargetImpostor;};
        Texture*                    setImpostor(u32 _sx, u32 _sy);
        bbool                       checkIfImpostorSizeMustChange(); //check if resize was needed and if so, returns btrue
        static void                 computePatchControlPoints(Vec2d *_NSPoints, AnimPatchPointDyn *_ppDyn[], AnimPatchPoint  *_pp[], u32 _hdiv, u32 _vdiv, bbool _invert);


        bbool                       Draw(AnimMeshScene* _pAnimMeshScene);
        void                        setGlobalMaterialParams(u32 _flags, GFX_MATERIAL_TYPE _mattype, GFX_BLENDMODE _blendmode);
    private:
        bbool                       m_isImpostor;
        renderTarget*               m_renderTargetImpostor;
        u32                         m_width, m_height; //normal size of impostor, as set by setImpostor
        GFX_BLENDMODE               m_globalMaterialBlendMode;
        GFX_MATERIAL_TYPE           m_globalMaterialMatType;
    };

} // namespace ITF



#endif //_ITF_ANIMATIONDRAW_H_

