#ifndef _ITF_WAR_PCSTATECRUSHEDBYCHARACTER_H_
#define _ITF_WAR_PCSTATECRUSHEDBYCHARACTER_H_

#ifndef _ITF_WAR_PCSTATE_H_
#include "war/gameplay/Components/Player/PlayerControllerStates/War_PCState.h"
#endif //_ITF_WAR_PCSTATE_H_

class StateCrushedByCharacter : public War_State
{
    DECLARE_OBJECT_CHILD_RTTI(StateCrushedByCharacter,War_State,1381896961)

public:

    StateCrushedByCharacter() {}
    virtual ~StateCrushedByCharacter() {};

    virtual bbool   checkStateChange( f32 _dt, const InputAdapter::PressStatus* _buttons, f32* _axes );

    ITF_INLINE virtual StringID getAnimAction() const
    {
        static const StringID strAction("CRUSHEDBYCHARACTER");

        return strAction;
    }

    void            restart();

private:
};

#endif //_ITF_WAR_PCSTATECRUSHEDBYCHARACTER_H_


