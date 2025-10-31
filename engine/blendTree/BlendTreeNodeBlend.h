
#ifndef _ITF_BLENDTREENODEBLEND_H_
#define _ITF_BLENDTREENODEBLEND_H_

#ifndef _ITF_BLENDTREENODE_H_
#include "engine/blendTree/BlendTreeNode.h"
#endif //_ITF_BLENDTREENODE_H_

namespace ITF
{

extern const char* BlendTreeNodeBlend_GetClassNameStatic();
extern const char* BlendTreeNodeBlend_Template_GetClassNameStatic();

template <class RESULTTYPE>
class BlendTreeNodeBlend_Template;

template <class RESULTTYPE>
class BlendTreeNodeBlend : public BlendTreeNode <RESULTTYPE>
{
    DECLARE_OBJECT_CHILD_TEMPLATE_RTTI_ABSTRACT(BlendTreeNodeBlend,BlendTreeNode<RESULTTYPE>,628986251);

public:

    BlendTreeNodeBlend();
    virtual ~BlendTreeNodeBlend() { clear(); }

    virtual void                                                    init( BlendTreeInitDataBase* _initData, const BlendTreeNodeTemplate <RESULTTYPE>* _template );
    virtual void                                                    onLoadResource( struct BlendTreeLoadResourceBase* _resData );
    virtual bbool                                                   validate() const;

    virtual void                                                    addTransitionFrom( BlendTreeTransition <RESULTTYPE>* _transition );
    virtual void                                                    addTransitionTo( BlendTreeTransition <RESULTTYPE>* _transition );
    
protected:

    const BlendTreeNodeBlend_Template <RESULTTYPE>* getTemplate() const;

    void                                                            clear();

    SafeArray < BlendTreeNode <RESULTTYPE>* >                       m_leafs;
};

////////////////////////////////////////////////////////////////////////////////////

template <class RESULTTYPE>
class BlendTreeNodeBlend_Template : public BlendTreeNodeTemplate <RESULTTYPE>
{
    DECLARE_OBJECT_CHILD_TEMPLATE_RTTI_ABSTRACT(BlendTreeNodeBlend_Template,BlendTreeNodeTemplate<RESULTTYPE>,1110016475);
    DECLARE_SERIALIZE();

public:
    const u32 NO_LIMIT_CAPACITY() const { return 1000; } 

    BlendTreeNodeBlend_Template();
    ~BlendTreeNodeBlend_Template();

    const SafeArray < BlendTreeNodeTemplate <RESULTTYPE>* >&        getLeafs() const { return m_leafs; }
    bbool                                                           getIgnoreRuleChange() const { return m_ignoreRuleChange; }
    const virtual u32                                               getCapacity() const { return NO_LIMIT_CAPACITY(); }

private:

    SafeArray < BlendTreeNodeTemplate <RESULTTYPE>* >               m_leafs;
    bbool                                                           m_ignoreRuleChange;
};

BEGIN_SERIALIZATION_ABSTRACT_CHILD_TEMPLATE(BlendTreeNodeBlend_Template)
    
    BEGIN_CONDITION_BLOCK(ESerializeGroup_Data)
        SERIALIZE_CONTAINER_WITH_FACTORY("leafs", m_leafs, NULL);
    END_CONDITION_BLOCK()

    SERIALIZE_MEMBER("ignoreRuleChanges",m_ignoreRuleChange);

END_SERIALIZATION()

template <class RESULTTYPE>
BlendTreeNodeBlend_Template<RESULTTYPE>::BlendTreeNodeBlend_Template()
: m_ignoreRuleChange(bfalse)
{
}

template <class RESULTTYPE>
BlendTreeNodeBlend_Template<RESULTTYPE>::~BlendTreeNodeBlend_Template()
{
    u32 numLeafs = m_leafs.size();

    for ( u32 i = 0; i < numLeafs; i++ )
    {
        SF_DEL(m_leafs[i]);
    }
}

////////////////////////////////////////////////////////////////////////////////////

template <class RESULTTYPE>
BlendTreeNodeBlend<RESULTTYPE>::BlendTreeNodeBlend()
{
}

template <class RESULTTYPE>
void BlendTreeNodeBlend<RESULTTYPE>::clear()
{
    u32 numLeafs = m_leafs.size();

    for ( u32 i = 0; i < numLeafs; i++ )
    {
        SF_DEL(m_leafs[i]);
    }

    m_leafs.clear();
}

template <class RESULTTYPE>
void BlendTreeNodeBlend<RESULTTYPE>::init( BlendTreeInitDataBase* _initData, const BlendTreeNodeTemplate <RESULTTYPE>* _template )
{
    Super::init(_initData,_template);

    const BlendTreeNodeBlend_Template <RESULTTYPE>* node_template = getTemplate();
    const SafeArray < BlendTreeNodeTemplate <RESULTTYPE>* >& leafs = node_template->getLeafs();

    u32 numLeafs = leafs.size();

    for ( u32 i = 0; i < numLeafs; i++ )
    {
        const BlendTreeNodeTemplate <RESULTTYPE>* leaf_template = leafs[i];
        BlendTreeNode <RESULTTYPE>* node = leaf_template->createNode();

        node->init(_initData,leaf_template);
        m_leafs.push_back(node);
    }
}
template <class RESULTTYPE>
void BlendTreeNodeBlend<RESULTTYPE>::onLoadResource( BlendTreeLoadResourceBase* _resData )
{
    Super::onLoadResource(_resData);

    u32 numLeafs = m_leafs.size();

    for ( u32 i = 0; i < numLeafs; i++ )
    {
        BlendTreeNode <RESULTTYPE>* node = m_leafs[i];

        if ( node )
        {
            node->onLoadResource(_resData);
        }
    }
}

template <class RESULTTYPE>
bbool BlendTreeNodeBlend<RESULTTYPE>::validate() const
{
    u32 numLeafs = m_leafs.size();

    for ( u32 i = 0; i < numLeafs; i++ )
    {
        BlendTreeNode <RESULTTYPE>* node = m_leafs[i];

        if ( !node || !node->validate() )
        {
            return bfalse;
        }
    }

    return btrue;
}

template <class RESULTTYPE>
void BlendTreeNodeBlend<RESULTTYPE>::addTransitionFrom( BlendTreeTransition<RESULTTYPE>* _transition )
{
    Super::addTransitionFrom(_transition);

    u32 numLeafs = m_leafs.size();

    for ( u32 i = 0; i < numLeafs; i++ )
    {
        BlendTreeNode <RESULTTYPE>* node = m_leafs[i];

        if ( node )
        {
            node->addTransitionFrom(_transition);
        }
    }
}

template <class RESULTTYPE>
void BlendTreeNodeBlend<RESULTTYPE>::addTransitionTo( BlendTreeTransition<RESULTTYPE>* _transition )
{
    Super::addTransitionTo(_transition);

    u32 numLeafs = m_leafs.size();

    for ( u32 i = 0; i < numLeafs; i++ )
    {
        BlendTreeNode <RESULTTYPE>* node = m_leafs[i];

        if ( node )
        {
            node->addTransitionTo(_transition);
        }
    }
}

template <class RESULTTYPE>
const BlendTreeNodeBlend_Template <RESULTTYPE>* BlendTreeNodeBlend<RESULTTYPE>::getTemplate() const
{
    return static_cast<const BlendTreeNodeBlend_Template <RESULTTYPE>*>(this->m_template);
}

}

#endif // _ITF_BLENDTREENODEBLEND_H_

