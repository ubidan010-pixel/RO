#ifndef _ITF_RAYBASCULEPLATFORMCOMPONENT_H_
#define _ITF_RAYBASCULEPLATFORMCOMPONENT_H_

namespace ITF
{
    class Ray_BasculePlatformComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BasculePlatformComponent, ActorComponent,2501389937)
        DECLARE_VALIDATE_COMPONENT()
    public:
        DECLARE_SERIALIZE()

        Ray_BasculePlatformComponent();
        virtual ~Ray_BasculePlatformComponent();

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

        ITF_INLINE const class Ray_BasculePlatformComponent_Template * getTemplate() const;

        void                                applyLand( ObjectRef _poly, const Vec2d& _pos, const Vec2d& _speed, f32 _weight, bbool _isCrushAttack );
        void                                applyWeightFromPoint( const Vec2d& _pos, f32 _weight );

        void                                processStickEvent( class EventStickOnPolyline* _stickEvent );
        void                                processHangEvent( class EventHanging* _hangEvent );

        SafeArray<User>                     m_users;
        f32                                 m_forces;
        f32                                 m_totalWeight;
        f32                                 m_currentTorque;
        f32                                 m_currentAngle;
    };

    //-------------------------------------------------------------------------------------
    class Ray_BasculePlatformComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BasculePlatformComponent_Template, TemplateActorComponent,3575633478);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_BasculePlatformComponent);

    public:

        Ray_BasculePlatformComponent_Template();
        ~Ray_BasculePlatformComponent_Template();

        f32                         getStiff() const { return m_stiff; }
        f32                         getDamp() const { return m_damp; }
        f32                         getWeightToAngle() const { return m_weightToAngle; }
        const Angle&                getMaxAngle() const { return m_maxAngle; }
        f32                         getWeightMultiplier() const { return m_weightMultiplier; }
        f32                         getForceMultiplier() const { return m_forceMultiplier; }
        f32                         getCrushMultiplier() const { return m_crushMultiplier; }

    private:

        f32                         m_stiff;
        f32                         m_damp;
        f32                         m_weightToAngle;
        Angle                       m_maxAngle;
        f32                         m_weightMultiplier;
        f32                         m_forceMultiplier;
        f32                         m_crushMultiplier;
    };

    ITF_INLINE const class Ray_BasculePlatformComponent_Template * Ray_BasculePlatformComponent::getTemplate() const
    {
        return static_cast<const class Ray_BasculePlatformComponent_Template *>(m_template);
    }

}

#endif // _ITF_RAYBASCULEPLATFORMCOMPONENT_H_
