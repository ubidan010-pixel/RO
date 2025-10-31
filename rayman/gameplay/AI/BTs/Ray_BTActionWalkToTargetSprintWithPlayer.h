#ifndef _ITF_RAY_BTACTIONWALKTOTARGETSPRINTWITHPLAYER_H_
#define _ITF_RAY_BTACTIONWALKTOTARGETSPRINTWITHPLAYER_H_

#ifndef _ITF_BTACTIONWALKTOTARGET_H_
#include "gameplay/ai/BTs/BTActionWalkToTarget.h"
#endif // _ITF_BTACTIONWALKTOTARGET_H_

namespace ITF
{

    class Ray_BTActionWalkToTargetSprintWithPlayer : public BTActionWalkToTarget
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BTActionWalkToTargetSprintWithPlayer,BTActionWalkToTarget,2744887132);

    public:

        Ray_BTActionWalkToTargetSprintWithPlayer();
        ~Ray_BTActionWalkToTargetSprintWithPlayer();

        virtual void            onActivate();
        virtual void            onDeactivate();
        virtual bbool           validate( Actor* _actor ) const;
        virtual bbool           update( f32 _dt );

    private:

        const class Ray_BTActionWalkToTargetSprintWithPlayer_Template*  getTemplate() const;
        class Ray_GroundAIControllerComponent* getAIController() const;

        void                    checkSprinting();

        f32                     m_checkTimer;
        bbool                   m_sprinting;
    };

    class Ray_BTActionWalkToTargetSprintWithPlayer_Template : public BTActionWalkToTarget_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BTActionWalkToTargetSprintWithPlayer_Template,BTActionWalkToTarget_Template,3775712275);
        DECLARE_SERIALIZE();
        DECLARE_BTNODE_TEMPLATE(Ray_BTActionWalkToTargetSprintWithPlayer);

    public:

        Ray_BTActionWalkToTargetSprintWithPlayer_Template();
        ~Ray_BTActionWalkToTargetSprintWithPlayer_Template() {}

        f32                 getCheckIntervals() const { return m_checkIntervals; }
        f32                 getRange() const { return m_range; }

    private:

        f32                 m_checkIntervals;
        f32                 m_range;
    };

    ITF_INLINE const Ray_BTActionWalkToTargetSprintWithPlayer_Template*  Ray_BTActionWalkToTargetSprintWithPlayer::getTemplate() const {return static_cast<const Ray_BTActionWalkToTargetSprintWithPlayer_Template*>(m_template);}

} // namespace ITF

#endif // _ITF_RAY_BTACTIONWALKTOTARGETSPRINTWITHPLAYER_H_
