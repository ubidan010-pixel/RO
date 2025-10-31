
#ifndef _ITF_ANIMTREENODESEQUENCE_H_
#define _ITF_ANIMTREENODESEQUENCE_H_

#ifndef _ITF_BLENDTREENODEBLEND_H_
#include "engine/blendTree/BlendTreeNodeBlend.h"
#endif //_ITF_BLENDTREENODEBLEND_H_

#ifndef _ITF_BLENDTREENODE_H_
#include "engine/blendTree/BlendTreeNode.h"
#endif //_ITF_BLENDTREENODE_H_

#ifndef _ITF_INPUTCRITERIAS_H_
#include "engine/blendTree/InputCriterias.h"
#endif //_ITF_INPUTCRITERIAS_H_

#ifndef _ITF_ANIMTREERESULT_H_
#include "engine/blendTree/animtree/AnimTreeResult.h"
#endif //_ITF_ANIMTREERESULT_H_

namespace ITF
{

extern const char* AnimTreeNodeSequence_GetClassNameStatic();
extern const char* AnimTreeNodeSequence_Template_GetClassNameStatic();
class AnimTreeNodeSequence_Template;
class AnimTreeNodeSequence : public BlendTreeNodeBlend <AnimTreeResult>
{
    DECLARE_OBJECT_CHILD_RTTI(AnimTreeNodeSequence,BlendTreeNodeBlend<AnimTreeResult>,122885166);

public:
	
	typedef FixedArray < BlendTreeNode <AnimTreeResult>*, 24 > NodeList;

    AnimTreeNodeSequence()
        : m_activeLeaf(U32_INVALID)
        , m_loopCount(0)
        , m_numTimesLooped(0)
    {
    }

    virtual ~AnimTreeNodeSequence() { clear(); }

    virtual void                    getPlayingNodes( const InputContainer & _inputs, NodeList& _playingNodes );
    virtual void                    updateResult( f32 _deltaTime, const InputContainer & _inputs, BlendTreeResultBase* _result );
    virtual void                    onBecomeActive( const InputContainer& _inputs );
    virtual void                    onTransitionPlay();

protected:

    const class AnimTreeNodeSequence_Template * getTemplate() const ;

    void                            setActiveLeaf( const InputContainer& _inputs, u32 leafIndex );

    u32                             m_activeLeaf;
    i32                             m_loopCount;
    i32                             m_numTimesLooped;
};

////////////////////////////////////////////////////////////////////////////////////

class AnimTreeNodeSequence_Template : public BlendTreeNodeBlend_Template <AnimTreeResult>
{
    DECLARE_OBJECT_CHILD_RTTI(AnimTreeNodeSequence_Template,BlendTreeNodeBlend_Template<AnimTreeResult>,895279477);
    DECLARE_SERIALIZE_VIRTUAL()
    DECLARE_ANIMTREENODE_TEMPLATE(AnimTreeNodeSequence)

public:

    AnimTreeNodeSequence_Template() 
        : m_loopCount(0)
        , m_randomizeLoopCount(bfalse)
        , m_randomLoopCountMax(0)
        , m_randomLoopCountMin(0)
    {}
    ~AnimTreeNodeSequence_Template() {}

    ITF_INLINE i32                          getLoopCount() const { return m_loopCount; }
    ITF_INLINE bbool                        getRandomizeLoopCount() const { return m_randomizeLoopCount; }
    ITF_INLINE u32                          getRandomLoopCountMax() const { return m_randomLoopCountMax; }
    ITF_INLINE u32                          getRandomLoopCountMin() const { return m_randomLoopCountMin; }

private:

    i32                         m_loopCount;
    bbool                       m_randomizeLoopCount;
    u32                         m_randomLoopCountMax;
    u32                         m_randomLoopCountMin;
};

ITF_INLINE const AnimTreeNodeSequence_Template * AnimTreeNodeSequence::getTemplate() const { 
        return static_cast<const AnimTreeNodeSequence_Template*>( this->m_template );
}


////////////////////////////////////////////////////////////////////////////////////

}

#endif // _ITF_ANIMTREENODESEQUENCE_H_

