#ifndef _ITF_AIIDLEACTION_H_
#define _ITF_AIIDLEACTION_H_

#ifndef _ITF_AIACTION_H_
#include "gameplay/ai/Actions/AIAction.h"
#endif //_ITF_AIACTION_H_

namespace ITF
{
    class AIIdleAction : public AIAction
    {
        DECLARE_OBJECT_CHILD_RTTI(AIIdleAction, AIAction,917643379);

    public:

        AIIdleAction();
        virtual ~AIIdleAction();

        virtual void    onActivate();
        virtual void    update( f32 _dt );

    private:
        ITF_INLINE const class AIIdleAction_Template * getTemplate() const;

        f32     m_idleTime;
        f32     m_timer;
        bbool   m_waitingForSync;
    };

    class AIIdleAction_Template : public AIAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(AIIdleAction_Template,AIAction_Template,1495714450);
        DECLARE_SERIALIZE();
        DECLARE_AIACTION_TEMPLATE(AIIdleAction);

    public:

        AIIdleAction_Template();
        ~AIIdleAction_Template(){}

        ITF_INLINE f32 getMinIdleTime() const { return m_minIdleTime; }
        ITF_INLINE f32 getMaxIdleTime() const { return m_maxIdleTime; }
        ITF_INLINE bbool getStartTimerOnMetronome() const { return m_startTimerOnMetronome; }
        ITF_INLINE f32 getSyncRatio() const { return m_syncRatio; }
        ITF_INLINE f32 getSyncOffset() const { return m_syncOffset; }

    private:
        f32     m_minIdleTime;
        f32     m_maxIdleTime;
        bbool   m_startTimerOnMetronome;
        f32     m_syncRatio;
        f32     m_syncOffset;
    };
    ITF_INLINE const class AIIdleAction_Template * AIIdleAction::getTemplate() const
    {
        return static_cast<const class AIIdleAction_Template *>(m_template);
    }

}

#endif // _ITF_AIIDLEACTION_H_