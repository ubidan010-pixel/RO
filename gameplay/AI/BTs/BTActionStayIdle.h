#ifndef _ITF_BTACTIONSTAYIDLE_H_
#define _ITF_BTACTIONSTAYIDLE_H_

#ifndef _ITF_BTACTION_H_
#include "gameplay/ai/BTs/BTAction.h"
#endif // _ITF_BTACTION_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

namespace ITF
{

class BTActionStayIdle : public BTAction
{
    DECLARE_OBJECT_CHILD_RTTI(BTActionStayIdle,BTAction,1024087394);

public:

    BTActionStayIdle();
    ~BTActionStayIdle() {}

    virtual void                                init( BehaviorTree* _tree, const BTNode_Template* _template );
    virtual bbool                               validate( Actor* _actor ) const;
    virtual bbool                               update( f32 _dt );
    virtual void                                onActivate();

protected:

    enum StayState
    {
        STAYSTATE_GROUND,
        STAYSTATE_FALL,
        STAYSTATE_SWIM,
    };

    const class BTActionStayIdle_Template*      getTemplate() const;

    class PolyLine*                             getSwimPolyline() const;
    void                                        checkAvoidance();

    class StickToPolylinePhysComponent*         m_physComponent;
    class AnimLightComponent*                   m_animComponent;
    class GroundAIControllerComponent*          m_aiController;
    class PhantomComponent*                     m_phantomComponent;
    AIUtils::PolylineRegionList                 m_currentRegions;
    StayState                                   m_prevState;
    bbool                                       m_avoiding;
    f32                                         m_avoidanceTimer;
};

class BTActionStayIdle_Template : public BTAction_Template
{
    DECLARE_OBJECT_CHILD_RTTI(BTActionStayIdle_Template,BTAction_Template,2301954430);
    DECLARE_SERIALIZE();
    DECLARE_BTNODE_TEMPLATE(BTActionStayIdle);

public:

    BTActionStayIdle_Template();
    ~BTActionStayIdle_Template() {}

    ITF_INLINE const StringID&  getGroundAnim() const { return m_groundAnim; }
    ITF_INLINE const StringID&  getSwimAnim() const { return m_swimAnim; }
    ITF_INLINE const StringID&  getFallAnim() const { return m_fallAnim; }
    ITF_INLINE f32              getAvoidanceRadius() const { return m_avoidanceRadius; }

private:

    StringID                                m_groundAnim;
    StringID                                m_swimAnim;
    StringID                                m_fallAnim;
    f32                                     m_avoidanceRadius;
};

ITF_INLINE const BTActionStayIdle_Template*  BTActionStayIdle::getTemplate() const {return static_cast<const BTActionStayIdle_Template*>(m_template);}

} // namespace ITF

#endif // _ITF_BTACTIONSTAYIDLE_H_