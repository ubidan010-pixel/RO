#ifndef _ITF_RAY_AIDEATHBEHAVIOR_H_
#define _ITF_RAY_AIDEATHBEHAVIOR_H_

#ifndef _ITF_AIDEATHBEHAVIOR_H_
#include "gameplay/AI/Behaviors/AIDeathBehavior.h"
#endif //_ITF_AIDEATHBEHAVIOR_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif //_ITF_SPAWNER_H_

namespace ITF
{
    class Ray_EventSpawnReward;

    class Ray_AIDeathBehavior : public AIDeathBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIDeathBehavior, AIDeathBehavior,2044426537)

    public:

        Ray_AIDeathBehavior();
        virtual ~Ray_AIDeathBehavior();

        virtual void            onActivate();
        virtual void            onActorLoaded(Pickable::HotReloadType /*_hotReload*/);

        virtual void            onEvent(Event * _event);
    protected:
        void initRewardAndFx();
        void spawnRewardAndFx();
    private:
        ITF_INLINE const class Ray_AIDeathBehavior_Template * getTemplate() const;

        class StickToPolylinePhysComponent* m_physComponent;
        SpawneeGenerator        m_soulSpawner;
        Ray_EventSpawnReward*   m_reward;
    };
    class Ray_AIDeathBehavior_Template : public AIDeathBehavior_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIDeathBehavior_Template,AIDeathBehavior_Template,2099194204);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AIDeathBehavior);

    public:

        Ray_AIDeathBehavior_Template();
        ~Ray_AIDeathBehavior_Template(){SF_DEL(m_reward);}

        ITF_INLINE const Ray_EventSpawnReward* getReward() const { return m_reward; }
        ITF_INLINE const Path & getSoulPath() const { return m_soulPath; }
        ITF_INLINE const SafeArray<u32,4> & getNumRewards() const { return m_numRewards; }
        ITF_INLINE bbool getSpawnOnMarker() const { return m_spawnOnMarker; }


    private:
        Ray_EventSpawnReward*   m_reward;
        Path                    m_soulPath;
        SafeArray<u32,4>        m_numRewards;
        bbool                   m_spawnOnMarker;
    };

    ITF_INLINE const class Ray_AIDeathBehavior_Template * Ray_AIDeathBehavior::getTemplate() const
    {
        return static_cast<const class Ray_AIDeathBehavior_Template *>(m_template);
    }
}

#endif // _ITF_RAY_AIDEATHBEHAVIOR_H_

