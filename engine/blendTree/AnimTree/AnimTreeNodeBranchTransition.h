
#ifndef _ITF_ANIMTREENODEBRANCHTRANSITION_H_
#define _ITF_ANIMTREENODEBRANCHTRANSITION_H_

#ifndef _ITF_BLENDTREENODECHOOSEBRANCH_H_
#include "engine/blendTree/BlendTreeNodeChooseBranch.h"
#endif //_ITF_BLENDTREENODECHOOSEBRANCH_H_

#ifndef _ITF_ANIMTREERESULT_H_
#include "engine/blendTree/animTree/AnimTreeResult.h"
#endif //_ITF_ANIMTREERESULT_H_

namespace ITF
{

class AnimTreeNodeBranchTransition : public BlendTreeNodeChooseBranch <AnimTreeResult>
{
    DECLARE_OBJECT_CHILD_RTTI(AnimTreeNodeBranchTransition,BlendTreeNodeChooseBranch<AnimTreeResult>,1390672932);

public:

    AnimTreeNodeBranchTransition();
    virtual ~AnimTreeNodeBranchTransition() {}

    virtual void            onBecomeActive( const InputContainer& _inputs );
    virtual void            updateResult( f32 _deltaTime, const InputContainer & _inputs, BlendTreeResultBase* _result );
    virtual void            updateActiveChild( const InputContainer& _inputs, bbool _force ) {}
    virtual void            getPlayingNodes( const InputContainer & _inputs, NodeList& _playingNodes );

private:

};

/////////////////////////////////////////////////////////////////////////////////

class AnimTreeNodeBranchTransition_Template : public BlendTreeNodeChooseBranch_Template <AnimTreeResult>
{
    DECLARE_OBJECT_CHILD_RTTI(AnimTreeNodeBranchTransition_Template,BlendTreeNodeChooseBranch_Template<AnimTreeResult>,806186784);
    DECLARE_SERIALIZE_VIRTUAL()
    DECLARE_ANIMTREENODE_TEMPLATE(AnimTreeNodeBranchTransition)

public:

    AnimTreeNodeBranchTransition_Template() {}
    ~AnimTreeNodeBranchTransition_Template() {}

private:

};

}

#endif // _ITF_ANIMTREENODEBRANCHTRANSITION_H_

