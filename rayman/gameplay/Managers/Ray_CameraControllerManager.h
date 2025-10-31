#ifndef _ITF_RAY_CAMERACONTROLLERMANAGER_H_
#define _ITF_RAY_CAMERACONTROLLERMANAGER_H_

#ifndef _ITF_CAMERACONTROLLERMANAGER_H_
#include "engine/actors/components/Camera/CameraControllerManager.h"
#endif // _ITF_CAMERACONTROLLERMANAGER_H_

namespace ITF
{
    #define RAY_CAMERACONTROLLERMANAGER static_cast<Ray_CameraControllerManager*>(CameraControllerManager::get())

    class MarginBool
    {
    public:
        MarginBool()
            : m_left(bfalse)
            , m_right(bfalse)
            , m_top(bfalse)
            , m_bottom(bfalse)
        {}
        MarginBool( const bbool _left, const bbool _right, const bbool _top, const bbool _bottom )
            : m_left(_left)
            , m_right(_right)
            , m_top(_top)
            , m_bottom(_bottom)
        {}

        bbool m_left;
        bbool m_right;
        bbool m_top;
        bbool m_bottom;
    };

    class Ray_CameraLimiterData
    {
    public:
        Ray_CameraLimiterData()
            : m_useEjectMargin(bfalse)
            , m_ejectForce(0)
            , m_ejectMarginDetachesPlayer(btrue)
            , m_useDeathMargin(bfalse)
            , m_timeOut(2.f)
            , m_lastOnscreenPlayerKillsEveryone(btrue)
        {}
        ~Ray_CameraLimiterData() {}

        bbool           getUseEjectMargin() const { return m_useEjectMargin; }
        void            setUseEjectMargin( bbool _value ) { m_useEjectMargin = _value; }

        const Margin&   getEjectMargin() const { return m_ejectMargin; }
        void            setEjectMargin( const Margin& _value ) { m_ejectMargin = _value; }
        const MarginBool& getEjectMarginActive() const { return m_ejectMarginActive; }
        void            setEjectMarginActive( const MarginBool& _value ) { m_ejectMarginActive = _value; }

        f32             getEjectForce() const { return m_ejectForce; }
        void            setEjectForce( f32 _value ) { m_ejectForce = _value; }

        ITF_INLINE bbool getEjectMarginDetachesPlayer() const { return m_ejectMarginDetachesPlayer; }
        ITF_INLINE void setEjectMarginDetachesPlayer( const bbool _value ) { m_ejectMarginDetachesPlayer = _value; }

        bbool           getUseDeathMargin() const { return m_useDeathMargin; }
        void            setUseDeathMargin( bbool _value ) { m_useDeathMargin = _value; }

        const Margin&   getDeathMargin() const { return m_deathMargin; }
        void            setDeathMargin( Margin _value ) { m_deathMargin = _value; }

        bbool           getEjectAABB( const f32 _z, AABB& _aabb ) const;
        bbool           getDeathAABB( const f32 _z, AABB& _aabb ) const;

        ITF_INLINE f32  getTimeOut() const { return m_timeOut; }
        ITF_INLINE void setTimeOut( const f32 _value ) { m_timeOut = _value; }

        ITF_INLINE bbool getLastOnscreenPlayerKillsEveryone() const { return m_lastOnscreenPlayerKillsEveryone; }
        ITF_INLINE void setLastOnscreenPlayerKillsEveryone( const bbool _value ) { m_lastOnscreenPlayerKillsEveryone = _value; }

        ITF_INLINE const MarginBool& getConstraintActive() const { return m_constraintActive; }
        ITF_INLINE void setConstraintActive( const MarginBool& _value ) { m_constraintActive = _value; }
        void            setConstraint( const CameraModifierComponent* _cameraModifier );

    private:

        bbool   m_useEjectMargin;
        Margin  m_ejectMargin;
        MarginBool m_ejectMarginActive;
        f32     m_ejectForce;
        bbool   m_ejectMarginDetachesPlayer;

        bbool   m_useDeathMargin;
        Margin  m_deathMargin;
        f32     m_timeOut;
        bbool   m_lastOnscreenPlayerKillsEveryone;

        AABB    m_constraintAABB;
        MarginBool m_constraintActive;
    };

    class Ray_CameraControllerManager : public CameraControllerManager
    {
    public:

        static void                     create()    { s_instance = newAlloc(mId_Gameplay, Ray_CameraControllerManager()); }

        Ray_CameraControllerManager();
        virtual ~Ray_CameraControllerManager();

        virtual void    onEndUpdateProcess( f32 _dt ); // inherited from Interface_WorldEventsListener

        ITF_INLINE const Ray_CameraLimiterData& getLimiterData() const { return m_limiterData; }

    private:

        Ray_CameraLimiterData m_limiterData;

    };
}

#endif //_ITF_RAY_CAMERACONTROLLERMANAGER_H_