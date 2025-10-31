#ifndef _ITF_RAY_LANDOFTHEDEADTELEPORTCOMPONENT_H_
#define _ITF_RAY_LANDOFTHEDEADTELEPORTCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

namespace ITF
{
    class Ray_LandOfTheDeadTeleportComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_LandOfTheDeadTeleportComponent, ActorComponent,1349661976)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_LandOfTheDeadTeleportComponent();
        virtual ~Ray_LandOfTheDeadTeleportComponent();

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        Update( f32 _dt );

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

#ifdef ITF_SUPPORT_EDITOR
        virtual void        drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags  ) const;
#endif // ITF_SUPPORT_EDITOR

    private:
        ITF_INLINE const class Ray_LandOfTheDeadTeleportComponent_Template * getTemplate() const;

        void                updateShape();
        Vec2d               getShapePos() const;
        bbool               canTrigger() const;
        void                triggerActors();

        AnimLightComponent* m_animComponent;
        PhysShape*          m_scaledShape;
        bbool               m_enabled;
        bbool               m_firstUpdate;
        bbool               m_activated;
        bbool               m_transition;
    };

    //---------------------------------------------------------------------------------------------------

    class Ray_LandOfTheDeadTeleportComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_LandOfTheDeadTeleportComponent_Template,TemplateActorComponent,657266927);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_LandOfTheDeadTeleportComponent);

    public:

        Ray_LandOfTheDeadTeleportComponent_Template();
        ~Ray_LandOfTheDeadTeleportComponent_Template();

        const StringID&     getMapTag() const { return m_mapTag; }
        const StringID&     getAppearAnim() const { return m_appearAnim; }
        const StringID&     getIdleAnim() const { return m_idleAnim; }

        const PhysShape*    getShape() const { return m_shape; }
        const Vec2d&        getShapeOffset() const { return m_shapeOffset; }

    private:

        StringID            m_mapTag;
        StringID            m_appearAnim;
        StringID            m_idleAnim;

        PhysShape*          m_shape;
        Vec2d               m_shapeOffset;
    };
    ITF_INLINE const class Ray_LandOfTheDeadTeleportComponent_Template * Ray_LandOfTheDeadTeleportComponent::getTemplate() const
    {
        return static_cast<const class Ray_LandOfTheDeadTeleportComponent_Template *>(m_template);
    }
}

#endif //_ITF_RAY_LANDOFTHEDEADTELEPORTCOMPONENT_H_
