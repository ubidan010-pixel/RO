#ifndef _ITF_RAY_BTACTIONDANCEONDOOR_H_
#define _ITF_RAY_BTACTIONDANCEONDOOR_H_

#ifndef _ITF_BTACTION_H_
#include "gameplay/ai/BTs/BTAction.h"
#endif // _ITF_BTACTION_H_

namespace ITF
{

    class Ray_BTActionDanceOnDoor : public BTAction
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BTActionDanceOnDoor,BTAction,2027904792);

    public:

        Ray_BTActionDanceOnDoor();
        ~Ray_BTActionDanceOnDoor();

        virtual void                            init( BehaviorTree* _tree, const BTNode_Template* _template );
        virtual bbool                           validate( Actor* _actor ) const;
        virtual bbool                           decide( f32 _dt );
        virtual void                            onActivate();
        virtual bbool                           update( f32 _dt );
        virtual void                            updateAnimInput();

    private:

        const class Ray_BTActionDanceOnDoor_Template*  getTemplate() const;

        bbool                                   checkCanContinue( bbool& _allBusy, bbool& _mustWait );

        AnimatedComponent*                      m_animComponent;
        ObjectRef                               m_danceStone;
        bbool                                   m_waitingForSync;
    };

    class Ray_BTActionDanceOnDoor_Template : public BTAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BTActionDanceOnDoor_Template,BTAction_Template,1082371704);
        DECLARE_SERIALIZE();
        DECLARE_BTNODE_TEMPLATE(Ray_BTActionDanceOnDoor);

    public:

        Ray_BTActionDanceOnDoor_Template();
        ~Ray_BTActionDanceOnDoor_Template() {}

        StringID                    getAnim() const { return m_anim; }
        f32                         getSyncRatio() const { return m_syncRatio; }
        f32                         getSyncOffset() const { return m_syncOffset; }

    private:

        StringID            m_anim;
        f32                 m_syncRatio;
        f32                 m_syncOffset;
    };

    ITF_INLINE const Ray_BTActionDanceOnDoor_Template*  Ray_BTActionDanceOnDoor::getTemplate() const {return static_cast<const Ray_BTActionDanceOnDoor_Template*>(m_template);}

} // namespace ITF

#endif // _ITF_RAY_BTACTIONDANCEONDOOR_H_
