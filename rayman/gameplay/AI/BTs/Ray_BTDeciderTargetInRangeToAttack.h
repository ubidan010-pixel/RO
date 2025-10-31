#ifndef _ITF_RAY_BTDECIDERTARGETINRANGETOATTACK_H_
#define _ITF_RAY_BTDECIDERTARGETINRANGETOATTACK_H_

#ifndef _ITF_BTDECIDER_H_
#include "gameplay/ai/BTs/BTDecider.h"
#endif // _ITF_BTDECIDER_H_

namespace ITF
{

    class Ray_BTDeciderTargetInRangeToAttack : public BTDecider
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BTDeciderTargetInRangeToAttack,BTDecider,836754755);

    public:

        Ray_BTDeciderTargetInRangeToAttack();
        ~Ray_BTDeciderTargetInRangeToAttack() {}

        virtual void                            init( BehaviorTree* _tree, const BTNode_Template* _template );
        virtual bbool                           validate( Actor* _actor ) const;
        virtual bbool                           decide( f32 _dt );

    private:

        const class Ray_BTDeciderTargetInRangeToAttack_Template*  getTemplate() const;

        class PhantomComponent*                 m_phantomComponent;
    };

    class Ray_BTDeciderTargetInRangeToAttack_Template : public BTDecider_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BTDeciderTargetInRangeToAttack_Template,BTDecider_Template,40903667);
        DECLARE_SERIALIZE();
        DECLARE_BTNODE_TEMPLATE(Ray_BTDeciderTargetInRangeToAttack);

    public:

        Ray_BTDeciderTargetInRangeToAttack_Template();
        ~Ray_BTDeciderTargetInRangeToAttack_Template() {}

        const StringID&             getFact() const { return m_fact; }

    private:

        StringID                    m_fact;
    };

    ITF_INLINE const Ray_BTDeciderTargetInRangeToAttack_Template*  Ray_BTDeciderTargetInRangeToAttack::getTemplate() const {return static_cast<const Ray_BTDeciderTargetInRangeToAttack_Template*>(m_template);}

} // namespace ITF

#endif // _ITF_RAY_BTDECIDERTARGETINRANGETOATTACK_H_
