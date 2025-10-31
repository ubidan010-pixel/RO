#ifndef _ITF_WAR_PCSTATEPUNCHCOMBOBASE_H_
#define _ITF_WAR_PCSTATEPUNCHCOMBOBASE_H_

#ifndef _ITF_WAR_PCSTATEHITBASE_H_
#include "war/gameplay/Components/Player/PlayerControllerStates/War_PCStateHitBase.h"
#endif //_ITF_WAR_PCSTATEHITBASE_H_

class StatePunchComboBase : public StateHitBase
{
    DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(StatePunchComboBase,StateHitBase,908955770)
public:
    StatePunchComboBase();
    ~StatePunchComboBase() {}

    virtual void    onEnter();
    virtual void    onExit();
    virtual void    update(f32 _dt,const InputAdapter::PressStatus* _buttons, f32 * _axes);

protected:

    virtual bbool   canUpdateLookDir() const { return bfalse; }

    f32             m_hitFrequency;
    f32             m_timer;
    bbool           m_justEntered;
};

#endif //_ITF_WAR_PCSTATEPUNCHCOMBOBASE_H_

