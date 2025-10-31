
#ifndef _ITF_ANIMTREENODEMARCHBLEND_H_
#define _ITF_ANIMTREENODEMARCHBLEND_H_

#ifndef _ITF_BLENDTREENODECHOOSEBRANCH_H_
#include "engine/blendTree/BlendTreeNodeChooseBranch.h"
#endif //_ITF_BLENDTREENODECHOOSEBRANCH_H_

#ifndef _ITF_INPUTCRITERIAS_H_
#include "engine/blendTree/InputCriterias.h"
#endif //_ITF_INPUTCRITERIAS_H_

#ifndef _ITF_ANIMTREERESULT_H_
#include "engine/blendTree/animTree/AnimTreeResult.h"
#endif //_ITF_ANIMTREERESULT_H_

namespace ITF
{

class AnimTreeNodeMarchBlend : public BlendTreeNodeChooseBranch <AnimTreeResult>
{
    DECLARE_OBJECT_CHILD_RTTI(AnimTreeNodeMarchBlend,BlendTreeNodeChooseBranch<AnimTreeResult>,1885704003);

public:

    AnimTreeNodeMarchBlend();
    virtual ~AnimTreeNodeMarchBlend() {}

    virtual void                init( BlendTreeInitDataBase* _initData, const BlendTreeNodeTemplate <AnimTreeResult>* _template );
    virtual void                updateResult( f32 _deltaTime, const InputContainer & _inputs, BlendTreeResultBase* _result );
    virtual void                onTransitionPlay();

private:

    ITF_INLINE const class AnimTreeNodeMarchBlend_Template* getTemplate() const;

    virtual void                updateActiveChild( const InputContainer& _inputs, bbool _force );

    u32                         m_activeAction;
    u32                         m_activeTransition;
};

/////////////////////////////////////////////////////////////////////////////////

class AnimTreeNodeMarchBlend_Template : public BlendTreeNodeChooseBranch_Template <AnimTreeResult>
{
    DECLARE_OBJECT_CHILD_RTTI(AnimTreeNodeMarchBlend_Template,BlendTreeNodeChooseBranch_Template<AnimTreeResult>,1365303512);
    DECLARE_SERIALIZE_VIRTUAL()
    DECLARE_ANIMTREENODE_TEMPLATE(AnimTreeNodeMarchBlend)

public:

    AnimTreeNodeMarchBlend_Template();
    ~AnimTreeNodeMarchBlend_Template() {}

    f32                         getAnimPosToBlend() const { return m_animPosToBlend; }
    const virtual u32           getCapacity() const { return 4; }

private:

    f32                         m_animPosToBlend;
};

ITF_INLINE const AnimTreeNodeMarchBlend_Template* AnimTreeNodeMarchBlend::getTemplate() const { return static_cast<const AnimTreeNodeMarchBlend_Template*>(m_template); }

}

#endif // _ITF_ANIMTREENODEMARCHBLEND_H_

