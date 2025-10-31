#ifndef _ITF_RAY_BTACTIONWAITFORDANCEONDOOR_H_
#define _ITF_RAY_BTACTIONWAITFORDANCEONDOOR_H_

#ifndef _ITF_BTACTION_H_
#include "gameplay/ai/BTs/BTAction.h"
#endif // _ITF_BTACTION_H_

namespace ITF
{

    class Ray_BTActionWaitForDanceOnDoor : public BTAction
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BTActionWaitForDanceOnDoor,BTAction,2562822142);

    public:

        Ray_BTActionWaitForDanceOnDoor();
        ~Ray_BTActionWaitForDanceOnDoor();

        virtual void                            init( BehaviorTree* _tree, const BTNode_Template* _template );
        virtual bbool                           validate( Actor* _actor ) const;
        virtual bbool                           decide( f32 _dt );
        virtual void                            onActivate();
        virtual bbool                           update( f32 _dt );
        virtual void                            updateAnimInput();

    private:

        const class Ray_BTActionWaitForDanceOnDoor_Template*  getTemplate() const;

        bbool                                   checkCanContinue( bbool& _allBusy );

        AnimatedComponent*                      m_animComponent;
        ObjectRef                               m_danceDoor;
        bbool                                   m_waitingForSync;
    };

    class Ray_BTActionWaitForDanceOnDoor_Template : public BTAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BTActionWaitForDanceOnDoor_Template,BTAction_Template,2133261745);
        DECLARE_SERIALIZE();
        DECLARE_BTNODE_TEMPLATE(Ray_BTActionWaitForDanceOnDoor);

    public:

        Ray_BTActionWaitForDanceOnDoor_Template();
        ~Ray_BTActionWaitForDanceOnDoor_Template() {}

        StringID                    getAnim() const { return m_anim; }
        f32                         getSyncRatio() const { return m_syncRatio; }
        f32                         getSyncOffset() const { return m_syncOffset; }

    private:

        StringID            m_anim;
        f32                 m_syncRatio;
        f32                 m_syncOffset;
    };

    ITF_INLINE const Ray_BTActionWaitForDanceOnDoor_Template*  Ray_BTActionWaitForDanceOnDoor::getTemplate() const {return static_cast<const Ray_BTActionWaitForDanceOnDoor_Template*>(m_template);}

} // namespace ITF

#endif // _ITF_RAY_BTACTIONWAITFORDANCEONDOOR_H_
