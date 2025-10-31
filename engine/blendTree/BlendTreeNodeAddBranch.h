
#ifndef _ITF_BLENDTREENODEADDBRANCH_H_
#define _ITF_BLENDTREENODEADDBRANCH_H_

#ifndef _ITF_BLENDTREENODEBLEND_H_
#include "engine/blendTree/BlendTreeNodeBlend.h"
#endif //_ITF_BLENDTREENODEBLEND_H_

namespace ITF
{

extern const char* BlendTreeNodeAddBranch_GetClassNameStatic();
extern const char* BlendTreeNodeAddBranch_Template_GetClassNameStatic();

struct BlendTreeBranchWeight
{
    DECLARE_SERIALIZE()
    BlendTreeBranchWeight() : m_weight(1.0f){}
    f32 m_weight;
    ProceduralInputData m_proceduralWeight;
};

template <class RESULTTYPE>
class BlendTreeNodeAddBranch_Template;

template <class RESULTTYPE>
class BlendTreeNodeAddBranch : public BlendTreeNodeBlend <RESULTTYPE>
{
    DECLARE_OBJECT_CHILD_TEMPLATE_RTTI(BlendTreeNodeAddBranch,BlendTreeNodeBlend<RESULTTYPE>,2629788227);

public:

	DECLARE_NODE_LIST
	
    BlendTreeNodeAddBranch() {}
    virtual ~BlendTreeNodeAddBranch() {}

    virtual void                        init( struct BlendTreeInitDataBase* _initData, const BlendTreeNodeTemplate <RESULTTYPE>* _template );
    virtual void                        getPlayingNodes( const InputContainer & _inputs, NodeList& _playingNodes );
    virtual void                        updateResult( f32 _deltaTime, const InputContainer& _inputs, BlendTreeResultBase* _result );
    virtual void                        onBecomeActive( const InputContainer& _inputs );
    virtual void                        onTransitionPlay();

private:

    const BlendTreeNodeAddBranch_Template <RESULTTYPE>* getTemplate() const;

    f32                                 getBlendValue(u32 _index,f32 _dt, const InputContainer & _inputs);
    ITF_VECTOR<f32>                     m_prevWeights;
};

////////////////////////////////////////////////////////////////////////////////////

template <class RESULTTYPE>
class BlendTreeNodeAddBranch_Template : public BlendTreeNodeBlend_Template <RESULTTYPE>
{
    DECLARE_OBJECT_CHILD_TEMPLATE_RTTI(BlendTreeNodeAddBranch_Template,BlendTreeNodeBlend_Template<RESULTTYPE>,575024261);
    DECLARE_SERIALIZE_VIRTUAL()
    DECLARE_BLENDTREENODE_TEMPLATE(BlendTreeNodeAddBranch)

public:

    BlendTreeNodeAddBranch_Template() {}
    ~BlendTreeNodeAddBranch_Template() {}

    const ITF_VECTOR<BlendTreeBranchWeight>&    getWeights() const { return m_weights; }

private:

    ITF_VECTOR<BlendTreeBranchWeight>   m_weights;
};

BEGIN_SERIALIZATION_CHILD_TEMPLATE(BlendTreeNodeAddBranch_Template)
    SERIALIZE_CONTAINER_OBJECT("weights",m_weights);
END_SERIALIZATION()

////////////////////////////////////////////////////////////////////////////////////

template <class RESULTTYPE>
void BlendTreeNodeAddBranch<RESULTTYPE>::init( struct BlendTreeInitDataBase* _initData, const BlendTreeNodeTemplate <RESULTTYPE>* _template )
{
    Super::init(_initData,_template);

    u32 numWeights = this->m_leafs.size();

    this->m_prevWeights.resize(numWeights);

    for ( u32 i = 0; i < numWeights; i++ )
    {
        this->m_prevWeights[i] = 0.f;
    }
}

template <class RESULTTYPE>
f32 BlendTreeNodeAddBranch<RESULTTYPE>::getBlendValue(u32 _index,f32 _dt,  const InputContainer & _inputs)
{
    f32 blendValue = 1.0f;
    const ITF_VECTOR<BlendTreeBranchWeight>& weights = getTemplate()->getWeights();
    u32 numWeights = weights.size();

    if (_index < numWeights)
    {
        if (weights[_index].m_proceduralWeight.isValid())
            blendValue = weights[_index].m_proceduralWeight.getValue(weights[_index].m_weight,_dt,_inputs);
        else
            blendValue = weights[_index].m_weight;
    }
    return blendValue;
}
template <class RESULTTYPE>
void BlendTreeNodeAddBranch<RESULTTYPE>::getPlayingNodes( const InputContainer & _inputs, NodeList& _playingNodes )
{
    i32 numLeafs = static_cast<i32>(this->m_leafs.size());

    for ( i32 leafCounter = numLeafs-1; leafCounter >= 0; leafCounter-- )
    {
        BlendTreeNode <RESULTTYPE>* node = this->m_leafs[leafCounter];

        if ( node && getBlendValue(leafCounter,LOGICDT,_inputs) )
        {
            node->getPlayingNodes(_inputs,_playingNodes);
        }
    }

    Super::getPlayingNodes(_inputs,_playingNodes);
}

template <class RESULTTYPE>
void BlendTreeNodeAddBranch<RESULTTYPE>::updateResult( f32 _deltaTime, const InputContainer & _inputs, BlendTreeResultBase* _result )
{
    RESULTTYPE* resultTree = static_cast<RESULTTYPE*>(_result);
    u32 numLeafs = BlendTreeNodeBlend<RESULTTYPE>::m_leafs.size();

    RESULTTYPE localResult;

    for ( u32 leafCounter = 0; leafCounter < numLeafs; leafCounter++ )
    {
        BlendTreeNode <RESULTTYPE>* leaf = this->m_leafs[leafCounter];

        if ( !leaf )
        {
            continue;
        }

        localResult.clear();

        f32 currentWeight = getBlendValue(leafCounter,_deltaTime,_inputs);
        f32 prevWeight = this->m_prevWeights[leafCounter];

        if ( prevWeight == 0.f && currentWeight != 0.f )
        {
            leaf->onBecomeActive(_inputs);
        }

        if ( currentWeight )
        {
            leaf->updateResult(_deltaTime,_inputs,&localResult);
            resultTree->updateBlend(&localResult,btrue,btrue,currentWeight);
        }

        this->m_prevWeights[leafCounter] = currentWeight;
    }

    Super::updateResult(_deltaTime,_inputs,_result);
}

template <class RESULTTYPE>
void BlendTreeNodeAddBranch<RESULTTYPE>::onBecomeActive( const InputContainer& _inputs )
{
    u32 numLeafs = this->m_leafs.size();

    for ( u32 leafCounter = 0; leafCounter < numLeafs; leafCounter++ )
    {
        BlendTreeNode <RESULTTYPE>* childNode = this->m_leafs[leafCounter];

        if ( childNode )
        {
            f32 currentWeight = getBlendValue(leafCounter,LOGICDT,_inputs);

            if ( currentWeight )
            {
                childNode->onBecomeActive(_inputs);
            }
        }
    }
}

template <class RESULTTYPE>
void BlendTreeNodeAddBranch<RESULTTYPE>::onTransitionPlay()
{
    u32 numLeafs = this->m_leafs.size();

    for ( u32 leafCounter = 0; leafCounter < numLeafs; leafCounter++ )
    {
        BlendTreeNode <RESULTTYPE>* childNode = this->m_leafs[leafCounter];

        if ( childNode )
        {
            if ( this->m_prevWeights[leafCounter] )
            {
                childNode->onTransitionPlay();
            }
        }
    }

    Super::onTransitionPlay();
}

template <class RESULTTYPE>
const BlendTreeNodeAddBranch_Template <RESULTTYPE>* BlendTreeNodeAddBranch<RESULTTYPE>::getTemplate() const
{
    return static_cast<const BlendTreeNodeAddBranch_Template <RESULTTYPE>*>(this->m_template);
}

}

#endif // _ITF_BLENDTREENODEADDBRANCH_H_

