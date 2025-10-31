#ifndef _ITF_RAY_CAGMAPAMBIANCECOMPONENT_H_
#define _ITF_RAY_CAGMAPAMBIANCECOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

namespace ITF
{
    class Ray_CageMapAmbianceComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_CageMapAmbianceComponent, ActorComponent,1901195588)
        DECLARE_VALIDATE_COMPONENT()
    public:
        DECLARE_SERIALIZE()

        Ray_CageMapAmbianceComponent();
        virtual ~Ray_CageMapAmbianceComponent();

        virtual bbool   needsUpdate() const { return btrue; }
        virtual bbool   needsDraw() const { return btrue; }
        virtual bbool   needsDraw2D() const { return bfalse; }
        virtual void    Update( f32 _deltaTime );

        virtual void    onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void    onEvent(Event * _event);
        virtual void    Draw();

#ifdef ITF_SUPPORT_EDITOR
        virtual void        drawEdit( class ActorDrawEditInterface* /*drawInterface*/, u32 _flags ) const;
#endif // ITF_SUPPORT_EDITOR

    private:
        enum State
        {
            Dark,
            DarkToNeutral,
            Neutral, 
            NeutralToLight,
            Light,
        };
        ITF_INLINE const class Ray_CageMapAmbianceComponent_Template * getTemplate() const;
        void    updateAABB();
        State             m_state;

        bbool             m_forceFieldOpen;
        bbool             m_cageOpen;
        f32               m_timer;
        AFTERFX           m_afterFx;
        Color             m_colorRTarget;
        bbool             m_init;
        bbool             m_lightFadeDone;
        bbool             m_darkFadeDone;
        f32               m_darkAlphaTimer;
        f32               m_lightAlphaTimer;
        class LinkComponent * m_linkComponent;

    };

    //-------------------------------------------------------------------------------------
    class Ray_CageMapAmbianceComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_CageMapAmbianceComponent_Template, TemplateActorComponent,1855819352);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_CageMapAmbianceComponent);

    public:

        Ray_CageMapAmbianceComponent_Template();
        ~Ray_CageMapAmbianceComponent_Template();

        ITF_INLINE Color getDarkColor() const { return m_darkColor; }
        ITF_INLINE Color getNeutralColor() const { return m_neutralColor; }
        ITF_INLINE Color getLightColor() const { return m_lightColor; }
        ITF_INLINE f32 getDarkToNeutralTime() const { return m_darkToNeutralTime; }
        ITF_INLINE f32 getNeutralToLightTime() const { return m_neutralToLightTime; }
        ITF_INLINE i32 getStartRank() const { return m_startRank; }
        ITF_INLINE i32 getRenderRank() const { return m_renderRank; }
        ITF_INLINE f32 getDarkAlphaFadeTime() const { return m_darkAlphaFadeTime; }
        ITF_INLINE f32 getDarkAlphaRadialProgressionSpeed() const { return m_darkAlphaRadialProgressionSpeed; }
        ITF_INLINE f32 getLightAlphaFadeTime() const { return m_lightAlphaFadeTime; }
        ITF_INLINE f32 getLightAlphaRadialProgressionSpeed() const { return m_lightAlphaRadialProgressionSpeed; }
        ITF_INLINE const AABB& getForcedAABB() const { return m_forcedAABB; }

    private:
        Color m_darkColor;
        Color m_neutralColor;
        Color m_lightColor;
        f32 m_darkToNeutralTime;
        f32 m_neutralToLightTime;
        i32 m_startRank;
        i32 m_renderRank;
        f32 m_darkAlphaFadeTime;
        f32 m_darkAlphaRadialProgressionSpeed;
        f32 m_lightAlphaFadeTime;
        f32 m_lightAlphaRadialProgressionSpeed;
        AABB m_forcedAABB;
    };

    ITF_INLINE const class Ray_CageMapAmbianceComponent_Template * Ray_CageMapAmbianceComponent::getTemplate() const
    {
        return static_cast<const class Ray_CageMapAmbianceComponent_Template *>(m_template);
    }

}

#endif // _ITF_RAY_CAGMAPAMBIANCECOMPONENT_H_
