
#ifndef _ITF_ANIMTREE_H_
#define _ITF_ANIMTREE_H_

#ifndef _ITF_BLENDTREE_H_
#include "engine/blendTree/BlendTree.h"
#endif //_ITF_BLENDTREE_H_

#ifndef _ITF_ANIMTREERESULT_H_
#include "engine/blendTree/animTree/AnimTreeResult.h"
#endif //_ITF_ANIMTREERESULT_H_

namespace ITF
{

class AnimTree : public BlendTree<AnimTreeResult>
{
    typedef BlendTree<AnimTreeResult> Super;

public:
    AnimTree() {}
    ~AnimTree() {}

    bbool                       isAnimFinished() const { return m_currentBlendResult.m_animFinished; }
    bbool                       isAnimLooped() const { return m_currentBlendResult.m_animLooped; }

    const class AnimTree_Template*    getTemplate() const;

private:

    virtual void                updateTransitions( f32 _dt, const InputContainer& _inputs );
};

typedef BlendTreeNode <AnimTreeResult> AnimTreeNode;

/////////////////////////////////////////////////////////////////////////////////

class AnimTree_Template : public BlendTreeTemplate <AnimTreeResult>
{
    DECLARE_OBJECT_CHILD_RTTI(AnimTree_Template,BlendTreeTemplate <AnimTreeResult>,1780851451);
    DECLARE_SERIALIZE();

public:

    AnimTree_Template();
    ~AnimTree_Template() {}

private:
};

ITF_INLINE const AnimTree_Template* AnimTree::getTemplate() const
{
    return static_cast<const AnimTree_Template*>(m_template);
}

}

#endif // _ITF_ANIMTREE_H_

