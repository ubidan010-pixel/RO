#ifndef _ITF_RAYWMWORLDSPOTCOMPONENT_H_
#define _ITF_RAYWMWORLDSPOTCOMPONENT_H_

#ifndef _ITF_RAYWMSPOTCOMPONENT_H_
#include "rayman/gameplay/Components/WorldMap/Ray_WM_SpotComponent.h"
#endif //_ITF_RAYWMSPOTCOMPONENT_H_

namespace ITF
{
    class Ray_WM_WorldSpotComponent : public Ray_WM_SpotComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_WM_WorldSpotComponent, Ray_WM_SpotComponent,3142377630)
        DECLARE_SERIALIZE()

    public:

        Ray_WM_WorldSpotComponent() {};
        virtual ~Ray_WM_WorldSpotComponent() {};

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onSceneActive();
        virtual void        Update( f32 _dt );
        virtual void        Draw();

        virtual void        onEnter(bbool _changeMusic = bfalse);
        virtual void        onExit();
        virtual void        onAction();
        virtual void        onConnection();

    private:

        ITF_INLINE const class Ray_WM_WorldSpotComponent_Template* getTemplate() const;

        void                loadMap();

        ActorRef            m_titleRef;

    };


    //---------------------------------------------------------------------------------------------------

    class Ray_WM_WorldSpotComponent_Template : public Ray_WM_SpotComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_WM_WorldSpotComponent_Template, Ray_WM_SpotComponent_Template,2534612080);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_WM_WorldSpotComponent);

    public:

        Ray_WM_WorldSpotComponent_Template() {}
        virtual ~Ray_WM_WorldSpotComponent_Template() {}

    private:
    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const Ray_WM_WorldSpotComponent_Template* Ray_WM_WorldSpotComponent::getTemplate() const
    {
        return static_cast<const Ray_WM_WorldSpotComponent_Template*>(m_template);
    }
};

#endif //_ITF_RAYWMWORLDSPOTCOMPONENT_H_