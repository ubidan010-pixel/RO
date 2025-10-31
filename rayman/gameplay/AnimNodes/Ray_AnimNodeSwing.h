
#ifndef _ITF_RAYANIMNODESWING_H_
#define _ITF_RAYANIMNODESWING_H_

#ifndef _ITF_BLENDTREENODECHOOSEBRANCH_H_
#include "engine/blendTree/BlendTreeNodeChooseBranch.h"
#endif //_ITF_BLENDTREENODECHOOSEBRANCH_H_

#ifndef _ITF_ANIMTREERESULT_H_
#include "engine/blendTree/animTree/AnimTreeResult.h"
#endif //_ITF_ANIMTREERESULT_H_

namespace ITF
{

// Branches:
// 1st branch: Cycle with legs down
// 2nd branch: Cycle with legs up
// 3rd branch: Transition legs down to legs up
// 4th branch: Transition legs up to legs down
// 5th branch: UTurn legs down
// 6th branch: UTurn legs up
// Criterias:
// MoveX == 0       -> No input
// MoveX > 0        -> Swing forward
// MoveX < 0        -> Swing back

class AnimNodeSwing : public BlendTreeNodeChooseBranch <AnimTreeResult>
{
    DECLARE_OBJECT_CHILD_RTTI(AnimNodeSwing,BlendTreeNodeChooseBranch<AnimTreeResult>,1941628304)

public:

    AnimNodeSwing();
    virtual ~AnimNodeSwing() {}

    virtual void                init( BlendTreeInitDataBase* _initData, const BlendTreeNodeTemplate <AnimTreeResult>* _template );
    virtual void                updateResult( f32 _deltaTime, const InputContainer & _inputs, BlendTreeResultBase* _result );
    virtual void                getPlayingNodes( const InputContainer & _inputs, NodeList& _playingNodes );

private:

    enum EBranchCriteria
    {
        BRANCHCRITERIA_NONE = 0,
        BRANCHCRITERIA_FORWARD = 1,
        BRANCHCRITERIA_BACKWARD = 2,
        ENUM_FORCE_SIZE_32(0)
    };

    enum EBranchAction
    {
        IDLE_DOWN = 0,
        IDLE_UP = 1,
        DOWN_TO_UP = 2,
        UP_TO_DOWN = 3,
        UTURN_DOWN = 4,
        UTURN_UP = 5,
        ENUM_FORCE_SIZE_32(1)
    };

    virtual void                updateActiveChild( const InputContainer& _inputs, bbool _force );
    u32                         getActiveCriteria( const InputContainer& _inputs ) const;
    u32                         calculateNewActiveChild( const InputContainer& _inputs, bbool& _force ) const;
};

/////////////////////////////////////////////////////////////////////////////////

class AnimNodeSwing_Template : public BlendTreeNodeChooseBranch_Template <AnimTreeResult>
{
    DECLARE_OBJECT_CHILD_RTTI(AnimNodeSwing_Template,BlendTreeNodeChooseBranch_Template<AnimTreeResult>,4285655022)
    DECLARE_SERIALIZE_VIRTUAL()
    DECLARE_ANIMTREENODE_TEMPLATE(AnimNodeSwing)

public:

    AnimNodeSwing_Template() {}
    ~AnimNodeSwing_Template() {}
    const virtual u32                 getCapacity() const { return 6; }

private:

};

}

#endif // _ITF_RAYANIMNODESWING_H_

