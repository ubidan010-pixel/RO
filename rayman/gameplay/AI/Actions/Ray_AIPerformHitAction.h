#ifndef _ITF_RAY_AIPERFORMHITACTION_H_
#define _ITF_RAY_AIPERFORMHITACTION_H_

#ifndef _ITF_AIPERFORMHITACTION_H_
#include "gameplay/AI/Actions/AIPerformHitAction.h"
#endif //_ITF_AIPERFORMHITACTION_H_

namespace ITF
{
    class Ray_AIPerformHitAction : public AIPerformHitAction
    {
        DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(Ray_AIPerformHitAction,AIPerformHitAction,1258927796);

    public:

        Ray_AIPerformHitAction();
        virtual ~Ray_AIPerformHitAction();

        virtual void        update( f32 _delta );
        virtual void        onEvent( Event* _event);
        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);

    protected:
        ITF_INLINE const class Ray_AIPerformHitAction_Template * getTemplate() const;

        virtual void        performHit() = 0;

        bbool               m_performHit;
        ActorRef            m_originalSender;
    };

    class Ray_AIPerformHitAction_Template : public AIPerformHitAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(Ray_AIPerformHitAction_Template,AIPerformHitAction_Template,2485490304);
        DECLARE_SERIALIZE();

    public:

        Ray_AIPerformHitAction_Template();
        ~Ray_AIPerformHitAction_Template(){}

        ITF_INLINE StringID getPerformHitMarker() const { return m_performHitMarker; }
        ITF_INLINE bbool    getUsesAdditive() const { return m_usesAdditive; }
        u32                 getHitLevel() const { return m_hitLevel; }
        EReceivedHitType    getHitType() const { return m_hitType; }
        ITF_INLINE bbool getCanBeCrushed() const { return m_canBeCrushed; }

    private:
        u32                 m_hitLevel;
        EReceivedHitType    m_hitType;
        StringID            m_performHitMarker;
        bbool               m_usesAdditive;
        bbool               m_canBeCrushed;
    };

    ITF_INLINE const class Ray_AIPerformHitAction_Template * Ray_AIPerformHitAction::getTemplate() const
    {
        return static_cast<const class Ray_AIPerformHitAction_Template *>(m_template);
    }
}

#endif //_ITF_RAY_AIPERFORMHITACTION_H_

