#ifndef _ITF_RAY_SEEKINGBULLETAICOMPONENT_H_
#define _ITF_RAY_SEEKINGBULLETAICOMPONENT_H_

#ifndef _ITF_RAY_BULLETAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Bullet/Ray_BulletAIComponent.h"
#endif //_ITF_RAY_BULLETAICOMPONENT_H_

namespace ITF
{
    class Ray_SeekingBulletAIComponent : public Ray_BulletAIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_SeekingBulletAIComponent, Ray_BulletAIComponent,996090817)

    public:
        DECLARE_SERIALIZE();

        Ray_SeekingBulletAIComponent();
        virtual ~Ray_SeekingBulletAIComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onEvent( Event* _event );

    private:
        ITF_INLINE const class Ray_SeekingBulletAIComponent_Template * getTemplate() const;

        virtual void        updateMovement( f32 _dt );
        void                updateAutoSeek( f32 _dt );
        Vec2d               seek( f32 _dt, Actor* target, Vec2d& _currentDirection );

        ActorRef            m_target;
        f32                 m_offset;
        f32                 m_autoSeekCountdown;
    };


//---------------------------------------------------------------------------------------------------------------------------------------

    class Ray_SeekingBulletAIComponent_Template : public Ray_BulletAIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_SeekingBulletAIComponent_Template, Ray_BulletAIComponent_Template,1299182097);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_SeekingBulletAIComponent);

    public:

        Ray_SeekingBulletAIComponent_Template();
        ~Ray_SeekingBulletAIComponent_Template() {}

        ITF_INLINE Angle getMaxTurnAngle() const { return m_maxTurnAngle; }
        ITF_INLINE f32 getPhaseChangeRadius() const { return m_phaseChangeRadius; }
        ITF_INLINE bbool getAutoSeek() const { return m_autoSeek; }
        ITF_INLINE f32 getAutoSeekDelay() const { return m_autoSeekDelay; }
        ITF_INLINE const AABB& getAutoSeekRange() const { return m_autoSeekRange; }
        
    private:

        Angle               m_maxTurnAngle;
        f32                 m_phaseChangeRadius;
        bbool               m_autoSeek;
        f32                 m_autoSeekDelay;
        AABB                m_autoSeekRange;
    };
    ITF_INLINE const class Ray_SeekingBulletAIComponent_Template * Ray_SeekingBulletAIComponent::getTemplate() const
    {
        return static_cast<const class Ray_SeekingBulletAIComponent_Template *>(m_template);
    }
};

#endif //_ITF_RAY_SEEKINGBULLETAICOMPONENT_H_