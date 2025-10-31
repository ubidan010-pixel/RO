#ifndef _ITF_RAY_SOFTCOLLISION_H_
#define _ITF_RAY_SOFTCOLLISION_H_

namespace ITF
{
    class AnimLightComponent;

    class Ray_SoftCollision_Template
    {
        DECLARE_SERIALIZE();

    public:
        Ray_SoftCollision_Template();
        virtual ~Ray_SoftCollision_Template();

        ITF_INLINE f32 getRadius() const { return m_radius; }
        ITF_INLINE void setRadius( const f32 _value ) { m_radius = _value; }

        ITF_INLINE f32 getExitSpeed() const { return m_exitSpeed; }
        ITF_INLINE void setExitSpeed( const f32 _value ) { m_exitSpeed = _value; }

        ITF_INLINE f32 getExitForce() const { return m_exitForce; }
        ITF_INLINE void setExitForce( const f32 _value ) { m_exitForce = _value; }

        ITF_INLINE const Vec2d& getOffset() const { return m_offset; }
        ITF_INLINE void setOffset( const Vec2d& _value ) { m_offset = _value; }

    private:

        f32 m_radius;
        f32 m_exitSpeed;
        f32 m_exitForce;
        Vec2d m_offset;
    };

    class Ray_SoftCollision
    {
    public:
        Ray_SoftCollision();
        virtual ~Ray_SoftCollision();

        void init( const Ray_SoftCollision_Template* _template, ActorComponent* _owner );
        void update() const;
        void onEvent( Event* _event ) const;
#ifdef ITF_SUPPORT_EDITOR
        void drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const;
#endif // ITF_SUPPORT_EDITOR

        ITF_INLINE bbool getDisabled() const { return m_disabled; }
        ITF_INLINE void setDisabled( bbool _value ) { m_disabled = _value; }

        ITF_INLINE f32 getRadius() const { return m_radius; }
        ITF_INLINE void setRadius( f32 _value ) { m_radius = _value; }

    private:
        const Ray_SoftCollision_Template* m_template;
        Actor* m_actor;
        AnimLightComponent* m_animComponent;

        bbool m_disabled;
        f32 m_radius;

        enum Orientation
        {
            Orientation_WallLeft,
            Orientation_WallRight,
            Orientation_Ground,
            Orientation_Ceiling
        };

        f32 getScaledRadius() const;
        Vec2d getPos() const;
        Orientation getOrientation() const;
        const Color& getColor( const Orientation _orientation ) const;

        bbool computeForce( const Vec2d& myPos, const f32 myRadius, const Orientation myOrientation, Actor* otherActor, Vec2d& _force ) const;
    };

};

#endif //_ITF_RAY_SOFTCOLLISION_H_