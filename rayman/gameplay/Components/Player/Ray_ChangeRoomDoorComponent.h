#ifndef _ITF_RAY_CHANGEROOMDOORCOMPONENT_H_
#define _ITF_RAY_CHANGEROOMDOORCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

namespace ITF
{
    class LumDrop;

    class Ray_ChangeRoomDoorComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ChangeRoomDoorComponent, ActorComponent,4156471626)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_ChangeRoomDoorComponent();
        virtual ~Ray_ChangeRoomDoorComponent();

        virtual bbool               needsUpdate()   const { return btrue; }
        virtual bbool               needsDraw()     const { return bfalse; }
        virtual bbool               needsDraw2D()   const { return bfalse; }

        virtual void                onActorLoaded( Pickable::HotReloadType _hotReload );
        virtual void                onActorClearComponents();
        virtual void                Update( f32 _dt );

        void                        setJustUsed() { m_canActivate = bfalse; }

#ifdef ITF_SUPPORT_EDITOR
        virtual void        drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags  ) const;
#endif // ITF_SUPPORT_EDITOR

    private :
        const class Ray_ChangeRoomDoorComponent_Template* getTemplate() const;

        void                        updateShape();
        Vec2d                       getShapePos() const;
        void                        triggerActors();
        void                        activate();

        PhysShape*                  m_scaledShape;
        bbool                       m_canActivate;
        bbool                       m_goIn;
        bbool                       m_goOut;
    };

    class Ray_ChangeRoomDoorComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ChangeRoomDoorComponent_Template, TemplateActorComponent,1528141307);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_ChangeRoomDoorComponent);

    public:

        Ray_ChangeRoomDoorComponent_Template();
        ~Ray_ChangeRoomDoorComponent_Template();

        const PhysShape*            getShape() const { return m_shape; }
        const Vec2d&                getShapeOffset() const { return m_shapeOffset; }

    private:

        PhysShape*                  m_shape;
        Vec2d                       m_shapeOffset;
    };

    ITF_INLINE const Ray_ChangeRoomDoorComponent_Template* Ray_ChangeRoomDoorComponent::getTemplate() const
    {
        return static_cast<const Ray_ChangeRoomDoorComponent_Template*>(m_template);
    }
}

#endif //_ITF_RAY_CHANGEROOMDOORCOMPONENT_H_