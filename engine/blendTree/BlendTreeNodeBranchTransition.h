
#ifndef _ITF_BLENDTREENODEBRANCHTRANSITION_H_
#define _ITF_BLENDTREENODEBRANCHTRANSITION_H_

#ifndef _ITF_BLENDTREENODEBLEND_H_
#include "engine/blendTree/BlendTreeNodeBlend.h"
#endif //_ITF_BLENDTREENODEBLEND_H_

namespace ITF
{

extern const char* BlendTreeNodeBranchTransition_GetClassNameStatic();
extern const char* BlendTreeNodeBranchTransition_Template_GetClassNameStatic();

template <class RESULTTYPE>
class BlendTreeNodeBranchTransition_Template;

template <class RESULTTYPE>
class BlendTreeNodeBranchTransition : public BlendTreeNodeBlend <RESULTTYPE>
{
    DECLARE_OBJECT_CHILD_TEMPLATE_RTTI(BlendTreeNodeBranchTransition, BlendTreeNodeBlend<RESULTTYPE>,2656567969);

public:

	DECLARE_NODE_LIST
	
    BlendTreeNodeBranchTransition() : m_timeCounter(0.f) {}
    virtual ~BlendTreeNodeBranchTransition() {}

    virtual void                init( BlendTreeInitDataBase* _initData, const BlendTreeNodeTemplate <RESULTTYPE>* _template );
    virtual void                getPlayingNodes( const InputContainer & _inputs, NodeList& _playingNodes );
    virtual void                updateResult( f32 _deltaTime, const InputContainer & _inputs, BlendTreeResultBase* _result );
    virtual void                onBecomeActive( const InputContainer& _inputs );

private:

    const BlendTreeNodeBranchTransition_Template <RESULTTYPE>* getTemplate() const { return static_cast<const BlendTreeNodeBranchTransition_Template <RESULTTYPE>*>(this->m_template); }

    void                        processLeaf( f32 _deltaTime, const InputContainer & _inputs, 
                                             u32 _leafIndex, f32 _weight, bbool _active, bbool _mostSignificant,
                                             BlendTreeResultBase* _result );

    f32                         m_timeCounter;
};

////////////////////////////////////////////////////////////////////////////////////

template <class RESULTTYPE>
class BlendTreeNodeBranchTransition_Template : public BlendTreeNodeBlend_Template <RESULTTYPE>
{
    DECLARE_OBJECT_CHILD_TEMPLATE_RTTI(BlendTreeNodeBranchTransition_Template,BlendTreeNodeBlend_Template<RESULTTYPE>,3100575532);
    DECLARE_SERIALIZE_VIRTUAL()
    DECLARE_BLENDTREENODE_TEMPLATE(BlendTreeNodeBranchTransition)

public:

    BlendTreeNodeBranchTransition_Template() : m_blendTime(0.2f) {}
    ~BlendTreeNodeBranchTransition_Template() {}

    f32                         getBlendTime() const { return m_blendTime; }

private:

    f32                         m_blendTime;
};

BEGIN_SERIALIZATION_CHILD_TEMPLATE(BlendTreeNodeBranchTransition_Template)

SERIALIZE_MEMBER("blendTime",m_blendTime);

END_SERIALIZATION()

////////////////////////////////////////////////////////////////////////////////////

template <class RESULTTYPE>
void BlendTreeNodeBranchTransition<RESULTTYPE>::init( BlendTreeInitDataBase* _initData, const BlendTreeNodeTemplate <RESULTTYPE>* _template )
{
    Super::init(_initData,_template);

    ITF_ASSERT(this->m_leafs.size() == 2);
}

template <class RESULTTYPE>
void BlendTreeNodeBranchTransition<RESULTTYPE>::getPlayingNodes( const InputContainer & _inputs, NodeList& _playingNodes )
{
    if ( !m_timeCounter )
    {
        this->m_leafs[0]->getPlayingNodes(_inputs,_playingNodes);
    }
    else
    {
        this->m_leafs[1]->getPlayingNodes(_inputs,_playingNodes);
    }

    Super::getPlayingNodes(_inputs,_playingNodes);
}

template <class RESULTTYPE>
void BlendTreeNodeBranchTransition<RESULTTYPE>::updateResult( f32 _deltaTime, const InputContainer& _inputs, BlendTreeResultBase* _result )
{
    if ( !m_timeCounter )
    {
        processLeaf(_deltaTime,_inputs,0,1.f,btrue,btrue,_result);
    }
    else if ( m_timeCounter == getTemplate()->getBlendTime() )
    {
        processLeaf(_deltaTime,_inputs,1,1.f,btrue,btrue,_result);
    }
    else
    {
        f32 weight = 1.f - (m_timeCounter / getTemplate()->getBlendTime());

        processLeaf(_deltaTime,_inputs,0,weight,bfalse,weight>0.5f,_result);

        weight = 1.f - weight;

        processLeaf(_deltaTime,_inputs,1,weight,btrue,weight>=0.5f,_result);
    }

    m_timeCounter = Min(m_timeCounter+_deltaTime,getTemplate()->getBlendTime());

    Super::updateResult(_deltaTime,_inputs,_result);
}

template <class RESULTTYPE>
void BlendTreeNodeBranchTransition<RESULTTYPE>::processLeaf( f32 _deltaTime, const InputContainer & _inputs,
                                                             u32 _leafIndex, f32 _weight, bbool _active, bbool _mostSignificant,
                                                             BlendTreeResultBase* _result )
{
    RESULTTYPE localResult;
    this->m_leafs[_leafIndex]->updateResult(_deltaTime,_inputs,&localResult);
    _result->updateBlend(&localResult,_active,_mostSignificant,_weight);
}

template <class RESULTTYPE>
void BlendTreeNodeBranchTransition<RESULTTYPE>::onBecomeActive( const InputContainer& _inputs )
{
    Super::onBecomeActive(_inputs);

    m_timeCounter = 0.f;
    this->m_leafs[0]->onBecomeActive(_inputs);
    this->m_leafs[1]->onBecomeActive(_inputs);
}

}

#endif // _ITF_BLENDTREENODEBRANCHTRANSITION_H_

