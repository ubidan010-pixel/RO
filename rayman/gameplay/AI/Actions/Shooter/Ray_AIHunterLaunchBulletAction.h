#ifndef _ITF_RAY_AIHUNTERLAUNCHBULLETACTION_H_
#define _ITF_RAY_AIHUNTERLAUNCHBULLETACTION_H_

#ifndef _ITF_RAY_AIPERFORMHITACTION_H_
#include "rayman/gameplay/AI/Actions/Ray_AIPerformHitAction.h"
#endif //_ITF_RAY_AIPERFORMHITACTION_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif //_ITF_SPAWNER_H_

namespace ITF
{
    class Ray_AIHunterLaunchBulletAction : public Ray_AIPerformHitAction
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIHunterLaunchBulletAction, Ray_AIPerformHitAction,3971211802)

    public:

        Ray_AIHunterLaunchBulletAction();
        virtual ~Ray_AIHunterLaunchBulletAction();

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onActivate();
        virtual void        update( f32 _delta );
        virtual void        onEvent( Event* _event);

        void                setDirection( const Vec2d& _direction ) { m_direction = _direction; }
        void                setVictim( ObjectRef _victim) { m_victim = _victim; }
        ObjectRef           getVictim() const { return m_victim; }
    protected:

        virtual void        performHit();

    private:

        ITF_INLINE const class Ray_AIHunterLaunchBulletAction_Template * getTemplate() const;

        bbool               m_performHitEnded;
        u32                 m_bulletExitBoneIndex;
        Vec2d               m_direction;
        SpawneeGenerator    m_bulletSpawner;
        ObjectRef           m_victim;
    };

    class Ray_AIHunterLaunchBulletAction_Template : public Ray_AIPerformHitAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIHunterLaunchBulletAction_Template,Ray_AIPerformHitAction_Template,4139579893);
        DECLARE_AIACTION_TEMPLATE(Ray_AIHunterLaunchBulletAction);
        DECLARE_SERIALIZE();

    public:

        Ray_AIHunterLaunchBulletAction_Template();
        ~Ray_AIHunterLaunchBulletAction_Template(){}

        const StringID&     getPerformHitEndMarker() const { return m_performHitEndMarker; }
        const StringID&     getBulletExitBoneName() const { return  m_bulletExitBoneName; }
        const Path&         getBulletPath() const { return m_bulletPath; }
        f32                 getOffset() const { return m_offset; }
        ITF_INLINE f32      getLaunchSpeed() const { return m_launchSpeed; }
        ITF_INLINE bbool    useBoneOrientation() const { return m_useBoneOrientation; }


    private:

        StringID            m_performHitEndMarker;
        StringID            m_bulletExitBoneName;
        Path                m_bulletPath;
        f32                 m_offset;
        f32                 m_launchSpeed;
        bbool               m_useBoneOrientation;
    };

    ITF_INLINE const class Ray_AIHunterLaunchBulletAction_Template * Ray_AIHunterLaunchBulletAction::getTemplate() const
    {
        return static_cast<const class Ray_AIHunterLaunchBulletAction_Template*>(m_template);
    }
}

#endif //_ITF_RAY_AIHUNTERLAUNCHBULLETACTION_H_

