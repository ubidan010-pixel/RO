
#ifndef _ITF_ANIMTREENODERANDOMBRANCH_H_
#define _ITF_ANIMTREENODERANDOMBRANCH_H_

#ifndef _ITF_BLENDTREENODECHOOSEBRANCH_H_
#include "engine/blendTree/BlendTreeNodeChooseBranch.h"
#endif //_ITF_BLENDTREENODECHOOSEBRANCH_H_

#ifndef _ITF_ANIMTREERESULT_H_
#include "engine/blendTree/animTree/AnimTreeResult.h"
#endif //_ITF_ANIMTREERESULT_H_

namespace ITF
{

class AnimTreeNodeRandomBranch : public BlendTreeNodeChooseBranch <AnimTreeResult>
{
    DECLARE_OBJECT_CHILD_RTTI(AnimTreeNodeRandomBranch,BlendTreeNodeChooseBranch<AnimTreeResult>,2522641518);

public:

    AnimTreeNodeRandomBranch();
    virtual ~AnimTreeNodeRandomBranch() {}

    virtual void            onBecomeActive( const InputContainer& _inputs );
    virtual void            updateResult( f32 _deltaTime, const InputContainer & _inputs, BlendTreeResultBase* _result );
    virtual void            updateActiveChild( const InputContainer& _inputs, bbool _force ) {}
    virtual void            getPlayingNodes( const InputContainer & _inputs, NodeList& _playingNodes );

private:
    ITF_INLINE const class AnimTreeNodeRandomBranch_Template* getTemplate() const;

};

/////////////////////////////////////////////////////////////////////////////////

class AnimTreeNodeRandomBranch_Template : public BlendTreeNodeChooseBranch_Template <AnimTreeResult>
{
    DECLARE_OBJECT_CHILD_RTTI(AnimTreeNodeRandomBranch_Template,BlendTreeNodeChooseBranch_Template<AnimTreeResult>,475419902);
    DECLARE_SERIALIZE_VIRTUAL()
    DECLARE_ANIMTREENODE_TEMPLATE(AnimTreeNodeRandomBranch)

public:

    AnimTreeNodeRandomBranch_Template() : m_loop(btrue) {}
    ~AnimTreeNodeRandomBranch_Template() {}
    ITF_INLINE bbool getLoop() const { return m_loop; }

private:
    bbool   m_loop;
};
ITF_INLINE const AnimTreeNodeRandomBranch_Template* AnimTreeNodeRandomBranch::getTemplate() const { return static_cast<const AnimTreeNodeRandomBranch_Template*>(m_template); }

}

#endif // _ITF_ANIMTREENODERANDOMBRANCH_H_

