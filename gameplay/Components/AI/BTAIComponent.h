#ifndef _ITF_BTAICOMPONENT_H_
#define _ITF_BTAICOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_BEHAVIORTREE_H_
#include "gameplay/ai/BTs/BehaviorTree.h"
#endif // _ITF_BEHAVIORTREE_H_

namespace ITF
{

class BTAIComponent : public ActorComponent, IUpdateAnimInputCriteria
{
    DECLARE_OBJECT_CHILD_RTTI(BTAIComponent,ActorComponent,3955454657);
    DECLARE_SERIALIZE()
    DECLARE_VALIDATE_COMPONENT()

public:

    BTAIComponent();
    virtual ~BTAIComponent();

    virtual bbool       needsUpdate() const { return btrue; }
    virtual bbool       needsDraw() const { return bfalse; }
    virtual bbool       needsDraw2D() const { return bfalse; }

    virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
    virtual void        onEvent( Event * _event );
    virtual void        Update( f32 _dt );
    virtual void        updateAnimInput();
#ifdef ITF_SUPPORT_EDITOR
    virtual void        drawEdit( class ActorDrawEditInterface* drawInterface, u32 _flags  ) const;
#endif // ITF_SUPPORT_EDITOR

    void                setDisabled( bbool _val ) { m_disabled = _val; }

protected:

    const class BTAIComponent_Template* getTemplate() const;

    BehaviorTree            m_behaviorTree;
    AnimatedComponent*      m_animatedComponent;
    class PhysComponent*    m_physComponent;
    f32                     m_randomAnim;
    bbool                   m_disabled;
};

class BTAIComponent_Template : public TemplateActorComponent
{
    DECLARE_OBJECT_CHILD_RTTI(BTAIComponent_Template,TemplateActorComponent,2431330779);
    DECLARE_SERIALIZE();
    DECLARE_ACTORCOMPONENT_TEMPLATE(BTAIComponent);

public:

    BTAIComponent_Template();
    ~BTAIComponent_Template();

    const BehaviorTree_Template&        getBehaviorTree() const { return m_behaviorTree; }

private:

    BehaviorTree_Template               m_behaviorTree;
};

ITF_INLINE const class BTAIComponent_Template* BTAIComponent::getTemplate() const
{
    return static_cast<const class BTAIComponent_Template*>(m_template);
}

}

#endif // _ITF_AICOMPONENT_H_