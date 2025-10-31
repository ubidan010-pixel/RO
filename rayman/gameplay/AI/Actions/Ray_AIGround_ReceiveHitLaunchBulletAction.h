#ifndef _ITF_RAY_AIGROUND_RECEIVEHITLAUNCHBULLETACTION_H_
#define _ITF_RAY_AIGROUND_RECEIVEHITLAUNCHBULLETACTION_H_

#ifndef _ITF_RAY_AIGROUND_RECEIVENORMALHITACTION_H_
#include "rayman/gameplay/AI/Actions/Ray_AIGround_ReceiveNormalHitAction.h"
#endif //_ITF_RAY_AIGROUND_RECEIVENORMALHITACTION_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif //_ITF_SPAWNER_H_

namespace ITF
{
    class Ray_AIGround_ReceiveHitLaunchBulletAction : public Ray_AIGround_ReceiveNormalHitAction
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIGround_ReceiveHitLaunchBulletAction, Ray_AIGround_ReceiveNormalHitAction,3889743674);
        DECLARE_VALIDATE_ACTION()

    public:

        Ray_AIGround_ReceiveHitLaunchBulletAction();
        virtual ~Ray_AIGround_ReceiveHitLaunchBulletAction();

        virtual void onActorLoaded(Pickable::HotReloadType _reload);
        virtual void update(f32 _dt);
        virtual void onEvent( Event* _event);

    private:
        ITF_INLINE const class Ray_AIGround_ReceiveHitLaunchBulletAction_Template* getTemplate() const;
        void launchBullet();

        bbool m_launchBullet;
        u32 m_bulletExitBoneIndex;
        SpawneeGenerator m_spawner;
    };

    class Ray_AIGround_ReceiveHitLaunchBulletAction_Template : public Ray_AIGround_ReceiveNormalHitAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIGround_ReceiveHitLaunchBulletAction_Template,Ray_AIGround_ReceiveNormalHitAction_Template,648514903);
        DECLARE_SERIALIZE();
        DECLARE_AIACTION_TEMPLATE(Ray_AIGround_ReceiveHitLaunchBulletAction);

    public:
        Ray_AIGround_ReceiveHitLaunchBulletAction_Template();
        ~Ray_AIGround_ReceiveHitLaunchBulletAction_Template() {}

        ITF_INLINE const Path & getBulletPath() const;
        ITF_INLINE const StringID & getLauncherBoneName() const;
        ITF_INLINE const StringID & getLauncherMarker() const;
        ITF_INLINE f32 getBulletSpeed() const;

    private:
        Path m_bulletPath;
        StringID m_launcherBoneName;
        StringID m_launcherMarker;
        f32 m_bulletSpeed;
    };

    ITF_INLINE const class Ray_AIGround_ReceiveHitLaunchBulletAction_Template* Ray_AIGround_ReceiveHitLaunchBulletAction::getTemplate() const 
    { 
        return static_cast<const Ray_AIGround_ReceiveHitLaunchBulletAction_Template*>(m_template); 
    }

    ITF_INLINE const Path & Ray_AIGround_ReceiveHitLaunchBulletAction_Template::getBulletPath() const
    {
        return m_bulletPath;
    }

    ITF_INLINE const StringID & Ray_AIGround_ReceiveHitLaunchBulletAction_Template::getLauncherBoneName() const
    {
        return m_launcherBoneName;
    }

    ITF_INLINE const StringID & Ray_AIGround_ReceiveHitLaunchBulletAction_Template::getLauncherMarker() const
    {
        return m_launcherMarker;
    }

    ITF_INLINE f32 Ray_AIGround_ReceiveHitLaunchBulletAction_Template::getBulletSpeed() const
    {
        return m_bulletSpeed;
    }
}

#endif //_ITF_RAY_AIGROUND_RECEIVEHITLAUNCHBULLETACTION_H_