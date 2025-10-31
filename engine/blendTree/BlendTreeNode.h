
#ifndef _ITF_BLENDTREENODE_H_
#define _ITF_BLENDTREENODE_H_

#ifndef _ITF_BLENDTREERESULT_H_
#include "engine/blendTree/BlendTreeResult.h"
#endif //_ITF_BLENDTREERESULT_H_

#ifndef ITF_TEMPLATEDATABASE_H
#include "engine/TemplateManager/TemplateDatabase.h"
#endif // ITF_TEMPLATEDATABASE_H

#ifndef _ITF_INPUTCRITERIAS_H_
#include "engine/blendTree/InputCriterias.h"
#endif //_ITF_INPUTCRITERIAS_H_

#define DECLARE_NODE_LIST typedef FixedArray < BlendTreeNode <RESULTTYPE>*, 24 > NodeList;

namespace ITF
{

extern const char* BlendTreeNode_GetClassNameStatic();
extern const char* BlendTreeNodeTemplate_GetClassNameStatic();

template <class RESULTTYPE>
class BlendTreeTransition;

template <class RESULTTYPE>
class BlendTreeNodeTemplate;

template <class RESULTTYPE>
class BlendTreeNode : public IRTTIObject
{
    DECLARE_OBJECT_CHILD_TEMPLATE_RTTI_ABSTRACT(BlendTreeNode,IRTTIObject,4158429769);

public:

    DECLARE_NODE_LIST

    BlendTreeNode() : m_template(NULL) {}
    virtual ~BlendTreeNode() {}

    virtual void                                            init( struct BlendTreeInitDataBase* _initData, const BlendTreeNodeTemplate <RESULTTYPE>* _template );
    virtual void                                            onLoadResource( struct BlendTreeLoadResourceBase* _resData ) {}
    virtual void                                            getPlayingNodes( const InputContainer & _inputs, NodeList& _playingNodes ) { _playingNodes.push_back(this); }
    virtual void                                            updateResult( f32 _deltaTime, const InputContainer & _inputs, BlendTreeResultBase* _result );
    virtual void                                            onBecomeActive( const InputContainer& _inputs ) {}
    virtual void                                            onTransitionPlay() {}
    virtual bbool                                           validate() const { return btrue; }

    const StringID&                                         getId() const;
    virtual void                                            addTransitionFrom( BlendTreeTransition <RESULTTYPE>* _transition ) { m_transitionsFrom.push_back(_transition); }
    virtual void                                            addTransitionTo( BlendTreeTransition <RESULTTYPE>* _transition ) { m_transitionsTo.push_back(_transition); }
    const SafeArray<BlendTreeTransition <RESULTTYPE>*>&     getTransitionFrom() const { return m_transitionsFrom; }
    const SafeArray<BlendTreeTransition <RESULTTYPE>*>&     getTransitionTo() const { return m_transitionsTo; }

protected:

    void                                                    clear();

    const BlendTreeNodeTemplate <RESULTTYPE>*               m_template;
    SafeArray<BlendTreeTransition <RESULTTYPE>*>            m_transitionsFrom;
    SafeArray<BlendTreeTransition <RESULTTYPE>*>            m_transitionsTo;
};

//======================================================================================================
//======================================================================================================

template <class RESULTTYPE>
void BlendTreeNode<RESULTTYPE>::clear()
{
    m_transitionsFrom.clear();
    m_transitionsTo.clear();
}

template <class RESULTTYPE>
void BlendTreeNode<RESULTTYPE>::init( BlendTreeInitDataBase* _initData, const BlendTreeNodeTemplate <RESULTTYPE>* _template )
{
    m_template = _template;

    if (m_template->getId().isValid())
    {
        BlendTreeInitData <RESULTTYPE>* initData = static_cast< BlendTreeInitData <RESULTTYPE>* >(_initData);
#if !defined(ITF_FINAL) && defined(ITF_DEBUG_STRINGID)
        ITF_ASSERT_MSG(initData->m_namedNodes->find(m_template->getId()) == initData->m_namedNodes->end(),"Node %s is already defined",m_template->getId().getDebugString());
#else
       // ITF_ASSERT(initData->m_namedNodes->find(m_template->getNameID()) == initData->m_namedNodes->end()); //CASA::TODO::
#endif
        (*initData->m_namedNodes)[m_template->getId()] = this;
    }
}

template <class RESULTTYPE>
void BlendTreeNode<RESULTTYPE>::updateResult( f32 _deltaTime, const InputContainer & _inputs, BlendTreeResultBase* _result )
{
    BlendTreeResult <RESULTTYPE>* res = static_cast< BlendTreeResult <RESULTTYPE>* >(_result);
    res->m_playingNodes.push_back(this);
}

////////////////////////////////////////////////////////////////////////////////////

#define DECLARE_BLENDTREENODE_TEMPLATE(blendTreeNodeClass) \
public: \
    virtual class BlendTreeNode<RESULTTYPE>* createNode() const { return newAlloc(mId_System, blendTreeNodeClass<RESULTTYPE>()); } \

template <class RESULTTYPE>
class BlendTreeNodeTemplate : public TemplateObjBase
{
    DECLARE_OBJECT_CHILD_TEMPLATE_RTTI_ABSTRACT(BlendTreeNodeTemplate,TemplateObjBase,1641433799);
    DECLARE_SERIALIZE();

public:

    BlendTreeNodeTemplate() : m_numId(-1) {}
    ~BlendTreeNodeTemplate() {}

    virtual BlendTreeNode<RESULTTYPE>* createNode() const = 0;

    const StringID&                 getId() const { return m_nameId; }
    void                            setId(StringID _nameId) { m_nameId = _nameId; }
    i32                             getNumId() const { return m_numId; }
    void                            setNumId( i32 _numId) { m_numId = _numId; }
    const virtual u32               getCapacity() const { return 0; }

private:

    StringID                        m_nameId;
    i32                             m_numId;
};

BEGIN_SERIALIZATION_TEMPLATE_ABSTRACT(BlendTreeNodeTemplate)

    SERIALIZE_MEMBER("nodeName",m_nameId);

END_SERIALIZATION()

template <class RESULTTYPE>
const StringID& BlendTreeNode<RESULTTYPE>::getId() const { return m_template->getId(); }

}

#endif // _ITF_BLENDTREENODE_H_

