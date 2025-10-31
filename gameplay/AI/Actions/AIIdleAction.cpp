#include "precompiled_gameplay.h"

#ifndef _ITF_AIIDLEACTION_H_
#include "gameplay/ai/Actions/AIIdleAction.h"
#endif //_ITF_AIIDLEACTION_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_METRONOMEMANAGER_H_
#include "engine/sound/MetronomeManager.h"
#endif //_ITF_METRONOMEMANAGER_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(AIIdleAction_Template)

    BEGIN_SERIALIZATION_CHILD(AIIdleAction_Template)
        SERIALIZE_MEMBER("minTime", m_minIdleTime);
        SERIALIZE_MEMBER("maxTime", m_maxIdleTime);
        SERIALIZE_MEMBER("startTimerOnMetronome", m_startTimerOnMetronome);
        SERIALIZE_MEMBER("syncRatio", m_syncRatio);
        SERIALIZE_MEMBER("syncOffset", m_syncOffset);
    END_SERIALIZATION()

        AIIdleAction_Template::AIIdleAction_Template()
        : m_minIdleTime(0.f)
        , m_maxIdleTime(0.f)
        , m_startTimerOnMetronome(bfalse)
        , m_syncRatio(1.f)
        , m_syncOffset(0.f)
    {
        m_animDefaultAction = ITF_GET_STRINGID_CRC(IDLE,1633200266);
    }


    IMPLEMENT_OBJECT_RTTI(AIIdleAction)

    AIIdleAction::AIIdleAction()
        : Super()
        , m_idleTime(0)
        , m_timer(0)
        , m_waitingForSync(btrue)
    {
    }

    AIIdleAction::~AIIdleAction()
    {
    }

    void AIIdleAction::onActivate()
    {
        Super::onActivate();

        if (getTemplate()->getMinIdleTime() >= 0 && getTemplate()->getMaxIdleTime()> 0)
            m_idleTime = Seeder::getSharedSeeder().GetFloat(getTemplate()->getMinIdleTime(), getTemplate()->getMaxIdleTime());
        else
            m_idleTime = 0;
        m_timer = 0;
        m_waitingForSync = btrue;
    }

    void AIIdleAction::update( f32 _dt )
    {
        Super::update(_dt);
        if (m_idleTime > 0)
        {
            if (getTemplate()->getStartTimerOnMetronome() && m_waitingForSync)
            {
                f32 metronome = METRONOME_MANAGER->getBarPercentage(getTemplate()->getSyncOffset(), getTemplate()->getSyncRatio());
                if (metronome < _dt * 2)
                {
                    m_waitingForSync = bfalse;
                }
                else
                {
                    return;
                }
            }

            m_timer += _dt;
            if (m_timer > m_idleTime)
            {
                m_aiBehavior->onActionFinished();
            }
        }
    }

}