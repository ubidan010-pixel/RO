#ifndef _ITF_WAR_PCSTATEPUNCHCOMBOACCROBATIC_H_
#define _ITF_WAR_PCSTATEPUNCHCOMBOACCROBATIC_H_

#ifndef _ITF_WAR_PCSTATEPUNCHCOMBOBASE_H_
#include "war/gameplay/Components/Player/PlayerControllerStates/War_PCStatePunchComboBase.h"
#endif //_ITF_WAR_PCSTATEPUNCHCOMBOBASE_H_

class StatePunchComboAccrobatic : public StatePunchComboBase
{
    DECLARE_OBJECT_CHILD_RTTI(StatePunchComboAccrobatic,StatePunchComboBase,984030797)
public:
    StatePunchComboAccrobatic() : m_stage(0), m_nextHit(bfalse) {}
    ~StatePunchComboAccrobatic() {}

    virtual void    onEnter();
    virtual bbool   checkStateChange( f32 _dt, const InputAdapter::PressStatus* _buttons, f32* _axes );
    virtual void    update(f32 _dt,const InputAdapter::PressStatus* _buttons, f32 * _axes);
    virtual void    updateAnimInputs();

    ITF_INLINE virtual StringID getAnimAction() const
    {
        static const StringID strAction("PUNCHCOMBOACCROBATIC");

        return strAction;
    }

private:
    
    u32                     m_stage;
    bbool                   m_nextHit;
};

#endif //_ITF_WAR_PCSTATEPUNCHCOMBOACCROBATIC_H_

