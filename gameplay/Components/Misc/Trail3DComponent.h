
#ifndef _ITF_TRAIL3DCOMPONENT_H_
#define _ITF_TRAIL3DCOMPONENT_H_

#ifndef _ITF_GRAPHICCOMPONENT_H_
#include "engine/actors/components/graphiccomponent.h"
#endif //_ITF_GRAPHICCOMPONENT_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_BONEID_H_
#include "engine/animation/BoneID.h"
#endif //_ITF_BONEID_H_

#ifndef _ITF_TRAIL_H_
#include "engine/display/Trail.h"
#endif //_ITF_TRAIL_H_


namespace ITF
{
    class Trail3DComponent : public GraphicComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Trail3DComponent,GraphicComponent,2699037183);
        DECLARE_SERIALIZE();

    public:

        Trail3DComponent();
        ~Trail3DComponent();
        virtual bbool   needsUpdate() const { return btrue; }
        virtual bbool   needsDraw() const;
        virtual bbool   needsDraw2D() const;
        virtual void    onBecomeActive();
        virtual void    onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void    Update( f32 _deltaTime );
        virtual void    Draw();
        virtual void    Draw2D();
#ifdef ITF_SUPPORT_EDITOR
        virtual void    onEditorMove(bbool _modifyInitialPos = btrue);
#endif // ITF_SUPPORT_EDITOR

        virtual void    onResourceReady();
        virtual void    onEvent( Event * _event);

        void            clear();
        void            clearDepth() { m_initDepth = btrue; }


        ITF_VECTOR<Trail>& getTrailList() {return m_trailList;}

    private:

        ITF_INLINE const class Trail3DComponent_Template*  getTemplate() const;
        void                getAttachPos( Vec3d& _pos, BoneID & attachedBone );

        ITF_VECTOR<Trail>   m_trailList;
        ITF_VECTOR<BoneID>  m_attachedBoneList;

        AnimLightComponent* m_animComponent;

        bbool               m_trailActive;
        bbool               m_trailFaiding;

        bbool               m_initDepth;
        f32                 m_depth;
    };

    //-------------------------------------------------------------------------------------
    class Trail3DComponent_Template : public GraphicComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Trail3DComponent_Template,GraphicComponent_Template,3314277959);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Trail3DComponent);

    public:

        Trail3DComponent_Template();
        ~Trail3DComponent_Template() {}

        const ITF_VECTOR<Trail_Template>&   getTrailList() const { return m_trailList; }
        bbool                               getStartActive() const { return m_startActive; }
        bbool                               getDraw2D() const {return m_draw2D;}

    private:

        ITF_VECTOR<Trail_Template>          m_trailList;
        bbool                               m_startActive;
        bbool                               m_draw2D;
    };


    const Trail3DComponent_Template*  Trail3DComponent::getTemplate() const {return static_cast<const Trail3DComponent_Template*>(m_template);}
}

#endif // _ITF_TRAIL3DCOMPONENT_H_

