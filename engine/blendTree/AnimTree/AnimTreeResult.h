
#ifndef _ITF_ANIMTREERESULT_H_
#define _ITF_ANIMTREERESULT_H_

#ifndef _ITF_ANIMATIONTRACK_H_
#include "engine/animation/AnimationTrack.h"
#endif //_ITF_ANIMATIONTRACK_H_

#ifndef _ITF_BLENDTREERESULT_H_
#include "engine/blendTree/BlendTreeResult.h"
#endif //_ITF_BLENDTREERESULT_H_

namespace ITF
{

//===========================================================================================================
//===========================================================================================================

struct AnimTreeResultLeaf
{
    AnimTreeResultLeaf() : m_subAnim(NULL), m_weight(1.f), m_usePatches(bfalse), m_useEvents(bfalse) {}
    virtual ~AnimTreeResultLeaf() = default;

    virtual void        clear() { m_events.clear(); }
    virtual void        updateBlend( bbool _activeNode, bbool _mostSignificant, f32 _currentWeight );

    class SubAnimFrameInfo* m_subAnim;
    AnimEventContainer      m_events;
    f32                     m_weight;

    bbool                   m_usePatches;
    bbool                   m_useEvents;
};

//===========================================================================================================
//===========================================================================================================

struct AnimTreeResult : public BlendTreeResult <AnimTreeResult>
{
    typedef BlendTreeResult <AnimTreeResult> Super;

    AnimTreeResult() : m_animPos(0.f), m_animFinished(bfalse), m_animLooped(bfalse) {}

    virtual void                        clear() { Super::clear(); m_anims.clear(); m_animFinished = bfalse; m_animLooped = bfalse; m_animPos = 0.f; }
    void                                updateBlend( BlendTreeResultBase* _blendResult, bbool _activeNode,
                                                     bbool _mostSignificant, f32 _currentWeight );

    ITF_VECTOR <AnimTreeResultLeaf>     m_anims;
    f32                                 m_animPos;
    bbool                               m_animFinished;
    bbool                               m_animLooped;
};

//===========================================================================================================
//===========================================================================================================

struct AnimTreeLoadResource : public BlendTreeLoadResourceBase
{
    class SubAnimSet*   m_subAnimSet;
};

//===========================================================================================================
//===========================================================================================================

struct AnimTreeInitData : public BlendTreeInitData <AnimTreeResult>
{
};

//===========================================================================================================
//===========================================================================================================

#define DECLARE_ANIMTREENODE_TEMPLATE(blendTreeNodeClass) \
public: \
    virtual class BlendTreeNode<AnimTreeResult>* createNode() const { return newAlloc(mId_System, blendTreeNodeClass()); } \

}

#endif // _ITF_ANIMTREERESULT_H_

