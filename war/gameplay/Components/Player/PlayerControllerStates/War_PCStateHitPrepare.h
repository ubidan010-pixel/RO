#ifndef _ITF_WAR_PCSTATEHITPREPARE_H_
#define _ITF_WAR_PCSTATEHITPREPARE_H_

#ifndef _ITF_WAR_PCSTATEHITBASE_H_
#include "war/gameplay/Components/Player/PlayerControllerStates/War_PCStateHitBase.h"
#endif //_ITF_WAR_PCSTATEHITBASE_H_

class StateHitPrepare : public StateHitBase
{
    DECLARE_OBJECT_CHILD_RTTI(StateHitPrepare,StateHitBase,2530625326)
public:
    StateHitPrepare()
        : m_prevPunchLevel(0)
        , m_uturnTimer(0)
        , m_justEntered(bfalse)
    {}
    ~StateHitPrepare() {}

    virtual void                onEnter();
    virtual void                onExit();
    virtual bbool               checkStateChange( f32 _dt, const InputAdapter::PressStatus* _buttons, f32* _axes );
    virtual void                update(f32 _dt,const InputAdapter::PressStatus* _buttons, f32 * _axes);

    ITF_INLINE virtual StringID getAnimAction() const
    {
        static const StringID strAction("PREPAREHIT");
        return strAction;
    }

protected:

    u32                         m_prevPunchLevel;               // The level of the punch we are performing now
    f32                         m_uturnTimer;                   // We allow a small time to transform the hit into a uturn hit
    bbool                       m_justEntered;
};

#endif //_ITF_WAR_PCSTATEHITPREPARE_H_


