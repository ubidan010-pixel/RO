
#ifndef _ITF_ANIMTREENODEPLAYANIM_H_
#define _ITF_ANIMTREENODEPLAYANIM_H_

#ifndef _ITF_BLENDTREENODE_H_
#include "engine/blendTree/BlendTreeNode.h"
#endif //_ITF_BLENDTREENODE_H_

#ifndef _ITF_ANIMTREERESULT_H_
#include "engine/blendTree/animTree/AnimTreeResult.h"
#endif //_ITF_ANIMTREERESULT_H_

#ifndef _ITF_SUBANIMFRAMEINFO_H_
#include "engine/animation/SubAnimFrameInfo.h"
#endif //_ITF_SUBANIMFRAMEINFO_H_

namespace ITF
{

class AnimTreeNodePlayAnim : public BlendTreeNode <AnimTreeResult>
{
    DECLARE_OBJECT_CHILD_RTTI(AnimTreeNodePlayAnim,BlendTreeNode<AnimTreeResult>,1500494396);
public:

    AnimTreeNodePlayAnim() : m_firstUpdate(btrue) {}
    virtual ~AnimTreeNodePlayAnim() {}

    virtual void            onLoadResource( BlendTreeLoadResourceBase* _resData );
    virtual void            updateResult( f32 _deltaTime, const InputContainer & _inputs, BlendTreeResultBase* _result );
    virtual void            onBecomeActive( const InputContainer& _inputs );
    virtual bbool           validate() const;

    void                    setReverse( bbool _reverse );
    bbool                   getIsReversed() const;

protected:

    ITF_INLINE const class AnimTreeNodePlayAnim_Template* getTemplate() const;

    virtual void            setCursorPos( f32 _deltaTime, const InputContainer& _inputs );
    virtual void            setPlayRate( f32 _deltaTime, const InputContainer& _inputs );

    SubAnimFrameInfo        m_anim;
    bbool                   m_firstUpdate;
};

/////////////////////////////////////////////////////////////////////////////////

class AnimTreeNodePlayAnim_Template : public BlendTreeNodeTemplate <AnimTreeResult>
{
    DECLARE_OBJECT_CHILD_RTTI(AnimTreeNodePlayAnim_Template,BlendTreeNodeTemplate<AnimTreeResult>,3928059907);
    DECLARE_SERIALIZE_VIRTUAL()
    DECLARE_ANIMTREENODE_TEMPLATE(AnimTreeNodePlayAnim)

public:

    AnimTreeNodePlayAnim_Template() : m_usePatches(btrue), m_useEvents(btrue) {}
    ~AnimTreeNodePlayAnim_Template() {}

    const StringID&                 getAnimationName() const { return m_animationName; }
    const ProceduralInputData&      getProceduralCursor() const { return m_proceduralCursor; }
    const ProceduralInputData&      getProceduralPlayRate() const { return m_proceduralPlayRate; }
    bbool                           getUsePatches() const { return m_usePatches; }
    bbool                           getUseEvents() const { return m_useEvents; }

private:

    StringID                m_animationName;
    ProceduralInputData     m_proceduralCursor;
    ProceduralInputData     m_proceduralPlayRate;
    bbool                   m_usePatches;
    bbool                   m_useEvents;
};

ITF_INLINE const AnimTreeNodePlayAnim_Template* AnimTreeNodePlayAnim::getTemplate() const { return static_cast<const AnimTreeNodePlayAnim_Template*>(m_template); }

}

#endif // _ITF_ANIMTREENODEPLAYANIM_H_

