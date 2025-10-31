#ifndef _ITF_BTNODE_H_
#define _ITF_BTNODE_H_

namespace ITF
{

struct BTNodeTemplate_Ref
{
    DECLARE_SERIALIZE();

    BTNodeTemplate_Ref();

    StringID                    m_refId;
    class BTNode_Template*      m_refPtr;
};

class BTNode : public IRTTIObject
{
    DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(BTNode,IRTTIObject,1676915347);

public:

    BTNode();
    virtual ~BTNode() {}

    virtual void                        init( class BehaviorTree* _tree, const class BTNode_Template* _template );
    virtual bbool                       validate( Actor* _actor ) const { return btrue; }
    virtual bbool                       update( f32 _dt ) { return btrue; }
    virtual bbool                       decide( f32 _dt ) { return btrue; }
    virtual void                        onActivate() {}
    virtual void                        onDeactivate() {}
    virtual void                        updateAnimInput() {}
    virtual void                        onEvent( Event* _event ) {}
#ifdef ITF_SUPPORT_EDITOR
    virtual void                        drawEdit( class ActorDrawEditInterface* drawInterface, u32 _flags  ) const {}
#endif // ITF_SUPPORT_EDITOR

protected:

    const class BTNode_Template*        getTemplate() const;

    const class BTNode_Template*        m_template;
    BehaviorTree*                       m_tree;
};

#define DECLARE_BTNODE_TEMPLATE(btNodeClass) \
public: \
    virtual class BTNode* createBTNode() const { return newAlloc(mId_System, btNodeClass()); } \
    virtual StringID::StringIdValueType getBTNodeClassName() const { return getBTNodeClassCRCStatic(); } \
    static StringID::StringIdValueType getBTNodeClassCRCStatic() { return btNodeClass::GetClassCRCStatic(); }

class BTNode_Template : public IRTTIObject
{
    DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(BTNode_Template,IRTTIObject,2115900170);
    DECLARE_SERIALIZE_VIRTUAL();

public:

    BTNode_Template() {}
    virtual ~BTNode_Template() {}

    virtual void                onTemplateLoaded() {}
    virtual class BTNode*       createBTNode() const { ITF_ASSERT(0); return NULL; }

    const StringID&             getNameId() const { return m_nameId; }

private:

    StringID                    m_nameId;
};

ITF_INLINE const BTNode_Template*  BTNode::getTemplate() const {return static_cast<const BTNode_Template*>(m_template);}

} // namespace ITF

#endif // _ITF_BTNODE_H_