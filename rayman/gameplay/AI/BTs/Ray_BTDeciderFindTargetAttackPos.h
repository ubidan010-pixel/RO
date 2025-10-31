#ifndef _ITF_RAY_BTDECIDERFINDTARGETATTACKPOS_H_
#define _ITF_RAY_BTDECIDERFINDTARGETATTACKPOS_H_

#ifndef _ITF_BTDECIDER_H_
#include "gameplay/ai/BTs/BTDecider.h"
#endif // _ITF_BTDECIDER_H_

namespace ITF
{

    class Ray_BTDeciderFindTargetAttackPos : public BTDecider
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BTDeciderFindTargetAttackPos,BTDecider,4144474367);

    public:

        Ray_BTDeciderFindTargetAttackPos();
        ~Ray_BTDeciderFindTargetAttackPos() {}

        virtual void                            init( BehaviorTree* _tree, const BTNode_Template* _template );
        virtual bbool                           validate( Actor* _actor ) const;
        virtual bbool                           decide( f32 _dt );

    private:

        const class Ray_BTDeciderFindTargetAttackPos_Template*  getTemplate() const;

        void                                    getShapeInfo( const PhysShape* _shape,
                                                              const Vec2d& _pos, f32 _angle,
                                                              f32& _leftSide, f32& _rightSide ) const;

        class PhysComponent*                    m_physComponent;
    };

    class Ray_BTDeciderFindTargetAttackPos_Template : public BTDecider_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BTDeciderFindTargetAttackPos_Template,BTDecider_Template,647085612);
        DECLARE_SERIALIZE();
        DECLARE_BTNODE_TEMPLATE(Ray_BTDeciderFindTargetAttackPos);

    public:

        Ray_BTDeciderFindTargetAttackPos_Template();
        ~Ray_BTDeciderFindTargetAttackPos_Template() {}

        const StringID&     getFactTarget() const { return m_factTarget; }
        const StringID&     getFactPos() const { return m_factPos; }

    private:

        StringID            m_factTarget;
        StringID            m_factPos;
    };

    ITF_INLINE const Ray_BTDeciderFindTargetAttackPos_Template*  Ray_BTDeciderFindTargetAttackPos::getTemplate() const {return static_cast<const Ray_BTDeciderFindTargetAttackPos_Template*>(m_template);}

} // namespace ITF

#endif // _ITF_RAY_BTDECIDERFINDTARGETATTACKPOS_H_
