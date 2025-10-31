#ifndef _ITF_RAY_PLATFORMERCAMERACOMPONENT_H_
#define _ITF_RAY_PLATFORMERCAMERACOMPONENT_H_

#ifndef _ITF_INGAMECAMERACOMPONENT_H_
#include "engine/actors/components/Camera/InGameCameraComponent.h"
#endif //_ITF_INGAMECAMERACOMPONENT_H_

#ifndef _ITF_CAMERACONTROLLERMANAGER_H_
#include "engine/actors/components/Camera/CameraControllerManager.h"
#endif // _ITF_CAMERACONTROLLERMANAGER_H_

namespace ITF
{
    class Ray_PlatformerCameraComponent : public InGameCameraComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_PlatformerCameraComponent, InGameCameraComponent,920536816)

    public:
        DECLARE_SERIALIZE()

        Ray_PlatformerCameraComponent();
        virtual ~Ray_PlatformerCameraComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onEvent( Event* _event );
        virtual void        onCameraEvent( Event* _event );

        virtual void onActorLoaded(Pickable::HotReloadType _hotReload);

#ifdef ITF_SUPPORT_DEBUGFEATURE
        virtual void        drawDebug( const CameraControllerData& _data );
#endif //ITF_SUPPORT_DEBUGFEATURE

    protected:        
        virtual void        setSubjectStance (SubjectStance* _subject);
        virtual bbool       isPlayingPageSequence( ObjectRef _objRef );
        virtual bbool       isPlayingShooter( ObjectRef _objRef );
        void                processQueryLimiter( Ray_EventQueryCameraLimiter* _query );
        Margin              computeLimits( const f32 _margin, const Vec2d& _direction, const bbool _useVerticalLimit );
        MarginBool          computeLimits( const Vec2d& _direction, const bbool _useVerticalLimit );

    private:
        ITF_INLINE const class Ray_PlatformerCameraComponent_Template* getTemplate() const;
    };


    class Ray_PlatformerCameraComponent_Template : public InGameCameraComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_PlatformerCameraComponent_Template,InGameCameraComponent_Template,714787660);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_PlatformerCameraComponent);

    public:

        Ray_PlatformerCameraComponent_Template();
        ~Ray_PlatformerCameraComponent_Template();

        bbool               getUseEjectMargin() const { return m_useEjectMargin; }
        f32                 getEjectMargin() const { return m_ejectMargin; }
        ITF_INLINE bbool    getEjectMarginDetachesPlayer() const { return m_ejectMarginDetachesPlayer; }
        f32                 getEjectForce() const { return m_ejectForce; }

        bbool               getUseDeathMargin() const { return m_useDeathMargin; }
        f32                 getDeathMargin() const { return m_deathMargin; }

        f32                 getTimeOut() const { return m_timeOut; }
        ITF_INLINE bbool    getLastOnscreenPlayerKillsEveryone() const { return m_lastOnscreenPlayerKillsEveryone; }

        bbool               getPursuitUseDeathMargin() const { return m_pursuitUseDeathMargin; }
        f32                 getPursuitDeathMargin() const { return m_pursuitDeathMargin; }

        f32                 getPursuitTimeOut() const { return m_pursuitTimeOut; }

    private:

        bbool   m_useEjectMargin;
        f32     m_ejectMargin;
        f32     m_ejectForce;
        bbool   m_ejectMarginDetachesPlayer;

        bbool   m_useDeathMargin;
        f32     m_deathMargin;

        f32     m_timeOut;
        bbool   m_lastOnscreenPlayerKillsEveryone;

        // Chest2 / Darktoon Cage death margin
        bbool   m_pursuitUseDeathMargin;
        f32     m_pursuitDeathMargin;

        // Chest2 / Darktoon Cage time out
        f32     m_pursuitTimeOut;
    };

    ITF_INLINE const Ray_PlatformerCameraComponent_Template* Ray_PlatformerCameraComponent::getTemplate() const
    {
        return static_cast<const Ray_PlatformerCameraComponent_Template*>(m_template);
    }


}

#endif // _ITF_RAY_PLATFORMERCAMERACOMPONENT_H_

