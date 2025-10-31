#ifndef _ITF_RAY_PCSTATEPUNCHCOMBOMITRAILLE_H_
#define _ITF_RAY_PCSTATEPUNCHCOMBOMITRAILLE_H_

#ifndef _ITF_RAY_PCSTATEPUNCHCOMBOBASE_H_
#include "rayman/gameplay/Components/Player/PlayerControllerStates/Ray_PCStateHitRelease.h"
#endif //_ITF_RAY_PCSTATEPUNCHCOMBOBASE_H_

class StatePunchComboMitraille : public StateHitRelease
{
    DECLARE_OBJECT_CHILD_RTTI(StatePunchComboMitraille,StateHitRelease,840868636)
public:
    StatePunchComboMitraille() : m_numHits(0), m_burstStage(0), m_canContinue(bfalse), m_comboHitNext(bfalse) {}
    ~StatePunchComboMitraille() {}

    virtual void    onEnter();
    virtual bbool   checkStateChange( f32 _dt );
    virtual void    update(f32 _dt);
    virtual void    updateAnimInputs();
    virtual void    onEvent(Event * _event);

    virtual StringID getAnimAction() const
    {
        return ITF_GET_STRINGID_CRC(PUNCHCOMBOBURST,784262255);
    }

private:

    virtual bbool   canUpdateLookDir() const;
    virtual Vec2d   getBaseHitDir() const;

    u32             m_numHits;
    u32             m_burstStage;
    bbool           m_canContinue;
    bbool           m_comboHitNext;
};

#endif //_ITF_RAY_PCSTATEPUNCHCOMBOMITRAILLE_H_

