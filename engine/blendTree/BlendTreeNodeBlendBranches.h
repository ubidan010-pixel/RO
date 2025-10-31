
#ifndef _ITF_BLENDTREENODEBLENDBRANCHES_H_
#define _ITF_BLENDTREENODEBLENDBRANCHES_H_

#ifndef _ITF_BLENDTREENODEBLEND_H_
#include "engine/blendTree/BlendTreeNodeBlend.h"
#endif //_ITF_BLENDTREENODEBLEND_H_

namespace ITF
{

extern const char* BlendTreeNodeBlendBranches_GetClassNameStatic();
extern const char* BlendTreeNodeBlendBranches_Template_GetClassNameStatic();

template <class RESULTTYPE>
class BlendTreeNodeBlendBranches_Template;

template <class RESULTTYPE>
class BlendTreeNodeBlendBranches : public BlendTreeNodeBlend <RESULTTYPE>
{
    DECLARE_OBJECT_CHILD_TEMPLATE_RTTI(BlendTreeNodeBlendBranches,BlendTreeNodeBlend<RESULTTYPE>,652455649);

public:
	
	DECLARE_NODE_LIST

    BlendTreeNodeBlendBranches() : m_blendInputIndex(U32_INVALID) {}
    virtual ~BlendTreeNodeBlendBranches() {}

    virtual void                    init( BlendTreeInitDataBase* _initData, const BlendTreeNodeTemplate <RESULTTYPE>* _template );
    virtual void                    getPlayingNodes( const InputContainer & _inputs, NodeList& _playingNodes );
    virtual void                    updateResult( f32 _deltaTime, const InputContainer& _inputs, BlendTreeResultBase* _result );
    virtual void                    onBecomeActive( const InputContainer& _inputs );
    virtual void                    onTransitionPlay();

private:

    const BlendTreeNodeBlendBranches_Template <RESULTTYPE>* getTemplate() const { return static_cast<const BlendTreeNodeBlendBranches_Template <RESULTTYPE>*>(this->m_template); }

    struct BlendLeaf
    {
        BlendLeaf() : m_currentWeight(0.f) {}

        f32 m_currentWeight;
    };

    void                            updateWeights( const InputContainer & _inputs, bbool _force );
    void                            getResultChild( f32 _deltaTime, const InputContainer & _inputs, u32 _leafIndex,
                                                    bbool _getEvents, RESULTTYPE* _result );

    SafeArray <BlendLeaf>           m_blendLeafsData;
    u32                             m_blendInputIndex;
};

////////////////////////////////////////////////////////////////////////////////////

template <class RESULTTYPE>
class BlendTreeNodeBlendBranches_Template : public BlendTreeNodeBlend_Template <RESULTTYPE>
{
    DECLARE_OBJECT_CHILD_TEMPLATE_RTTI(BlendTreeNodeBlendBranches_Template,BlendTreeNodeBlend_Template<RESULTTYPE>,3239327879);
    DECLARE_SERIALIZE_VIRTUAL()
    DECLARE_BLENDTREENODE_TEMPLATE(BlendTreeNodeBlendBranches)

public:

    class BlendLeaf
    {
        DECLARE_SERIALIZE()
    public:

        BlendLeaf() : m_blendParam(0.f) {}

        f32                 getBlendParam() const { return m_blendParam; }

    private:

        f32                 m_blendParam;
    };

    BlendTreeNodeBlendBranches_Template() {}
    ~BlendTreeNodeBlendBranches_Template() {}

    const SafeArray <BlendLeaf>&    getLeafData() const { return m_blendLeafsData; }
    const StringID&                 getBlendInput() const { return m_blendInput; }

private:

    SafeArray <BlendLeaf>           m_blendLeafsData;
    StringID                        m_blendInput;
};

BEGIN_SERIALIZATION_CHILD_TEMPLATE(BlendTreeNodeBlendBranches_Template)

    SERIALIZE_CONTAINER_OBJECT("blendParams",m_blendLeafsData);
    SERIALIZE_MEMBER("blendInput",m_blendInput);

END_SERIALIZATION()

BEGIN_SERIALIZATION_SUBCLASS_TEMPLATE(BlendTreeNodeBlendBranches_Template,BlendLeaf)

    SERIALIZE_MEMBER("blendParam",m_blendParam);

END_SERIALIZATION()

////////////////////////////////////////////////////////////////////////////////////

template <class RESULTTYPE>
void BlendTreeNodeBlendBranches<RESULTTYPE>::init( BlendTreeInitDataBase* _initData, const BlendTreeNodeTemplate <RESULTTYPE>* _template )
{
    Super::init(_initData,_template);

    u32 numLeafData = getTemplate()->getLeafData().size();

    ITF_ASSERT(numLeafData == getTemplate()->getLeafs().size());

    m_blendLeafsData.resize(numLeafData);

    u32 numInputs = _initData->m_inputs->size();

    for ( u32 i = 0; i < numInputs; i++ )
    {
        const Input& input = (*_initData->m_inputs)[i];

        if ( input.getId() == getTemplate()->getBlendInput() )
        {
            m_blendInputIndex = i;
            break;
        }
    }
}

template <class RESULTTYPE>
void BlendTreeNodeBlendBranches<RESULTTYPE>::getPlayingNodes( const InputContainer & _inputs, NodeList& _playingNodes )
{
    u32 leafSize = m_blendLeafsData.size();

    if ( m_blendInputIndex != U32_INVALID )
    {
        if ( leafSize > 1 )
        {
            f32 inputValue;
            const Input& input = _inputs[m_blendInputIndex];

            input.getValue(inputValue);

            for ( u32 leafIndex = 0; leafIndex < leafSize-1; leafIndex++ )
            {
                const typename BlendTreeNodeBlendBranches_Template<RESULTTYPE>::BlendLeaf& nextBlendLeafTemplate = getTemplate()->getLeafData()[leafIndex+1];
                const typename BlendTreeNodeBlendBranches_Template<RESULTTYPE>::BlendLeaf& blendLeafTemplate = getTemplate()->getLeafData()[leafIndex];

                if ( leafIndex == leafSize-2 && nextBlendLeafTemplate.getBlendParam() <= inputValue )
                {
                    this->m_leafs[leafIndex+1]->getPlayingNodes(_inputs,_playingNodes);
                }
                else
                {
                    if ( nextBlendLeafTemplate.getBlendParam() >= inputValue )
                    {
                        f32 dif = nextBlendLeafTemplate.getBlendParam() - blendLeafTemplate.getBlendParam();

                        ITF_ASSERT(dif);

                        inputValue = Max(inputValue,blendLeafTemplate.getBlendParam());

                        f32 weight = 1.f - ( ( inputValue - blendLeafTemplate.getBlendParam() ) / dif );

                        if ( weight > 0.5f )
                        {
                            this->m_leafs[leafIndex]->getPlayingNodes(_inputs,_playingNodes);
                        }

                        if ( ( 1.f - weight ) >= 0.5f )
                        {
                            this->m_leafs[leafIndex+1]->getPlayingNodes(_inputs,_playingNodes);
                        }

                        break;
                    }
                }
            }
        }
    }

    Super::getPlayingNodes(_inputs,_playingNodes);
}

template <class RESULTTYPE>
void BlendTreeNodeBlendBranches<RESULTTYPE>::updateResult( f32 _deltaTime, const InputContainer& _inputs, BlendTreeResultBase* _result )
{
    updateWeights(_inputs,bfalse);

    RESULTTYPE* result = static_cast<RESULTTYPE*>(_result);

    u32 leafSize = m_blendLeafsData.size();

    for ( u32 i = 0; i < leafSize; i++ )
    {
        if ( m_blendLeafsData[i].m_currentWeight == 0.f )
        {
            continue;
        }

        if ( i == (leafSize - 1) || m_blendLeafsData[i].m_currentWeight == 1.f )
        {
            getResultChild(_deltaTime,_inputs,i,btrue,result);
        }
        else
        {
            getResultChild(_deltaTime,_inputs,i,m_blendLeafsData[i].m_currentWeight>=m_blendLeafsData[i+1].m_currentWeight,result);
            getResultChild(_deltaTime,_inputs,i+1,m_blendLeafsData[i].m_currentWeight<m_blendLeafsData[i+1].m_currentWeight,result);
        }

        break;
    }

    Super::updateResult(_deltaTime,_inputs,_result);
}

template <class RESULTTYPE>
void BlendTreeNodeBlendBranches<RESULTTYPE>::getResultChild( f32 _deltaTime, const InputContainer & _inputs, u32 _leafIndex,
                                                                                bbool _activeBranch, RESULTTYPE* _result )
{
    BlendLeaf& blendLeaf = m_blendLeafsData[_leafIndex];
    BlendTreeNode<RESULTTYPE>* childNode = this->m_leafs[_leafIndex];

    if ( !childNode )
    {
        return;
    }

    RESULTTYPE localResult;

    childNode->updateResult(_deltaTime,_inputs,&localResult);

    _result->updateBlend(&localResult,_activeBranch,_activeBranch,blendLeaf.m_currentWeight);
}

template <class RESULTTYPE>
void BlendTreeNodeBlendBranches<RESULTTYPE>::updateWeights( const InputContainer& _inputs, bbool _force )
{
    u32 leafSize = m_blendLeafsData.size();

    if ( leafSize <= 1 )
    {
        return;
    }

    if ( m_blendInputIndex == U32_INVALID )
    {
        LOG("AnimTreeNodeBlendAnim: input not found");
        return;
    }

    f32 inputValue;
    const Input& input = _inputs[m_blendInputIndex];

    input.getValue(inputValue);

    for ( u32 leafIndex = 0; leafIndex < leafSize-1; leafIndex++ )
    {
        const typename BlendTreeNodeBlendBranches_Template<RESULTTYPE>::BlendLeaf& nextBlendLeafTemplate = getTemplate()->getLeafData()[leafIndex+1];
        const typename BlendTreeNodeBlendBranches_Template<RESULTTYPE>::BlendLeaf& blendLeafTemplate = getTemplate()->getLeafData()[leafIndex];
        BlendLeaf& nextBlendLeaf = m_blendLeafsData[leafIndex+1];
        BlendLeaf& blendLeaf = m_blendLeafsData[leafIndex];

        if ( leafIndex == leafSize-2 && nextBlendLeafTemplate.getBlendParam() <= inputValue )
        {
            if ( nextBlendLeaf.m_currentWeight == 0.f || _force )
            {
                this->m_leafs[leafIndex+1]->onBecomeActive(_inputs);
            }

            blendLeaf.m_currentWeight = 0.f;
            nextBlendLeaf.m_currentWeight = 1.f;
        }
        else
        {
            if ( nextBlendLeafTemplate.getBlendParam() < inputValue )
            {
                blendLeaf.m_currentWeight = 0.f;
            }
            else
            {
                f32 dif = nextBlendLeafTemplate.getBlendParam() - blendLeafTemplate.getBlendParam();

                ITF_ASSERT(dif);

                inputValue = Max(inputValue,blendLeafTemplate.getBlendParam());

                f32 weight = 1.f - ( ( inputValue - blendLeafTemplate.getBlendParam() ) / dif );

                if ( weight != 0.f && ( _force || blendLeaf.m_currentWeight == 0.f ) )
                {
                    this->m_leafs[leafIndex]->onBecomeActive(_inputs);
                }

                if ( weight != 1.f && ( _force || nextBlendLeaf.m_currentWeight == 0.f ) )
                {
                    this->m_leafs[leafIndex+1]->onBecomeActive(_inputs);
                }

                blendLeaf.m_currentWeight = weight;
                nextBlendLeaf.m_currentWeight = 1.f - weight;

                for ( u32 i = leafIndex + 2; i < leafSize; i++ )
                {
                    BlendLeaf& blendLeaf = this->m_blendLeafsData[i];

                    blendLeaf.m_currentWeight = 0.f;
                }

                break;
            }
        }
    }
}

template <class RESULTTYPE>
void BlendTreeNodeBlendBranches<RESULTTYPE>::onBecomeActive( const InputContainer& _inputs )
{
    updateWeights(_inputs,btrue);
}

template <class RESULTTYPE>
void BlendTreeNodeBlendBranches<RESULTTYPE>::onTransitionPlay()
{
    u32 numLeafs = this->m_leafs.size();

    for ( u32 leafCounter = 0; leafCounter < numLeafs; leafCounter++ )
    {
        BlendTreeNode <RESULTTYPE>* childNode = this->m_leafs[leafCounter];

        if ( childNode )
        {
            BlendLeaf& blendLeaf = m_blendLeafsData[leafCounter];

            if ( blendLeaf.m_currentWeight )
            {
                childNode->onTransitionPlay();
            }
        }
    }

    Super::onTransitionPlay();
}

}

#endif // _ITF_BLENDTREENODEBLENDBRANCHES_H_

