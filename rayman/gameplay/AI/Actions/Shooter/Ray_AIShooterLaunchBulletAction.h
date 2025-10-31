#ifndef _ITF_RAY_AISHOOTERLAUNCHBULLETACTION_H_
#define _ITF_RAY_AISHOOTERLAUNCHBULLETACTION_H_

#ifndef _ITF_RAY_AIPERFORMHITACTION_H_
#include "rayman/gameplay/AI/Actions/Ray_AIPerformHitAction.h"
#endif //_ITF_RAY_AIPERFORMHITACTION_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif //_ITF_SPAWNER_H_


namespace ITF
{
    class Ray_AIShooterLaunchBulletAction : public Ray_AIPerformHitAction
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIShooterLaunchBulletAction, Ray_AIPerformHitAction,289071746);

    public:

        Ray_AIShooterLaunchBulletAction();
        virtual ~Ray_AIShooterLaunchBulletAction();

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onActivate();
        virtual void        update( f32 _delta );
        virtual void        onEvent( Event* _event);

        void                setDirection( const Vec2d& _direction ) { m_direction = _direction; }
        void                setVictim( ObjectRef _victim) { m_victim = _victim; }
        ObjectRef           getVictim() const { return m_victim; }
    
    protected:

        virtual void        onEndMarkerReached();
        virtual void        performHit();

    private:
        ITF_INLINE const class Ray_AIShooterLaunchBulletAction_Template* getTemplate() const;

        bbool               m_performHitEnded;
        //u32                 m_burstCount;
        //u32                 m_burstIndex;
        u32                 m_bulletExitBoneIndex;
        Vec2d               m_direction;
        SpawneeGenerator    m_bulletSpawner;
        ObjectRef           m_victim;
    };

    class Ray_AIShooterLaunchBulletAction_Template : public Ray_AIPerformHitAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIShooterLaunchBulletAction_Template,Ray_AIPerformHitAction_Template,546109621);
        DECLARE_AIACTION_TEMPLATE(Ray_AIShooterLaunchBulletAction);
        DECLARE_SERIALIZE();

    public:

        Ray_AIShooterLaunchBulletAction_Template();
        ~Ray_AIShooterLaunchBulletAction_Template(){}

        //const StringID&     getPerformHitEndMarker() const { return m_performHitEndMarker; }
        //u32                 getMinBurstCount() const { return m_minBurstCount; }            // we can shoot a burst of n rounds
        //u32                 getMaxBurstCount() const { return m_maxBurstCount; }
        ITF_INLINE const StringID&      getBulletExitBoneName() const { return m_bulletExitBoneName; }
        ITF_INLINE const Path&          getBulletPath()         const { return m_bulletPath; }
        ITF_INLINE f32                  getOffset()             const { return m_offset; }
        ITF_INLINE bbool                useBonesEnd()           const { return m_useBonesEnd; }


    private:

        //StringID            m_performHitEndMarker;
        //u32                 m_minBurstCount;            // we can shoot a burst of n rounds
        //u32                 m_maxBurstCount;
        StringID            m_bulletExitBoneName;
        Path                m_bulletPath;
        f32                 m_offset;
        bbool               m_useBonesEnd;
    };

    ITF_INLINE const class Ray_AIShooterLaunchBulletAction_Template * Ray_AIShooterLaunchBulletAction::getTemplate() const
    {
        return static_cast<const class Ray_AIShooterLaunchBulletAction_Template*>(m_template);
    }
}

#endif //_ITF_RAY_AISHOOTERLAUNCHBULLETACTION_H_


