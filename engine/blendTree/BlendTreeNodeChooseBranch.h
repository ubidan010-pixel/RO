
#ifndef _ITF_BLENDTREENODECHOOSEBRANCH_H_
#define _ITF_BLENDTREENODECHOOSEBRANCH_H_

#ifndef _ITF_BLENDTREENODEBLEND_H_
#include "engine/blendTree/BlendTreeNodeBlend.h"
#endif //_ITF_BLENDTREENODEBLEND_H_

#ifndef _ITF_INPUTCRITERIAS_H_
#include "engine/blendTree/InputCriterias.h"
#endif //_ITF_INPUTCRITERIAS_H_

namespace ITF
{

extern const char* BlendTreeNodeChooseBranch_GetClassNameStatic();
extern const char* BlendTreeNodeChooseBranch_Template_GetClassNameStatic();

template <class RESULTTYPE>
class BlendTreeNodeChooseBranch_Template;

template <class RESULTTYPE>
class BlendTreeNodeChooseBranch : public BlendTreeNodeBlend <RESULTTYPE>
{
    DECLARE_OBJECT_CHILD_TEMPLATE_RTTI(BlendTreeNodeChooseBranch,BlendTreeNodeBlend<RESULTTYPE>,2837374494);

public:
	
	DECLARE_NODE_LIST

    BlendTreeNodeChooseBranch()
    : m_timeCounter(0.f)
    , m_activeLeaf(U32_INVALID)
    , m_mostSignificantLeaf(0)
    {
    }

    virtual ~BlendTreeNodeChooseBranch() { clear(); }

    virtual void                    init( BlendTreeInitDataBase* _initData, const BlendTreeNodeTemplate <RESULTTYPE>* _template );
    virtual void                    getPlayingNodes( const InputContainer & _inputs, NodeList& _playingNodes );
    virtual void                    updateResult( f32 _deltaTime, const InputContainer & _inputs, BlendTreeResultBase* _result );
    virtual void                    onBecomeActive( const InputContainer& _inputs );
    virtual void                    onTransitionPlay();

protected:

    const BlendTreeNodeChooseBranch_Template <RESULTTYPE>* getTemplate() const { return static_cast<const BlendTreeNodeChooseBranch_Template <RESULTTYPE>*>(this->m_template); }

    struct BlendLeaf 
    {
        BlendLeaf() : m_startWeight(0.f), m_targetWeight(0.f), m_currentWeight(0.f) {}

        f32                     m_startWeight;
        f32                     m_targetWeight;
        f32                     m_currentWeight;

        CriteriaArray           m_criteria;
    };

    typedef ITF_VECTOR <BlendLeaf> ChildDataContainer;

    void                        clear();
    void                        processLeaf( u32 _leafIndex, f32 _deltaTime, const InputContainer& _inputs, RESULTTYPE* _result );
    void                        setActiveLeaf( const InputContainer& _inputs, u32 leafIndex, bbool _force );
    virtual void                processBlend( f32 _deltaTime, const InputContainer & _inputs, BlendTreeResultBase* _result );
    virtual void                updateActiveChild( const InputContainer& _inputs, bbool _force );
    void                        updateBlend( f32 _deltaTime );

    f32                         m_timeCounter;
    u32                         m_activeLeaf;
    u32                         m_mostSignificantLeaf;
    ChildDataContainer          m_childData;
};

////////////////////////////////////////////////////////////////////////////////////

template <class RESULTTYPE>
class BlendTreeNodeChooseBranch_Template : public BlendTreeNodeBlend_Template <RESULTTYPE>
{
    DECLARE_OBJECT_CHILD_TEMPLATE_RTTI(BlendTreeNodeChooseBranch_Template,BlendTreeNodeBlend_Template<RESULTTYPE>,2519193777);
    DECLARE_SERIALIZE_VIRTUAL()
    DECLARE_BLENDTREENODE_TEMPLATE(BlendTreeNodeChooseBranch)

public:

    BlendTreeNodeChooseBranch_Template() : m_blendTime(0.f), m_startingLeaf(U32_INVALID), m_noUpdateInactive(bfalse) {}
    ~BlendTreeNodeChooseBranch_Template() {}

    struct BlendLeaf 
    {
        DECLARE_SERIALIZE()

        BlendLeaf() {}

        CriteriaDescArray       m_criteriaDesc;
    };

    typedef ITF_VECTOR <BlendLeaf> ChildDataContainer;

    f32                         getBlendTime() const { return m_blendTime; }
    u32                         getStartingLeaf() const { return m_startingLeaf; }
    const ChildDataContainer&   getChildData() const { return m_childData; }
    bbool                       getNoUpdateInactive() const { return m_noUpdateInactive; }

private:

    f32                         m_blendTime;
    u32                         m_startingLeaf;
    ChildDataContainer          m_childData;
    bbool                       m_noUpdateInactive;
};

BEGIN_SERIALIZATION_CHILD_TEMPLATE(BlendTreeNodeChooseBranch_Template)

    SERIALIZE_MEMBER("blendTime",m_blendTime);
    SERIALIZE_MEMBER("startingLeaf",m_startingLeaf);
    SERIALIZE_CONTAINER_OBJECT("leafsCriterias",m_childData);
    SERIALIZE_MEMBER("noUpdateInactive",m_noUpdateInactive);

END_SERIALIZATION()

BEGIN_SERIALIZATION_SUBCLASS_TEMPLATE(BlendTreeNodeChooseBranch_Template,BlendLeaf)

    SERIALIZE_CONTAINER_OBJECT("criterias",m_criteriaDesc);

END_SERIALIZATION()

////////////////////////////////////////////////////////////////////////////////////

template <class RESULTTYPE>
void BlendTreeNodeChooseBranch<RESULTTYPE>::clear()
{
    for ( typename ChildDataContainer::iterator it = m_childData.begin(); it != m_childData.end(); ++it )
    {
        BlendLeaf& childData = *it;

        for (CriteriaArray::const_iterator it = childData.m_criteria.begin(); it != childData.m_criteria.end(); ++it)
        {
            delete (*it);
        }
    }

    m_childData.clear();
}

template <class RESULTTYPE>
void BlendTreeNodeChooseBranch<RESULTTYPE>::init( BlendTreeInitDataBase* _initData, const BlendTreeNodeTemplate <RESULTTYPE>* _template )
{
    Super::init(_initData,_template);

    if ( this->m_leafs.size() != m_childData.size() )
    {
        m_childData.resize(this->m_leafs.size());
    }

    const typename BlendTreeNodeChooseBranch_Template<RESULTTYPE>::ChildDataContainer& childDataTemplate = getTemplate()->getChildData();
    typename BlendTreeNodeChooseBranch_Template<RESULTTYPE>::ChildDataContainer::const_iterator itTemplate = childDataTemplate.begin();
    typename ChildDataContainer::iterator it = m_childData.begin();

    for ( ; it != m_childData.end() && itTemplate != childDataTemplate.end(); ++it, ++itTemplate )
    {
        const typename BlendTreeNodeChooseBranch_Template<RESULTTYPE>::BlendLeaf& blendLeafTemplate = *itTemplate;
        BlendLeaf& blendLeaf = *it;

        for ( u32 i = 0; i < blendLeafTemplate.m_criteriaDesc.size(); i++ )
        {
            const CriteriaDesc& desc = blendLeafTemplate.m_criteriaDesc[i];
            Criteria* crit = Criteria::createCriteria(desc,_initData->m_inputs);

            if ( crit )
            {
                blendLeaf.m_criteria.push_back(crit);
            }
        }

        blendLeaf.m_currentWeight = 0.f;
        blendLeaf.m_startWeight = 0.f;
    }
}

template <class RESULTTYPE>
void BlendTreeNodeChooseBranch<RESULTTYPE>::getPlayingNodes( const InputContainer & _inputs, NodeList& _playingNodes )
{
    if ( this->getTemplate()->getIgnoreRuleChange() && m_activeLeaf != U32_INVALID )
    {
        BlendTreeNode <RESULTTYPE>* node = this->m_leafs[m_activeLeaf];
        node->getPlayingNodes(_inputs,_playingNodes);
    }
    else
    {
        u32 leafIndex = 0;

        for ( typename ChildDataContainer::iterator it = m_childData.begin(); it != m_childData.end(); ++it, leafIndex++ )
        {
            BlendLeaf& childData = *it;

            if ( Criteria::matchCriteria(childData.m_criteria) )
            {
                BlendTreeNode <RESULTTYPE>* node = this->m_leafs[leafIndex];
                node->getPlayingNodes(_inputs,_playingNodes);
                break;
            }
        }
    }

    Super::getPlayingNodes(_inputs,_playingNodes);
}

template <class RESULTTYPE>
void BlendTreeNodeChooseBranch<RESULTTYPE>::updateResult( f32 _deltaTime, const InputContainer& _inputs, BlendTreeResultBase* _result )
{
    if ( !this->getTemplate()->getIgnoreRuleChange() )
    {
        updateActiveChild(_inputs,bfalse);
    }

    processBlend(_deltaTime,_inputs,_result);

    Super::updateResult(_deltaTime,_inputs,_result);
}

template <class RESULTTYPE>
void BlendTreeNodeChooseBranch<RESULTTYPE>::processBlend( f32 _deltaTime, const InputContainer & _inputs, BlendTreeResultBase* _result )
{
    if ( m_timeCounter )
    {
        updateBlend(_deltaTime);
    }
    else
    {
        m_mostSignificantLeaf = m_activeLeaf;
    }

    RESULTTYPE* result = static_cast<RESULTTYPE*>(_result);

    if ( !m_timeCounter && m_activeLeaf != U32_INVALID )
    {
        processLeaf(m_activeLeaf,_deltaTime,_inputs,result);
    }
    else
    {
        u32 numLeafs = this->m_leafs.size();

        for ( u32 leafIndex = 0; leafIndex < numLeafs; leafIndex++ )
        {
            processLeaf(leafIndex,_deltaTime,_inputs,result);
        }
    }
}

template <class RESULTTYPE>
void BlendTreeNodeChooseBranch<RESULTTYPE>::processLeaf( u32 _leafIndex, f32 _deltaTime, const InputContainer& _inputs, RESULTTYPE* _result )
{
    BlendLeaf& blendLeaf = m_childData[_leafIndex];

    if ( blendLeaf.m_currentWeight == 0.f )
    {
        return;
    }

    bbool active = blendLeaf.m_currentWeight && ( _leafIndex == m_activeLeaf || blendLeaf.m_currentWeight == 1.f );
    bbool mostSignificant = _leafIndex == m_mostSignificantLeaf;
    BlendTreeNode<RESULTTYPE>* childNode = this->m_leafs[_leafIndex];
    
    f32 dt;

    if ( !getTemplate()->getNoUpdateInactive() || m_activeLeaf == _leafIndex )
    {
        dt = _deltaTime;
    }
    else
    {
        dt = 0.f;
    }

    RESULTTYPE localResult;

    childNode->updateResult(dt,_inputs,&localResult);

    _result->updateBlend(&localResult,active,mostSignificant,blendLeaf.m_currentWeight);
}

template <class RESULTTYPE>
void BlendTreeNodeChooseBranch<RESULTTYPE>::updateActiveChild( const InputContainer& _inputs, bbool _force )
{
    u32 leafIndex = 0;

    for ( typename ChildDataContainer::iterator it = m_childData.begin(); it != m_childData.end(); ++it, leafIndex++ )
    {
        BlendLeaf& childData = *it;

        if ( Criteria::matchCriteria(childData.m_criteria) )
        {
            setActiveLeaf(_inputs,leafIndex,_force);
            break;
        }
    }
}

template <class RESULTTYPE>
void BlendTreeNodeChooseBranch<RESULTTYPE>::updateBlend( f32 _deltaTime )
{
    f32 highestWeight = 0.f;
    u32 childIndex = 0;

    m_timeCounter = Max(m_timeCounter-_deltaTime,0.f);

    f32 t = 1.f - (m_timeCounter/getTemplate()->getBlendTime());

    for ( typename ChildDataContainer::iterator it = m_childData.begin(); it != m_childData.end(); ++it, childIndex++ )
    {
        BlendLeaf& leaf = *it;

        leaf.m_currentWeight = Interpolate(leaf.m_startWeight,leaf.m_targetWeight,t);

        if ( leaf.m_currentWeight > highestWeight )
        {
            m_mostSignificantLeaf = childIndex;
            highestWeight = leaf.m_currentWeight;
        }
    }
}

template <class RESULTTYPE>
void BlendTreeNodeChooseBranch<RESULTTYPE>::setActiveLeaf( const InputContainer& _inputs, u32 leafIndex, bbool _force )
{
    if ( m_activeLeaf != leafIndex || _force )
    {
        u32 numLeafs = this->m_leafs.size();

        for ( u32 i = 0; i < numLeafs; i++ )
        {
            BlendLeaf& leaf = m_childData[i];

            if ( i == leafIndex )
            {
                leaf.m_targetWeight = 1.f;

                if ( !getTemplate()->getBlendTime() || _force )
                {
                    leaf.m_currentWeight = 1.f;
                }
                else
                {
                    leaf.m_startWeight = leaf.m_currentWeight;
                }
            }
            else
            {
                leaf.m_targetWeight = 0.f;

                if ( !getTemplate()->getBlendTime() || _force )
                {
                    leaf.m_currentWeight = 0.f;
                }
                else
                {
                    leaf.m_startWeight = leaf.m_currentWeight;
                }
            }
        }

        m_activeLeaf = leafIndex;
        m_timeCounter = _force ? 0.f : getTemplate()->getBlendTime();

        BlendTreeNode <RESULTTYPE>* node = this->m_leafs[leafIndex];

        if ( node )
        {
            node->onBecomeActive(_inputs);
        }
    }
}

template <class RESULTTYPE>
void BlendTreeNodeChooseBranch<RESULTTYPE>::onBecomeActive( const InputContainer& _inputs )
{
    u32 startingLeaf = getTemplate()->getStartingLeaf();

    if ( startingLeaf == U32_INVALID )
    {
        updateActiveChild(_inputs,btrue);
    }
    else
    {
        setActiveLeaf(_inputs,startingLeaf,btrue);
    }
}

template <class RESULTTYPE>
void BlendTreeNodeChooseBranch<RESULTTYPE>::onTransitionPlay()
{
    if ( m_activeLeaf != U32_INVALID && m_activeLeaf < this->m_leafs.size() )
    {
        BlendTreeNode <RESULTTYPE>* childNode = this->m_leafs[m_activeLeaf];

        if ( childNode )
        {
            childNode->onTransitionPlay();
        }
    }

    Super::onTransitionPlay();
}

}

#endif // _ITF_BLENDTREENODECHOOSEBRANCH_H_

