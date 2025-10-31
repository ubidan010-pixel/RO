#ifndef _ITF_LINKCOMPONENT_H_
#define _ITF_LINKCOMPONENT_H_

#ifndef _ITF_OBJECTPATH_H_
#include "core/ObjectPath.h"
#endif //_ITF_OBJECTPATH_H_

#ifndef _ITF_COLOR_H_
#include "core/Color.h"
#endif //_ITF_COLOR_H_

namespace ITF
{

    struct TagValue
    {
        DECLARE_SERIALIZE()

        StringID    m_tag;
        String8     m_value;

        bbool operator ==(const StringID& _tag) const
        {
            return m_tag == _tag;
        };
    };

    //---------------------------------------------------------------------------------------------------

    class ChildEntry
    {
        DECLARE_SERIALIZE()

        typedef ITF_VECTOR<TagValue> TagValueList;

        void                setPath(const ObjectPath& _path)                        { m_childPath = _path; }
        const ObjectPath&   getPath()const                                          { return m_childPath; }
        template < typename T >
        bbool               getTagValue( const StringID& _tag, T& _value) const;
        bbool               hasTag( const StringID& _tag) const;
        template < typename T >
        void                setTagValue( const StringID& _tag, const T& _value);
        bbool               removeTag(const StringID& _tag);
        void                removeTags();
        const TagValueList& getTagValueList()const                                  { return m_tagValueList; }

        bbool               operator==(const ObjectPath& _path) const               { return m_childPath == _path; }

    private:
        ObjectPath          m_childPath;
        TagValueList        m_tagValueList;
    };

    // f32 specialization
    template<>
    bbool ChildEntry::getTagValue<f32>( const StringID& _tag, f32& _value ) const;
    template<>
    void ChildEntry::setTagValue<f32>( const StringID& _tag, const f32& _value );

    // i32 specialization
    template<>
    bbool ChildEntry::getTagValue<i32>( const StringID& _tag, i32& _value ) const;
    template<>
    void ChildEntry::setTagValue<i32>( const StringID& _tag, const i32& _value );
    
    // bool specialization
    template<>
    bbool ChildEntry::getTagValue<bool>( const StringID& _tag, bool& _value ) const;
    template<>
    void ChildEntry::setTagValue<bool>( const StringID& _tag, const bool& _value );

    // StringID specialization
    template<>
    bbool ChildEntry::getTagValue<StringID>( const StringID& _tag, StringID& _value ) const;
    template<>
    void ChildEntry::setTagValue<StringID>( const StringID& _tag, const StringID& _value );

    // String8 specialization
    template<>
    bbool ChildEntry::getTagValue<String8>( const StringID& _tag, String8& _value ) const;
    template<>
    void ChildEntry::setTagValue<String8>( const StringID& _tag, const String8& _value );

    //---------------------------------------------------------------------------------------------------

    class LinkComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(LinkComponent, ActorComponent,1144483611);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        typedef ITF_VECTOR<ObjectPath> ChildrenPathList;
        typedef ITF_VECTOR<ChildEntry> ChildrenList;

        LinkComponent();
        ~LinkComponent();

        virtual bbool       needsUpdate() const { return bfalse; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);

        void                addChild( const ObjectPath& _child, bbool _addToLinkManager = btrue );
        void                removeChild( const ObjectPath& _child, bbool _removeFromLinkManager = btrue );
        bbool               hasChild( const ObjectPath& _child );
        BaseObject *        getChildObject( const ObjectPath& _child ) const;

        ITF_INLINE const ChildrenList& getChildren() const          { return m_children; }
        ITF_INLINE ChildrenList& getMutableChildren()           { return m_children; }


        bbool               removeTag( const ObjectPath& _child, const StringID& _tag );
        bbool               removeTags( const ObjectPath& _child );

        void                sendEventToChildren( Event* _event, bbool _discardChildWithTags = bfalse );
        void                sendEventToChildren( Event* _event, StringID _filterTag );

        void                clearLinks();

        template < typename T >
        bbool               getTagValue( const ObjectPath& _child, const StringID& _tag, T& _value) const
        {
            bbool ret = bfalse;
            ChildrenList::const_iterator it = std::find(m_children.begin(), m_children.end(), _child);
            if(it != m_children.end())
                ret = it->getTagValue(_tag, _value);
            return ret;
        }

        bbool               hasTag(const ObjectPath& _child, const StringID& _tag)
        {
            bbool ret = bfalse;
            ChildrenList::const_iterator it = std::find(m_children.begin(), m_children.end(), _child);
            if(it != m_children.end())
                ret = it->hasTag(_tag);
            return ret;
        }

        template < typename T >
        void                setTagValue( const ObjectPath& _child, const StringID& _tag, const T& _value)
        {
            ChildrenList::iterator it = std::find(m_children.begin(), m_children.end(), _child);
            if(it != m_children.end())
                it->setTagValue(_tag, _value);
        }

        virtual void        onActorRemovedFromScene(Scene* /*_pScene*/, bbool _forDeletion);

    protected:



        ITF_INLINE const class LinkComponent_Template*  getTemplate() const;



        void                registerLinks();
        void                convertFromOldFormat();

        ChildrenList        m_children;
        

    private:
        ChildrenPathList    m_childrenPath;
        void                convertFromPathList();

        // children are saved in scene description (instance data)
        // deprecated, this won't be used anymore
        ObjectIdList        m_linkedChildrenIds;

        friend class Link_Plugin;
    };

    //---------------------------------------------------------------------------------------------------

    class LinkComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(LinkComponent_Template,TemplateActorComponent,1680185781);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(LinkComponent);

    public:

        LinkComponent_Template();
        ~LinkComponent_Template() {}

#ifdef ITF_SUPPORT_DEBUGFEATURE
        ITF_INLINE const Color& getDebugColor() const { return m_debugColor; }
        ITF_INLINE const Color& getDebugColorSelected() const { return m_debugColorSelected; }
        ITF_INLINE bbool        getDebugChildIndex() const { return m_debugChildIndex; }
#endif // ITF_SUPPORT_DEBUGFEATURE

    private:

#ifdef ITF_SUPPORT_DEBUGFEATURE
        Color                   m_debugColor;
        Color                   m_debugColorSelected;
        bbool                   m_debugChildIndex;
#endif // ITF_SUPPORT_DEBUGFEATURE
    };


    const LinkComponent_Template*  LinkComponent::getTemplate() const {return static_cast<const LinkComponent_Template*>(m_template);}

    //---------------------------------------------------------------------------------------------------
}

#endif // _ITF_LINKCOMPONENT_H_
