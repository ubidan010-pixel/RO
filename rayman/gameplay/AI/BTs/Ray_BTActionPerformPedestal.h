#ifndef _ITF_RAY_BTACTIONPERFORMPEDESTAL_H_
#define _ITF_RAY_BTACTIONPERFORMPEDESTAL_H_

#ifndef _ITF_BTACTION_H_
#include "gameplay/ai/BTs/BTAction.h"
#endif // _ITF_BTACTION_H_

#ifndef _ITF_PROCEDURALPOLYLINE_H_
#include "gameplay/Components/Environment/ProceduralPolyline.h"
#endif //_ITF_PROCEDURALPOLYLINE_H_

namespace ITF
{

    class Ray_BTActionPerformPedestal : public BTAction
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BTActionPerformPedestal,BTAction,226688569);

    public:

        Ray_BTActionPerformPedestal();
        ~Ray_BTActionPerformPedestal();

        virtual void                            init( BehaviorTree* _tree, const BTNode_Template* _template );
        virtual bbool                           validate( Actor* _actor ) const;
        virtual void                            onActivate();
        virtual void                            onDeactivate();
        virtual bbool                           decide( f32 _dt );
        virtual bbool                           update( f32 _dt );
        virtual void                            updateAnimInput();
        virtual void                            onEvent( class Event* _event );
#ifdef ITF_SUPPORT_EDITOR
        virtual void                            drawEdit( class ActorDrawEditInterface* drawInterface, u32 _flags  ) const;
#endif // ITF_SUPPORT_EDITOR

    private:

        const class Ray_BTActionPerformPedestal_Template*  getTemplate() const;

        void                                    checkTriggerPedestal();
        Vec2d                                   getPedestalPos() const;
        bbool                                   checkNeedJump() const;

        void                                    updatePedestal( f32 _dt );
        void                                    updateJump( f32 _dt );

        void                                    processQueryPedestalInfo( class Ray_EventQueryPedestalInfo * _pedestalSupportCheck );
        void                                    processReleasePedestal( class Ray_EventReleasePedestal * _releasePedestal );
        void                                    processCheckPedestalSupport( class Ray_EventCheckPedestalSupport * _pedestalSupportCheck );

        void                                    setPedestal();
        void                                    setJump();

        static const f32                        s_onPedestalInterpolateDuration;

        AnimatedComponent*                      m_animComponent;
        StickToPolylinePhysComponent*           m_physComponent;
        class GroundAIControllerComponent*      m_aiControllerComponent;
        class PhantomComponent*                 m_phantomComponent;
        mutable u32                             m_supportBoneIndex;
        ObjectRefList                           m_objectsOnPedestal;
        f32                                     m_onPedestalInterpolateCounter;
        Vec2d                                   m_startPedestalPos;
        bbool                                   m_firstUpdate;
        bbool                                   m_onPedestalValid;
        bbool                                   m_doingPedestal;
    };

    class Ray_BTActionPerformPedestal_Template : public BTAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BTActionPerformPedestal_Template,BTAction_Template,171296680);
        DECLARE_SERIALIZE();
        DECLARE_BTNODE_TEMPLATE(Ray_BTActionPerformPedestal);

    public:

        Ray_BTActionPerformPedestal_Template();
        ~Ray_BTActionPerformPedestal_Template() {}

        const StringID&             getAnimPedestal() const { return m_animPedestal; }
        const StringID&             getAnimJump() const { return m_animJump; }
        const StringID&             getWalkTargetFactActor() const { return m_walkTargetFactActor; }
        const StringID&             getWalkTargetFactPos() const { return m_walkTargetFactPos; }
        const StringID&             getSupportBone() const { return m_supportBone; }
        f32                         getAreaRadius() const { return m_areaRadius; }
        f32                         getDistanceCheck() const { return m_distanceCheck; }
        f32                         getFeetDistanceCheck() const { return m_feetDistanceCheck; }

    private:

        StringID                    m_animPedestal;
        StringID                    m_animJump;
        StringID                    m_supportBone;
        StringID                    m_walkTargetFactActor;
        StringID                    m_walkTargetFactPos;
        f32                         m_areaRadius;
        f32                         m_distanceCheck;
        f32                         m_feetDistanceCheck;
    };

    ITF_INLINE const Ray_BTActionPerformPedestal_Template*  Ray_BTActionPerformPedestal::getTemplate() const {return static_cast<const Ray_BTActionPerformPedestal_Template*>(m_template);}

} // namespace ITF

#endif // _ITF_RAY_BTACTIONPERFORMPEDESTAL_H_
