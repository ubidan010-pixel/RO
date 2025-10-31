#ifndef _ITF_BTACTIONPLAYANIM_H_
#define _ITF_BTACTIONPLAYANIM_H_

#ifndef _ITF_BTACTION_H_
#include "gameplay/ai/BTs/BTAction.h"
#endif // _ITF_BTACTION_H_

namespace ITF
{

class BTActionPlayAnim : public BTAction
{
    DECLARE_OBJECT_CHILD_RTTI(BTActionPlayAnim,BTAction,908742055);

public:

    BTActionPlayAnim();
    ~BTActionPlayAnim() {}

    virtual void                            init( class BehaviorTree* _tree, const class BTNode_Template* _template );
    virtual bbool                           validate( Actor* _actor ) const;
    virtual void                            onActivate();
    virtual bbool                           update( f32 _dt );

private:

    const class BTActionPlayAnim_Template*  getTemplate() const;

    class AnimLightComponent*               m_animComponent;
    bbool                                   m_firstUpdate;
};

class BTActionPlayAnim_Template : public BTAction_Template
{
    DECLARE_OBJECT_CHILD_RTTI(BTActionPlayAnim_Template,BTAction_Template,2872294888);
    DECLARE_SERIALIZE();
    DECLARE_BTNODE_TEMPLATE(BTActionPlayAnim);

public:

    BTActionPlayAnim_Template() : m_retOnFinish(btrue) {}
    ~BTActionPlayAnim_Template() {}

    const StringID&             getAnim() const { return m_anim; }
    const StringID&             getRestartOnFact() const { return m_restartOnFact; }
    bbool                       getRetOnFinish() const { return m_retOnFinish; }

private:

    StringID                    m_anim;
    StringID                    m_restartOnFact;
    bbool                       m_retOnFinish;
};

ITF_INLINE const BTActionPlayAnim_Template*  BTActionPlayAnim::getTemplate() const {return static_cast<const BTActionPlayAnim_Template*>(m_template);}

} // namespace ITF

#endif // _ITF_BTACTIONPLAYANIM_H_