#ifndef _ITF_RAY_BTACTIONACTIVATESTONE_H_
#define _ITF_RAY_BTACTIONACTIVATESTONE_H_

#ifndef _ITF_BTACTION_H_
#include "gameplay/ai/BTs/BTAction.h"
#endif // _ITF_BTACTION_H_

namespace ITF
{

    class Ray_BTActionActivateStone : public BTAction
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BTActionActivateStone,BTAction,4167552829);

    public:

        Ray_BTActionActivateStone();
        ~Ray_BTActionActivateStone();

        virtual void                            init( BehaviorTree* _tree, const BTNode_Template* _template );
        virtual bbool                           validate( Actor* _actor ) const;
        virtual bbool                           decide( f32 _dt );
        virtual void                            onActivate();
        virtual void                            onDeactivate();
        virtual void                            onEvent( Event* _event );
        virtual bbool                           update( f32 _dt );

    private:

        const class Ray_BTActionActivateStone_Template*  getTemplate() const;

        bbool                                   checkCanContinue( bbool& _allBusy );

        AnimatedComponent*                      m_animComponent;
        ObjectRef                               m_stone;
		ObjectRef								m_trigger;
        bbool                                   m_finished;
        bbool                                   m_finishMarkerReached;
    };

    class Ray_BTActionActivateStone_Template : public BTAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BTActionActivateStone_Template,BTAction_Template,616072720);
        DECLARE_SERIALIZE();
        DECLARE_BTNODE_TEMPLATE(Ray_BTActionActivateStone);

    public:

        Ray_BTActionActivateStone_Template();
        ~Ray_BTActionActivateStone_Template() {}

        StringID                    getAnim() const { return m_anim; }
        StringID                    getFinishAnim() const { return m_finishAnim; }

    private:

        StringID            m_anim;
        StringID            m_finishAnim;
    };

    ITF_INLINE const Ray_BTActionActivateStone_Template*  Ray_BTActionActivateStone::getTemplate() const {return static_cast<const Ray_BTActionActivateStone_Template*>(m_template);}

} // namespace ITF

#endif // _ITF_RAY_BTACTIONACTIVATESTONE_H_
