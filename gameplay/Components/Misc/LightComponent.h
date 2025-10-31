
#ifndef _ITF_LIGHTCOMPONENT_H_
#define _ITF_LIGHTCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

namespace ITF
{
    class Lua;

    enum light_shape
    {
        shape_box,
        shape_circle
    };

    class LightComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(LightComponent,ActorComponent,2312787560);

    public:
        DECLARE_SERIALIZE()

        LightComponent();
        ~LightComponent();
        
        virtual     bbool   needsUpdate         (       ) const     {   return btrue;   }
        virtual     bbool   needsDraw           (       ) const     {   return btrue;  }
		virtual 	bbool 	needsDraw2D			(		) const 	{   return bfalse;  }
        virtual     void    onActorLoaded       (  Pickable::HotReloadType /*_hotReload*/ );
        virtual     void    Update              ( f32 _deltaTime );
        virtual     void    Draw                (       );
#ifdef ITF_SUPPORT_EDITOR
        virtual     void    drawEdit            ( class ActorDrawEditInterface* drawInterface, u32 _flags  ) const;
#endif // ITF_SUPPORT_EDITOR
        virtual     void    updateAABB          (       );
        virtual     void    onActorRemovedFromScene(Scene* _pScene, bbool _forDeletion);
        virtual     void    onBecomeActive      (    );
        virtual     void    onBecomeInactive    (    );
        
        void        processLight                (   BaseObject* _ObjToDraw, ITF_ParticleGenerator* _gen = NULL  );

        ITF_INLINE  Color   getColor            (       ) const;
        ITF_INLINE  f32     getNear             (       ) const;
        ITF_INLINE  f32     getFar              (       ) const;
        //ITF_INLINE  bbool   getUseOnActor       (       ) const;
        //ITF_INLINE  bbool   getUseOnFrize       (       ) const;
        const bbool     getUseBV() const { return m_useBV; }

        LightInfo       m_lightInfo;

    protected:
        f32             m_near;
        f32             m_far;

    private:

        ITF_INLINE const class LightComponent_Template*  getTemplate() const;


        light_shape     m_shape;
        bbool           m_useOnActor;
        bbool           m_useOnFrize;
        bbool           m_useBV;
        f32             m_alpha;
        f32             m_red;
        f32             m_green;
        f32             m_blue;
        Color           m_color;

        AABB            m_farAABB;
    };

    //-------------------------------------------------------------------------------------
    class LightComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(LightComponent_Template,TemplateActorComponent,975033407);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(LightComponent);

    public:

        LightComponent_Template();
        ~LightComponent_Template() {}

        f32             getNear() const { return m_near; }
        f32             getFar() const { return m_far; }
        const String8&  getShape() const { return m_strShape; }
        ITF_INLINE const AABB & getBox() const { return m_box; }
        ITF_INLINE f32 getBoxRange() const { return m_boxRange; }
        ITF_INLINE const Color & getLightColor() const { return m_lightColor; }

    private:

        f32             m_near;
        f32             m_far;
        String8         m_strShape;
        AABB            m_box;
        f32             m_boxRange;
        Color           m_lightColor;
    };



    const LightComponent_Template*  LightComponent::getTemplate() const {return static_cast<const LightComponent_Template*>(m_template);}

    ITF_INLINE  f32     LightComponent::getNear             (       ) const     { return  m_near; }
    ITF_INLINE  f32     LightComponent::getFar              (       ) const     { return  m_far; }
}

#endif // _ITF_LIGHTCOMPONENT_H_

