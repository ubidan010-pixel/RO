#ifndef _ITF_BTACTIONJUMPTOBACK_H_
#define _ITF_BTACTIONJUMPTOBACK_H_

#ifndef _ITF_BTACTION_H_
#include "gameplay/ai/BTs/BTAction.h"
#endif // _ITF_BTACTION_H_

namespace ITF
{

class BTActionJumpToBack : public BTAction
{
    DECLARE_OBJECT_CHILD_RTTI(BTActionJumpToBack,BTAction,2367499933);

public:

    BTActionJumpToBack();
    ~BTActionJumpToBack() {}

    virtual void                                init( BehaviorTree* _tree, const BTNode_Template* _template );
    virtual bbool                               validate( Actor* _actor ) const;
    virtual void                                onActivate();
    virtual void                                onDeactivate();
    virtual bbool                               update( f32 _dt );

protected:

    const class BTActionJumpToBack_Template*    getTemplate() const;

    class StickToPolylinePhysComponent*         m_physComponent;
    class AnimLightComponent*                   m_animComponent;
    class GroundAIControllerComponent*          m_aiController;
    Vec3d                                       m_startPos;
    f32                                         m_timeCounter;
};

class BTActionJumpToBack_Template : public BTAction_Template
{
    DECLARE_OBJECT_CHILD_RTTI(BTActionJumpToBack_Template,BTAction_Template,2731024669);
    DECLARE_SERIALIZE();
    DECLARE_BTNODE_TEMPLATE(BTActionJumpToBack);

public:

    BTActionJumpToBack_Template();
    ~BTActionJumpToBack_Template() {}

    ITF_INLINE const StringID&  getAnim() const { return m_anim; }
    ITF_INLINE f32              getTime() const { return m_time; }
    ITF_INLINE f32              getZOffset() const { return m_zOffset; }
    ITF_INLINE f32              getHeight() const { return m_height; }

private:

    StringID                                m_anim;
    f32                                     m_time;
    f32                                     m_zOffset;
    f32                                     m_height;
};

ITF_INLINE const BTActionJumpToBack_Template*  BTActionJumpToBack::getTemplate() const {return static_cast<const BTActionJumpToBack_Template*>(m_template);}

} // namespace ITF

#endif // _ITF_BTACTIONJUMPTOBACK_H_