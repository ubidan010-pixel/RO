#ifndef _ITF_WAR_PCSTATEPUNCHCOMBOBURST_H_
#define _ITF_WAR_PCSTATEPUNCHCOMBOBURST_H_

#ifndef _ITF_WAR_PCSTATEPUNCHCOMBOBASE_H_
#include "war/gameplay/Components/Player/PlayerControllerStates/War_PCStatePunchComboBase.h"
#endif //_ITF_WAR_PCSTATEPUNCHCOMBOBASE_H_

class StatePunchComboBurst : public StatePunchComboBase
{
    DECLARE_OBJECT_CHILD_RTTI(StatePunchComboBurst,StatePunchComboBase,2120022642)
public:
    StatePunchComboBurst() {}
    ~StatePunchComboBurst() {}

    virtual bbool   checkStateChange( f32 _dt, const InputAdapter::PressStatus* _buttons, f32* _axes );

    ITF_INLINE virtual StringID getAnimAction() const
    {
        static const StringID strAction("PUNCHCOMBOBURST");
        return strAction;
    }

private:

};

#endif //_ITF_WAR_PCSTATEPUNCHCOMBOBURST_H_

