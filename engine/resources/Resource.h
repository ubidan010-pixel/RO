#ifndef _ITF_RESOURCE_H_
#define _ITF_RESOURCE_H_

#ifndef _ITF_BASEOBJECT_H_
#include "core/baseObject.h"
#endif //_ITF_BASEOBJECT_H_

#ifndef _ITF_BLOB_H_
#include "core/Blob.h"
#endif //_ITF_BLOB_H_

#ifndef _ITF_TEMPLATESINGLETON_H_
#include "core/templateSingleton.h"
#endif //_ITF_TEMPLATESINGLETON_H_
/*
#ifndef ITF_FINAL
#ifdef ITF_WINDOWS
    #define ENABLE_RESOURCE_HISTORY
#endif // ITF_WINDOWS
#endif // ITF_FINAL
*/
namespace ITF
{
    class Resource;
    class ResourceGroup;

//    #define RESOURCE_IS_BASEOBJECT

    #ifdef RESOURCE_IS_BASEOBJECT
        #define ResourceIdentifierType  ObjectRef
        #define InvalidateResourceIdentifier(_id_) _id_.invalidate()
        #define getResourceFromIdentifier(_ident_) (Resource*)_ident_.getObject()
        #define getIdentifierFromResourcePtr(_resource_) _resource_->getRef()
        #define constructResourceParent() BaseObject(),
    #else
        #define ResourceIdentifierType  Resource*
        #define InvalidateResourceIdentifier(_id_) _id_ = NULL
        #define getResourceFromIdentifier(_ident_) _ident_
        #define getIdentifierFromResourcePtr(_resource_) _resource_
        #define constructResourceParent()
    #endif //RESOURCE_IS_BASEOBJECT

    class ResourceID
    {
    public:
        static ResourceID   Invalid;
        ResourceID() {invalidateResourceId();}

        void serialize( class ArchiveMemory& _archive );

        bool operator == (const ResourceID& _id) const
        {
            return (m_resource == _id.m_resource);
        }

        ITF_INLINE bool operator != (const ResourceID& _id) const
        {
            return !(*this ==_id);
        }

        bool equals (const ResourceID& _id) const
        {
            return (m_resource == _id.m_resource);
        }

        const ResourceID& operator = (const ResourceID& _id)
        {
            m_resource = _id.m_resource; return *this;
        }

        ResourceID( const ResourceID& _id)
        {
            m_resource = _id.m_resource;
        }

        Resource* getResource() const
        {
        #ifdef RESOURCE_IS_BASEOBJECT
            return (Resource*)m_resource.getObject();
        #else
            return m_resource;
        #endif //RESOURCE_IS_BASEOBJECT
        }

        void  invalidateResourceId();
        bbool isValidResourceId()const;

        #ifdef RESOURCE_IS_BASEOBJECT
            const ObjectRef& getResourceRef() const;
        #else
            Resource* getResourceRef() const;
        #endif //RESOURCE_IS_BASEOBJECT

        void setID (Resource* _id)
        {
        #ifdef RESOURCE_IS_BASEOBJECT
            m_resource = _id->getRef();
        #else
            m_resource = _id;
        #endif //RESOURCE_IS_BASEOBJECT
        }

        const Path* getPath() const;
        String8 getDebugString() const;

    private:
        friend class ResourceManager;
        friend class Resource;
        ResourceIdentifierType  m_resource;
    };
    VECTAGSET(ResourceID, ITF::MemoryId::mId_Resource)

class   ResourceListener
{
public:
    virtual  ~ResourceListener() {};
    virtual void onNewResource(Resource* pResource) = 0;

};

ITF_COMPILE_TIME_ASSERT (ResourceID_size_error, sizeof(ResourceID) == sizeof(class Resource*));


#ifdef ENABLE_RESOURCE_HISTORY
    enum ResourceHistoryOp
    {
        ResourceHistoryOp_CreateResource,
        ResourceHistoryOp_DeleteResource
    };

    typedef struct ResourceHistory
    {
        ResourceHistoryOp       m_opType;
        u32                     m_opTime;
        ObjectRef               m_resourceRef;
        String8                 m_path;
        Resource*               m_resourcePointer;
    } ResourceHistory;


class ResourceHistoryManager:public TemplateSingleton<ResourceHistoryManager>
{
public:
    ResourceHistoryManager();
    ~ResourceHistoryManager();

    void addResourceHistory(const ResourceHistory& _resourceHistory);
    void dumpResourceHistory(ObjectRef _or);

    const ITF_VECTOR<ResourceHistory>& getResourcesHistory() {return m_history;}

private:
    ITF_THREAD_CRITICAL_SECTION m_cs;
    ITF_VECTOR<ResourceHistory> m_history;
};
#endif // ENABLE_RESOURCE_HISTORY

//////////////////////////////////////////////////////////////////////////
///
/// Resource base class. All resources must inherit from this class to be
/// handled by the ResourceManager.
#ifdef RESOURCE_IS_BASEOBJECT
class Resource : public BaseObject
#else
class Resource : public IRTTIObject
#endif
{
public:
#ifdef RESOURCE_IS_BASEOBJECT
    DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(Resource,BaseObject,839246104)
#else
    DECLARE_OBJECT_CHILD_RTTI(Resource,IRTTIObject,839246104)
#endif
        enum ResourceType : i32
        {
            ResourceType_Texture = 0,
            ResourceType_Sound,
            ResourceType_Lua,
            ResourceType_FriezeConfig,
            ResourceType_Font,
            ResourceType_XML,
            ResourceType_AnimTrack,
            ResourceType_AnimSkeleton,
            ResourceType_AnimPatchBank,
            ResourceType_ResourceGroup,
            ResourceType_DependenciesMap,
            ResourceType_MusicPart,
            ResourceType_Count,
            ResourceType_All = ResourceType_Count,
            ENUM_FORCE_SIZE_32(0)
        };

        #ifdef ENABLE_RESOURCE_HISTORY
            static void dumpResourceHistory(ObjectRef _or);
        #endif // ENABLE_RESOURCE_HISTORY

        static void    forceDeletion(Resource *_res);

        //////////////////////////////////////////////////////////////////////////
        ///
        /// frees and reloads resource
        ///
        virtual void    reload        (                        );

        //////////////////////////////////////////////////////////////////////////
        ///
        /// @return the type of the resource (texture, sound, etc.)
        ///
        virtual ResourceType getType()const {ITF_ASSERT_MSG(bfalse, "pure virtual: must be implemented in children classes"); return ResourceType_Count;}

        //////////////////////////////////////////////////////////////////////////
        ///
        /// Some resources are groups, some resources have a ResourceGroup as a member, 
        /// and some don't.
        /// @return a pointer to the ResourceGroup contained by the resource, if any
        ///
        virtual ResourceGroup*  getGroup() {return NULL;}


        virtual void    postCreate() {};

        //////////////////////////////////////////////////////////////////////////
        ///
        /// @return the users count
        ///
        ITF_INLINE u32 getPhysicalUsersCount() const {return m_physicalDataRefCount;}

        static void addListener(ResourceListener* _listener);

        virtual bbool   isPhysicalExist();
        virtual bbool   isPhysicallyLoaded(); // not const because some variables may be updated here to avoid systematic re-evaluation
        
        u32   decreaseLifeTime() {return (m_TimeLifeBeforeDestroy > 0) ? --m_TimeLifeBeforeDestroy : 0; }
        void  setTimeLifeDestroy(u32 _TimeLifeBeforeDestroy) {m_TimeLifeBeforeDestroy = _TimeLifeBeforeDestroy;}

        void flushPhysical();
        virtual bbool delayPhysicalFlush() {return btrue;} // tels the resource manager to wait for seome seconds before releasing the resource's physical data to avoid unload/reload ping pong

        const ResourceID&  getID() const {return m_ID;}

        virtual void onResourceProcessingFromMainThread() {}

        //TODO move it to private,there is still issue with lua that doesn't allow this behavior right now
        void    setPhysicalLoaded(bbool _bPhysicalLoaded);
        void    hijackPhysicalLoaded(bbool _bPhysicalLoaded) {m_bPhysicalLoaded = _bPhysicalLoaded;}

        bbool   isCooked()  const       {return m_isCooked;}
        void    setCooked(bbool _Value) {m_isCooked = _Value;}

        bbool   isPending() const           {return m_bPending;}
        void    setPending(bbool _bPending) {m_bPending = _bPending;}

        bbool   isDeleteMe() const              {return m_bDeleteMe;}
        void    setDeleteMe(bbool _bDeleteMe)   {m_bDeleteMe = _bDeleteMe;}

        virtual void    setStatic(bbool _static);
        bbool   isStatic() const {return m_isStatic;}

        virtual void    setSubResource(bbool _sub) {m_isSubResource = _sub;}
        bbool   isSubResource() const {return m_isSubResource;}

         bbool   isBeingLoaded() {return m_bPhysicalLoaded || m_bPending;}

        //////////////////////////////////////////////////////////////////////////
        ///
        /// Tells a resource it's used by one (aditional) user:
        /// - Updates logical'e reference counter (increments)
#ifdef ASSERT_ENABLED
        void  addLogicalDataUser      (                        ) ;
        static const u16* getTracedResourceName();
#else
        ITF_INLINE void  addLogicalDataUser      (                        ) { m_logicalDataRefCount++; }
#endif

        const Path&   getPath() const {return m_path;}
        
#ifndef ITF_FINAL

        static ResourceType  getResourceTypenameByType(const String &_typename);
        void    getTypeName(String& _typename);
        virtual String  getCustomInfo() {return "";}
        void    dumpInfo(Blob& _blob);

        f32     getTimeToLoad() const           {return m_fTimeToLoad;}
        void    setTimeToLoad(f32 _timetoload)  {m_fTimeToLoad = _timetoload;}

        u32     getMemoryUsage()    const               {return m_memoryUsage;}
        void    setMemoryUsage(u32 _memoryUsage)        {m_memoryUsage = _memoryUsage;}

#endif //!ITF_FINAL


#ifdef ITF_SUPPORT_COOKING
        virtual bbool mustBeCooked()      const   {return bfalse;}
        virtual bbool fileCookAvailable();
        virtual void setUseDefault(Resource* _pDefaultResource)   {_pDefaultResource = _pDefaultResource;};

        ITF_INLINE bbool isShared() {return m_bUseAsShared;} 

        ITF_INLINE void setAsShared(bbool _bShared) {m_bUseAsShared = _bShared;}

        virtual void cancelDefault()              {};
#endif //ITF_SUPPORT_COOKING
        

protected:
       friend class ResourceLoader;

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Resets resource reference count and sets path data
    /// @path the path where the resource should be loaded
    ///
    Resource    (                          ) {ITF_ASSERT_MSG(bfalse, "forbidden to call default constructor");}
    Resource    (    const Path &path      );
    Resource    (    const String8 &_ID    );
    Resource    (    const char*    _ID    );

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Empty destructor
    ///
    virtual            ~Resource    (                        );

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Should be overloaded by all resources
    /// unloads from memory
    ///
    virtual void    flushPhysicalData() {ITF_ASSERT_MSG(bfalse, "pure virtual: must be implemented in children classes");}
    void            startFlushPhysicalData    ();
    void            endFlushPhysicalData      ();
    virtual void    load            ();
    virtual void    unload          ();

protected:
        Path      m_path;

    friend class ResourceGroup;

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Tells a resource it's used by one (aditional) user:
    /// - Updates resource'e reference counter (increments)
    /// - Loads the resource's data if not yet loaded
    bbool    addPhysicalDataUser        (                        );

private:
    friend class ResourceManager;

        u8          m_bDeleteMe:1;
        u8          m_bPending:1;
        u8          m_canBeFreed:1;
        u8          m_isStatic:1;
        u8          m_isSubResource:1;


        ResourceID  m_ID;
        bbool       m_isCooked;
        i32         m_physicalDataRefCount;
        i32         m_logicalDataRefCount;
        u32         m_TimeLifeBeforeDestroy;    //based on frame elapsed;
        bbool       m_bPhysicalLoaded;

#ifdef ITF_SUPPORT_COOKING
        bbool       m_bUseAsShared;
#endif //ITF_SUPPORT_COOKING
     
#ifndef ITF_FINAL
        f32         m_fTimeToLoad;         
        u32         m_memoryUsage;
#endif //!ITF_FINAL

        static ITF_VECTOR<ResourceListener*> m_listeners;

    void    keepOnlyOneLogicalUser();

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Loads the resource's binary data
    /// Should be overloaded by all resources
    /// @return btrue if success
    ///
    virtual bbool    tryLoadFile     (                        ) {ITF_ASSERT_MSG(bfalse, "pure virtual: must be implemented in children classes"); return bfalse;}


    //////////////////////////////////////////////////////////////////////////
    /// Tells a resource it's no more used (by one user):
    /// - Updates resource'e reference counter (decrements)
    /// - Frees the resource's binary data if no more user (null reference count)
    void    removePhysicalDataUser    (                        );

    // This is static because at the end we delete the resource if no one uses it
    static void removeLogicalDataUser ( Resource* _res );

    bbool isLogicalLoaded         (                        ) const { return m_logicalDataRefCount > 0; }
    int   getLogicalRefCount      (                        ) const { return m_logicalDataRefCount; }

    #ifdef ASSERT_ENABLED
        bbool   m_reloading;
    #endif


#ifdef  ENABLE_RESOURCE_HISTORY
        static ResourceHistoryManager resourceHistoryManager;
#endif //ENABLE_RESOURCE_HISTORY
};


}  // namespace ITF

#endif // _ITF_RESOURCE_H_
