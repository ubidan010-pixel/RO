#ifndef _ITF_AISPAWNACTION_H_
#define _ITF_AISPAWNACTION_H_

#ifndef _ITF_AIACTION_H_
#include "gameplay/ai/Actions/AIAction.h"
#endif //_ITF_AIACTION_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif

namespace ITF
{
    class AISpawnAction : public AIAction
    {
        DECLARE_OBJECT_CHILD_RTTI(AISpawnAction, AIAction,2043362272);

    public:

        AISpawnAction();
        virtual ~AISpawnAction();

        virtual void        onActivate();
        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onEvent(Event * _event);

    private:
        SpawneeGenerator    m_generator;
        Event *             m_onSpawnEvent;
        ActorRef            m_originalSender;
        ITF_INLINE const class AISpawnAction_Template * getTemplate() const;
    };

    class AISpawnAction_Template : public AIAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(AISpawnAction_Template,AIAction_Template,1094967091);
        DECLARE_SERIALIZE();
        DECLARE_AIACTION_TEMPLATE(AISpawnAction);

    public:

        AISpawnAction_Template();
        ~AISpawnAction_Template()                                   { SF_DEL( m_onSpawnEvent ); }
        ITF_INLINE const Path &     getPath() const                 { return m_path; }
        ITF_INLINE StringID         getBonePosName() const          { return m_bonePosName; }
        ITF_INLINE const Event *    getOnSpawnEvent() const         { return m_onSpawnEvent; }
        ITF_INLINE u32              getNbSpawns() const             { return m_nbSpawns; }
        ITF_INLINE const Angle &    getAngleStart() const           { return m_angleStart; }
        ITF_INLINE const Angle &    getAngleStep() const            { return m_angleStep; }

    private:
        Path                m_path;
        StringID            m_bonePosName;
        Event *             m_onSpawnEvent;
        u32                 m_nbSpawns;
        Angle                 m_angleStart;
        Angle                 m_angleStep;
    };

    const class AISpawnAction_Template * AISpawnAction::getTemplate() const
    {
        return static_cast<const class AISpawnAction_Template *>(m_template);
    }
}

#endif // _ITF_AISPAWNACTION_H_

