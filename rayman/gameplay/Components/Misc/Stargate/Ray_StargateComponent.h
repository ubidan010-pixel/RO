#ifndef _ITF_RAY_STARGATECOMPONENT_H_
#define _ITF_RAY_STARGATECOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

namespace ITF
{
    class Ray_StargateComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_StargateComponent, ActorComponent,3525399849)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_StargateComponent();
        virtual ~Ray_StargateComponent();

        virtual bbool               needsUpdate() const { return btrue; }
        virtual bbool               needsDraw() const { return bfalse; }
        virtual bbool               needsDraw2D() const { return bfalse; }

        virtual void                onActorLoaded( Pickable::HotReloadType _reloadType );
        virtual void                Update( f32 _dt );
        virtual void                onEvent( Event * _event);

    private: 
        ITF_INLINE const class Ray_StargateComponent_Template * getTemplate() const;


        void                        useDoor( ObjectRef _triggerer );
        void                        checkUse();
		void						updateTeleportArea();

        class AnimatedComponent*    m_animComponent;
        class DetectorComponent*    m_detectorComponent;
        class LinkComponent*        m_linkComponent;
		PhysShapePolygon			m_teleportArea;
        bbool                       m_activated;
        bbool                       m_used;
    };

    //---------------------------------------------------------------------------------------------------

    class Ray_StargateComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_StargateComponent_Template,TemplateActorComponent,2993411321);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_StargateComponent);

    public:

        Ray_StargateComponent_Template();
        ~Ray_StargateComponent_Template() {}

        f32                     getSpeed() const { return m_speed; }
        const StringID&         getBoneStart() const { return m_startBone; }
		const StringID&			getTeleportAreaName() const { return m_teleportArea; }

    private:

        f32                     m_speed;
        StringID                m_startBone;
		StringID				m_teleportArea;
    };

    ITF_INLINE const class Ray_StargateComponent_Template * Ray_StargateComponent::getTemplate() const
    {
        return static_cast<const class Ray_StargateComponent_Template *>(m_template);
    }

}

#endif //_ITF_RAY_STARGATECOMPONENT_H_
