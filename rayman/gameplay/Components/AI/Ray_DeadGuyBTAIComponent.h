
#ifndef _ITF_RAY_DEADGUYBTAICOMPONENT_H_
#define _ITF_RAY_DEADGUYBTAICOMPONENT_H_

#ifndef _ITF_BTAICOMPONENT_H_
#include "gameplay/Components/AI/BTAIComponent.h"
#endif //_ITF_BTAICOMPONENT_H_

namespace ITF
{

class Ray_DeadGuyBTAIComponent : public BTAIComponent
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_DeadGuyBTAIComponent,BTAIComponent,3677113079)
    DECLARE_SERIALIZE()

public:

    Ray_DeadGuyBTAIComponent();
    virtual ~Ray_DeadGuyBTAIComponent();

    virtual bbool                       needsUpdate() const { return btrue; }
    virtual bbool                       needsDraw() const { return bfalse; }
    virtual bbool                       needsDraw2D() const { return bfalse; }

    virtual void                        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
    virtual void                        Update( f32 _dt );
    virtual void                        onEvent( Event * _event);
    virtual void                        updateAnimInput();

protected:

    enum ToothState
    {
        TOOTHSTATE_NONE = 0,
        TOOTHSTATE_FLYING,
        TOOTHSTATE_SET,
    };

    const class Ray_DeadGuyBTAIComponent_Template* getTemplate() const;

    void                                checkTriggerSequence();
    void                                openDoor();

    void                                processAnimEventThrowStart();
    void                                processAnimEventThrowEnd();
    void                                processReturnTeeth( class Ray_EventReturnTeeth* _teeth );
    void                                processPunch( class PunchStim* _punch );

    u32                                 m_numTeeth;
    u32                                 m_returningTeeth;
    SafeArray <ToothState>              m_toothState;
    bbool                               m_firstUpdate;
};

class Ray_DeadGuyBTAIComponent_Template : public BTAIComponent_Template
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_DeadGuyBTAIComponent_Template,BTAIComponent_Template,2137520839);
    DECLARE_SERIALIZE();
    DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_DeadGuyBTAIComponent);

public:

    Ray_DeadGuyBTAIComponent_Template();
    ~Ray_DeadGuyBTAIComponent_Template();

    u32                                 getMaxTeeth() const { return m_maxTeeth; }
    f32                                 getTeethSequenceDistance() const { return m_getTeethSequenceDistance; }

private:

    u32                                 m_maxTeeth;
    f32                                 m_getTeethSequenceDistance;
};

ITF_INLINE const class Ray_DeadGuyBTAIComponent_Template* Ray_DeadGuyBTAIComponent::getTemplate() const
{
    return static_cast<const class Ray_DeadGuyBTAIComponent_Template*>(m_template);
}

}

#endif // _ITF_RAY_DEADGUYBTAICOMPONENT_H_

