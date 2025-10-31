
#ifndef _ITF_OBJBINDING_H_
#define _ITF_OBJBINDING_H_

namespace ITF
{
    class ObjectGroup;

    struct BindData
    {
        ObjectRef   m_child;
    };

    class ObjBinding
    {
    public:

        ObjBinding();
        ~ObjBinding();

        typedef ITF_LIST <BindData>    ChildrenList;
        typedef SafeArray <ObjectRef> RemovedArray;

        void                setOwner( class BaseObject* _owner ) { m_owner = _owner; }
        class BaseObject*   getOwner() const { return m_owner; }

        ObjectRef           getParent() const { return m_parent; }
        bbool               isAncestor( ObjectRef _actor );
        bbool               isDescendant( ObjectRef _actor );

        void                bindChild( const ObjectRef& _ref );
        void                unbindChild( const ObjectRef& _ref ) { m_removed.push_back(_ref); }
        void                unbindAllChildren();
        void                unbindFromParent();

        bbool               isBound() const { return m_parent.isValid(); }

        void                updateChildren( f32 _deltaTime );
#ifdef ITF_SUPPORT_EDITOR
        void                onEditorMove(bbool _modifyInitialPos = btrue);
#endif // ITF_SUPPORT_EDITOR

        void                clear();
        u32                 addChildrenInGroupForUpdate(ObjectGroup* _listToFill  );

        ITF_INLINE bbool    hasChildren() const { return m_children.size() > 0; }
        ITF_INLINE const ChildrenList& getChildren() const { return m_children; }

        void                forceRemovePending() ;
        static ObjBinding*  getBindingFromObj( const ObjectRef& _ref );

    protected:

        void                removePending();

        ObjectRef           m_parent;
        ChildrenList        m_children;
        RemovedArray        m_removed;
        class BaseObject*   m_owner;
#ifndef ITF_FINAL
        bbool               m_isUpdating;
#endif
    };

}

#endif // _ITF_OBJBINDING_H_
