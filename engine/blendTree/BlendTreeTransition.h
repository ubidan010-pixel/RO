
#ifndef _ITF_BLENDTREETRANSITION_H_
#define _ITF_BLENDTREETRANSITION_H_

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_BLENDTREERESULT_H_
#include "engine/blendTree/BlendTreeResult.h"
#endif //_ITF_BLENDTREERESULT_H_


namespace ITF
{

template <class RESULTTYPE>
class BlendTreeTransition_Template;

template <class RESULTTYPE>
class BlendTreeTransition
{

public:

    BlendTreeTransition() : m_template(NULL), m_blendNode(NULL) {}
    ~BlendTreeTransition() 
    {
        clear();
    }

    void                                            init( struct BlendTreeInitDataBase* _initData, const BlendTreeTransition_Template <RESULTTYPE>* _template );
    void                                            onLoadResource( struct BlendTreeLoadResourceBase* _resData );
    bbool                                           validate() const;

    BlendTreeNode <RESULTTYPE>*                     getNode() const { return m_blendNode; }
    u32                                             getBlend() const;
    u32                                             getBlendFromTransition() const;
    bbool                                           hasFrom() const;

private:

    void                                            clear();

    const BlendTreeTransition_Template <RESULTTYPE>*    m_template;
    BlendTreeNode <RESULTTYPE>*                         m_blendNode;
};

////////////////////////////////////////////////////////////////////////////////////

template <class RESULTTYPE>
class BlendTreeNodeTemplate;

extern const char* BlendTreeTransition_Template_GetClassNameStatic();

template <class RESULTTYPE>
class BlendTreeTransition_Template : public TemplateObj
{
    DECLARE_OBJECT_CHILD_TEMPLATE_RTTI(BlendTreeTransition_Template,TemplateObj,1598545264);
    DECLARE_SERIALIZE();

public:

    BlendTreeTransition_Template() : m_blendNode(NULL), m_numBlendFrames(0), m_numBlendFramesFromTransition(4) {}
    ~BlendTreeTransition_Template();

    const SafeArray<StringID>&                      getFromList() const { return m_fromList; }
    const SafeArray<StringID>&                      getToList() const { return m_toList; }
    const BlendTreeNodeTemplate <RESULTTYPE>*       getNode() const { return m_blendNode; }
    u32                                             getNumBlendFrames() const { return m_numBlendFrames; }
    u32                                             getNumBlendFramesFromTransition() const { return m_numBlendFramesFromTransition; }
    
    void                                            setNode(BlendTreeNodeTemplate <RESULTTYPE>* _blendNode) { m_blendNode = _blendNode; }

private:

    SafeArray<StringID>                             m_fromList;
    SafeArray<StringID>                             m_toList;
    BlendTreeNodeTemplate <RESULTTYPE>*             m_blendNode;
    u32                                             m_numBlendFrames;
    u32                                             m_numBlendFramesFromTransition;
};

BEGIN_SERIALIZATION_TEMPLATE(BlendTreeTransition_Template)

    SERIALIZE_MEMBER("blend",m_numBlendFrames);
    SERIALIZE_MEMBER("blendFromTransition",m_numBlendFramesFromTransition);
    SERIALIZE_CONTAINER("from",m_fromList);
    SERIALIZE_CONTAINER("to",m_toList);

    BEGIN_CONDITION_BLOCK(ESerializeGroup_Data)
        SERIALIZE_OBJECT_WITH_FACTORY("node",m_blendNode, NULL);
    END_CONDITION_BLOCK()    

END_SERIALIZATION()

template <class RESULTTYPE>
BlendTreeTransition_Template<RESULTTYPE>::~BlendTreeTransition_Template()
{
    SF_DEL(m_blendNode);
}

////////////////////////////////////////////////////////////////////////////////////

template <class RESULTTYPE>
void BlendTreeTransition<RESULTTYPE>::init( BlendTreeInitDataBase* _initData, const BlendTreeTransition_Template <RESULTTYPE>* _template )
{
    m_template = _template;
    
    BlendTreeInitData <RESULTTYPE>* initData = static_cast< BlendTreeInitData <RESULTTYPE>* >(_initData);

    for (u32 i = 0; i < m_template->getFromList().size(); ++i)
    {
        typename ITF_MAP<StringID,BlendTreeNode <RESULTTYPE>* >::iterator it = initData->m_namedNodes->find(m_template->getFromList()[i]);

        if ( it == initData->m_namedNodes->end())
        {
#if !defined(ITF_FINAL) && defined(ITF_DEBUG_STRINGID)
            ITF_WARNING_CATEGORY(GPP, NULL, 0, "Transition error rule FROM with name %s not defined",m_template->getFromList()[i].getDebugString());
#else
            ITF_WARNING_CATEGORY(GPP, NULL, 0, "Transition error rule FROM with name not defined");
#endif
        }
        else
        {
            BlendTreeNode <RESULTTYPE>* node = it->second;

            node->addTransitionFrom(this);
        }
    }

    for (u32 i = 0; i < m_template->getToList().size(); ++i)
    {
        typename ITF_MAP<StringID,BlendTreeNode <RESULTTYPE>* >::iterator it = initData->m_namedNodes->find(m_template->getToList()[i]);

        if ( it == initData->m_namedNodes->end())
        {
#if !defined(ITF_FINAL) && defined(ITF_DEBUG_STRINGID)
            ITF_WARNING_CATEGORY(GPP, NULL, 0, "Transition error rule TO with name %s not defined",m_template->getToList()[i].getDebugString());
#else
            ITF_WARNING_CATEGORY(GPP, NULL, 0, "Transition error rule TO with name not defined");
#endif
        }
        else
        {
            BlendTreeNode <RESULTTYPE>* node = it->second;

            node->addTransitionTo(this);
        }
    }

    if ( m_template && m_template->getNode() )
    {
        m_blendNode = m_template->getNode()->createNode();
        m_blendNode->init(_initData,_template->getNode());
    }
}

template <class RESULTTYPE>
void BlendTreeTransition<RESULTTYPE>::onLoadResource( BlendTreeLoadResourceBase* _resData )
{
    if ( m_blendNode )
    {
        m_blendNode->onLoadResource(_resData);
    }
}

template <class RESULTTYPE>
void BlendTreeTransition<RESULTTYPE>::clear()
{
    SF_DEL(m_blendNode);
}

template <class RESULTTYPE>
bbool BlendTreeTransition<RESULTTYPE>::validate() const
{
    if ( m_blendNode && !m_blendNode->validate() )
    {
        return bfalse;
    }

    return btrue;
}

template <class RESULTTYPE>
u32 BlendTreeTransition<RESULTTYPE>::getBlend() const { return m_template->getNumBlendFrames(); }
template <class RESULTTYPE>
u32 BlendTreeTransition<RESULTTYPE>::getBlendFromTransition() const { return m_template->getNumBlendFramesFromTransition(); }
template <class RESULTTYPE>
bbool BlendTreeTransition<RESULTTYPE>::hasFrom() const { return m_template->getFromList().size() > 0;}

}

#endif // _ITF_BLENDTREETRANSITION_H_

