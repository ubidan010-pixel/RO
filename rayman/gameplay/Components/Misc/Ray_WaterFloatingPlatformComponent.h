#ifndef _ITF_RAYWATERFLOATINGPLATFORMCOMPONENT_H_
#define _ITF_RAYWATERFLOATINGPLATFORMCOMPONENT_H_


#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_


namespace ITF
{
    class Ray_WaterFloatingPlatformComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_WaterFloatingPlatformComponent, ActorComponent,2757467585)
        DECLARE_VALIDATE_COMPONENT()
    public:
        DECLARE_SERIALIZE()

        Ray_WaterFloatingPlatformComponent();
        virtual ~Ray_WaterFloatingPlatformComponent();

        virtual bbool   needsUpdate() const { return btrue; }
        virtual bbool   needsDraw() const { return bfalse; }
        virtual bbool   needsDraw2D() const { return bfalse; }
        virtual void    Update( f32 _deltaTime );
        virtual void    onEvent(Event * _event);
        virtual void    onActorLoaded(Pickable::HotReloadType /*_hotReload*/);

    private:
 
        struct User
        {
            User () : m_weight(0.0f) {}
            f32 m_weight;
            ActorRef m_user;
        };

        ITF_INLINE const class Ray_WaterFloatingPlatformComponent_Template * getTemplate() const;

        void                                updateFloat( f32 _dt );
        void                                updatePlatform( f32 _dt );

        void                                applyRotation( f32 _dt );
        void                                applyTranslation( f32 _dt );

        void                                applyLand( ObjectRef _poly, const Vec2d& _pos, const Vec2d& _speed, f32 _weight );
        void                                applyWeightFromPoint( const Vec2d& _pos, f32 _weight, f32& _weightAngle, f32& _weightHeight );

        void                                processStickEvent( class EventStickOnPolyline* _stickEvent );
        void                                processHangEvent( class EventHanging* _hangEvent );

        class ObjectControllerComponent*    m_objectControllerComponent;
        SafeArray<User>                     m_users;
        f32                                 m_forces;

        // Rotation
        f32                                 m_currentTorque;
        f32                                 m_currentAngle;
        f32                                 m_targetAngle;

        // Translation
        f32                                 m_currentSpeed;
        f32                                 m_currentHeight;
        f32                                 m_targetHeight;
    };

    //-------------------------------------------------------------------------------------
    class Ray_WaterFloatingPlatformComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_WaterFloatingPlatformComponent_Template, TemplateActorComponent,2886171535);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_WaterFloatingPlatformComponent);

    public:

        Ray_WaterFloatingPlatformComponent_Template();
        ~Ray_WaterFloatingPlatformComponent_Template();

        f32                         getStiff() const { return m_stiff; }
        f32                         getDamp() const { return m_damp; }
        f32                         getWeightToAngle() const { return m_weightToAngle; }
        f32                         getWeightToHeight() const { return m_weightToHeight; }
        const Angle&                getMaxAngle() const { return m_maxAngle; }
        f32                         getMaxHeight() const { return m_maxHeight; }
        f32                         getWeightMultiplier() const { return m_weightMultiplier; }
        f32                         getForceMultiplier() const { return m_forceMultiplier; }

        f32                         getFloatMinHeight() const { return m_floatMinHeight; }
        f32                         getFloatMaxHeight() const { return m_floatMaxHeight; }
        const Angle&                getFloatMinAngle() const { return m_floatMinAngle; }
        const Angle&                getFloatMaxAngle() const { return m_floatMaxAngle; }

    private:

        f32                         m_stiff;
        f32                         m_damp;
        f32                         m_weightToAngle;
        f32                         m_weightToHeight;
        Angle                       m_maxAngle;
        f32                         m_maxHeight;
        f32                         m_weightMultiplier;
        f32                         m_forceMultiplier;

        f32                         m_floatMinHeight;
        f32                         m_floatMaxHeight;
        Angle                       m_floatMinAngle;
        Angle                       m_floatMaxAngle;
    };

    ITF_INLINE const class Ray_WaterFloatingPlatformComponent_Template * Ray_WaterFloatingPlatformComponent::getTemplate() const
    {
        return static_cast<const class Ray_WaterFloatingPlatformComponent_Template *>(m_template);
    }

}

#endif // _ITF_RAYWATERFLOATINGPLATFORMCOMPONENT_H_
