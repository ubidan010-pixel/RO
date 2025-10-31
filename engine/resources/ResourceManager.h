#ifndef _ITF_RESOURCEMANAGER_H_
#define _ITF_RESOURCEMANAGER_H_

#ifndef _ITF_TEMPLATESINGLETON_H_
#include "core/templateSingleton.h"
#endif //_ITF_TEMPLATESINGLETON_H_

#ifndef _ITF_RESOURCE_H_   
#include "engine/resources/Resource.h"
#endif //_ITF_RESOURCE_H_   

#ifndef _ITF_STRINGID_H_
#include "core/StringID.h"
#endif //_ITF_STRINGID_H_
#ifndef _ITF_SYNCHRONIZE_H_
#include "engine/system/Synchronize.h"
#endif //_ITF_SYNCHRONIZE_H_

#ifndef _ITF_PREFETCH_FCT_H_
#include "engine/boot/PrefetchFct.h"
#endif //_ITF_PREFETCH_FCT_H_

    class    TiXmlElement;

namespace ITF
{
    class    Font;
    class    Texture;
    class    ResourceGroup;
    class    ResourceLoader;
    class    Sound;
    class    Thread;
    class   StringID;

    typedef void (*CookingFunc)(const String& _filename); 
    typedef bbool (*CookingNeededFunc)(const String& _filename,bbool _hdMode); 
    typedef void (*resourceLoadCallback)(void * _context); 

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Handles all the resources and the resources groups
    class ResourceManager:public TemplateSingleton<ResourceManager>
    {
    public:

        static const u32        m_defaultQuerryOpTableSize  = 2048;

        static ResourceID                       m_cookingGroupID;

        enum LocalThreadId
        {
            invalidThread           = -1,
            mainThread              = 0,
            asyncresourceThread     = 1,
            asyncloadsceneThread    = 2,
            maxThread               = 3,
            
        };



        class ResourceError
        {
            public:
                enum  ResourceErrorType
                {
                    ResourceError_FileDoesNotExist = 0,
                    ResourceError_TryLoadFileFailed
                };
                ResourceError(const ResourceID& _id, ResourceErrorType _type) {m_id = _id; m_type = _type; m_clientsFound=0;}
                ResourceErrorType   m_type;
                ResourceID          m_id;
                u32                 m_clientsFound;
                const char*         getErrorLabel() const;
        };

        void                                getErrors(ITF_VECTOR<ResourceError>& _dest, bbool _flush = btrue);
        void                                flushErrors();

        static bool PrefetchResource( const Path& path, PrefetchFactory::Params& params, Resource::ResourceType type );

        static void registerLocalThreadId(LocalThreadId _value) {m_localThreadId.setValue(_value);}
        static ITF_INLINE LocalThreadId getLocalThreadId()      {return static_cast<LocalThreadId>(m_localThreadId.getValue());}
        
        static ThreadLocalStorage  m_localThreadId;
        
        ResourceManager();
        virtual ~ResourceManager();
        void    cleanPostDeletedSingletons();


        bbool   addUserToResourcePhysicalData ( const ResourceID& _resID );
        bbool   addUserToResourcePhysicalData ( Resource* _res );

        void    removeUserToResourcePhysicalData ( const ResourceID& _resID );
        void    removeUserToResourcePhysicalData ( Resource* _res );

        void    queryAddUserToResourcePhysicalData ( const ResourceID& _resID );
        void    queryAddUserToResourcePhysicalData ( Resource* _res );

        void    queryRemoveUserToResourcePhysicalData ( const ResourceID& _resID );
        void    queryRemoveUserToResourcePhysicalData ( Resource* _res );

        void    queryAddUserToResourceLogicalData ( const ResourceID& _resID );
        void    queryAddUserToResourceLogicalData ( Resource* _res );

        void    queryRemoveUserToResourceLogicalData ( const ResourceID& _resID );
        void    queryRemoveUserToResourceLogicalData ( Resource* _res );

        void    queryAddResourceToGroup( const ResourceID& _groupID, const ResourceID& _resID );

        void    removeUserToResourceLogicalData(const ResourceID& _resID);
        void    removeUserToResourceLogicalData(Resource* _res);

        void    removeUserToResourceData(const ResourceID& _resID);
        void    removeUserToResourceData(Resource* _res);

        void    flushResourcesForDestroy();

        ITF_INLINE ITF_THREAD_CRITICAL_SECTION& getQueryOpsCs() { return m_csResourceQueryOps; }

        //////////////////////////////////////////////////////////////////////////
        ///
        /// Reloads all the resources
        void reloadResources();


        //////////////////////////////////////////////////////////////////////////
        ///
        /// Removes all the resources physical data from memory
        void unloadAllResources();


        //////////////////////////////////////////////////////////////////////////
        ///
        /// Creates a new Resource
        /// @param    type          the type of the resource to be created
        /// @param    relativePath  the path of the    resource's file
        /// @return                 the created resource
        ResourceID newResourceIDFromFile(Resource::ResourceType type, const Path &relativePath);
        //const ResourceID newResourceIDFromFile(Resource::ResourceType type, const String &relativePath);

        bbool getResourceIdFromFile(ResourceID& _resourceID, const StringID &_path, Resource::ResourceType _type);

        void    unregisterResource(const ResourceID & _rID);
        void    deleteResource(Resource *_res);

        //////////////////////////////////////////////////////////////////////////
        ///
        /// Creates a new Resource
        /// @param    type      the type of the resource to be created
        /// @return             the created resource
        Resource *newEmptyResource(Resource::ResourceType type);

        //////////////////////////////////////////////////////////////////////////
        ///
        /// Warning: implementation is slow!
        /// @return the registered textures count
        u32     getTextureCount();

        //////////////////////////////////////////////////////////////////////////
        ///
        /// Warning: implementation is slow!
        /// @return the asked texture
        Texture* getTextureAt(u32 _index);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn void getAllTexture(ITF_VECTOR<Texture*> &mVTexture)
        /// @brief  Gets all texture. 
        /// @param [in,out] mVTexture   [in,out] If non-null, the v texture. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        void getAllTexture(ITF_VECTOR<Texture*> &mVTexture);

        /// @fn bbool reloadResource(const String& _path)
        /// @brief  Reload a specific resource. 
        /// @param  _path   Full path name of the resource you want to reload. 
        bbool reloadResource(const String& _path);

        void reloadResourcesByType(Resource::ResourceType _typeResource);

        void synchronizeResourcesAsLoaded();
        void synchronizeResourcesToProcessFromMainThread();

        void    init();

        Font*           getFont                 ( const ResourceID& id );

        void    removeFromDestoyList(const Resource* _res);
        bbool   isInDestoyList(const Resource* _res);

        struct ResourceLoadCallbackData
        {
            ResourceLoadCallbackData(ITF_VECTOR<ResourceID> & _resList,void * _context, resourceLoadCallback _cb) :
                resList(_resList),context(_context),cb(_cb){}
            ITF_VECTOR<
                ResourceID> resList;
            void * context;
            resourceLoadCallback cb;
        };

        void registerResourceLoadCallback(ITF_VECTOR<ResourceID> & _idList, void * _context, resourceLoadCallback _cb)
        {
            ITF_VECTOR<ResourceID> resources;
            resources.reserve(_idList.size());
            for (ITF_VECTOR<ResourceID>::const_iterator it = _idList.begin(); it != _idList.end();++it)
                resources.push_back((*it));

            registerResourceLoadCallbackRecord(resources,_context,_cb);
        }

        void registerResourceLoadCallbackRecord(ITF_VECTOR<ResourceID> & _resList, void * _context, resourceLoadCallback _cb)
        {
            const LocalThreadId localThreadid = getLocalThreadId();
            csAutoLock cs(m_csResourceCallbackThread[localThreadid]);
            m_resourceLoadCallbacksThread[localThreadid].push_back(ResourceLoadCallbackData(_resList,_context,_cb));
        }

        void registerResourceGroupLoadCallback(const ResourceID& _group, void * _context, resourceLoadCallback _cb);


#ifndef ITF_FINAL
        void dumpActivity(Blob& _blob,Resource::ResourceType _resourcetype,bbool bDisplayCooked,bbool bDisplayNotCooked);
		void dumpActivity(ITF_VECTOR<String>& strings);
        u32 getMemoryUsage(Resource::ResourceType type);

        struct DebugMark
        {
            Resource* m_ptr;
            String8 m_name;
            bbool   m_deleted;
            DebugMark()
            {
                m_ptr = NULL;
                m_deleted = bfalse;
            }
        };

        void setDebugMark();
        void removeFromMarks(Resource* _pResource);
        DebugMark* findMark(Resource* _pResource, u32 _index);
        DebugMark* findMark(const String8& _name, u32 _index);
        u32 m_debugMarkIndex;
        ITF_VECTOR<DebugMark> m_marks[2];
#endif //!ITF_FINAL

        //////////////////////////////////////////////////////////////////////////
        ///
        /// Asks to load the resource's binary data
        ///
        void    requestFileLoading    (  const ResourceID& _res );

        bbool forceSynchronousLoading(const Resource::ResourceType _type);
        void forceResourceAnimSkeletonLoading(ResourceID _res,const String& _platform); // only for AnimSkeleton resource in specific conditions !!!

        bbool loadResourcesPending();
        bbool isResourceLoadPending(const Resource* pRes) const;

        void launch();

        void asyncLoad();

        void    setEnableAsync(bbool _bEnableAsync)        {m_bEnableAsync = _bEnableAsync;};
        bbool   isEnableAsync()                           {return m_bEnableAsync;}

        bbool isValidResource(const Resource* pRes);
        bbool isValidResource(const Resource::ResourceType _type, const Resource* pRes);
        void requestResourcePhysicalRelease(Resource* _res);
        void requestResourceLogicalRelease(Resource* _res);

        void registerCooking(CookingFunc _cookingFunction)                      {m_cookFunc = _cookingFunction;}
        void registerHResCooking(CookingFunc _cookingNeededFunction)            {m_cookFuncHRes = _cookingNeededFunction;}

        void registerNeededCooking(CookingNeededFunc _cookingNeededFunction)    {m_cookNeededFunc = _cookingNeededFunction;}
        
        
        
        u32 getResourceForDestroyPhysicalCount() const { return m_vResourceForDestroy.size(); }
        u32 getResourceForDestroyLogicalCount() const { return m_vResourceForDestroyLogical.size(); }
        u32 getResourceForDestroyCount() const { return m_vResourceForDestroy.size() + m_vResourceForDestroyLogical.size(); }

        //call by frame 
        void manageDestruction( bbool shutdown, bbool _deleteLogicalAllowed );

        // the 2 following methods can be use to stop the resource manager with a busy loop
        void requestStopAsync();
        bbool isStoppedAsync() const { return m_pAsyncThread==NULL || m_exitedAsync; }
        // Or this one is waiting on event
        void stopAsync();

        void synchronize( );

        void addResourceAsLoaded(Resource* pResource);
        void addResourceToProcessFromMainThread(Resource* pResource);

        void callResourceCallBack();

        void setLoadAllowed(bbool _loadAllowed)     {m_loadAllowed = _loadAllowed;}
        bbool isLoadAllowed() const                 {return m_loadAllowed;}

		const bbool isModeFinal() const			{return m_bModeFinal;}
		void setModeFinal(bbool _bModeFinal)	{m_bModeFinal = _bModeFinal;}


        void  setDeleteLogicalAllowed(bbool _deleteLogicalAllowed)  {m_deleteLogicalAllowed = _deleteLogicalAllowed;}
        bbool isDeleteLogicalAllowed() const                        {return m_deleteLogicalAllowed;}

        void    pause();
        void    unpause();
        ITF_INLINE bbool   isPaused() const { return m_paused != 0; }

        
        void setHotReloading(bbool _hotReloading)                       {m_hotReloading = _hotReloading;}
        bbool isHotReloading()                       {return m_hotReloading;}

        //Delay to load a resource from the async thread in milliseconds
        void setDelayToLoad(u32 _delayToLoad)                       {m_DelayToLoad = _delayToLoad;}
        void setImmediateFlushMode(bbool _bImmediateFlushMode);
        bbool getImmediateFlushMode() { return m_bImmediateFlushMode; }

        bbool isCookedFileAllowed(const String _filename);
        static bool getAlternativeFileNamesForCooking(const String& _resourcePathName, Vector<String> & _altResourcePathNames, const String& _platform); // return true if there is at least an alternative name
        static String getExistingAlternativeFileNameForCooking(const String& _resourcePathName, const String& _platform); // return _resourcePathName or an alternative if the file exists

        void setAllowTextureCooked(bbool _TextureCookedSupported)   { m_bAllowTextureCooked = _TextureCookedSupported; }

        void    disableDefaultResourceUsage() {m_bDisableDefaultResourceUsage = btrue;}
        
        void requestCooking(const String& _filename);
        bbool cookNeeded(const String& _filename);
        void flushPendingOps();
        Resource* getResourceByPath_slow(const StringID _path);
        static ResourceID m_InvalidResourceID;

#ifdef MODE_HRESTEXTURE_SUPPORTED
       bbool cookHResNeeded(const String& _filename);
       void  cookHRes(const String& _filename);
#endif //MODE_HRESTEXTURE_SUPPORTED


#ifdef ASSERT_ENABLED
        bbool canResourceBeDeleted(const ResourceID _id);
        bbool   checkIfResourceContainsTrackedResource(Resource* res);
#endif // ASSERT_ENABLED

        bbool hasResourcesToLoad() const;
        bbool hasAddQueryOps() const { return m_queryAddOps.size() > 0; }
        bbool hasRemoveQueryOps() const { return m_queryRemoveOps.size() > 0; }
        bbool hasQueryOps() const { return hasAddQueryOps() || hasRemoveQueryOps(); }

        void    updateFiles();
        void    updateFilesToLoad();
        void    updateAsyncFilesToLoad();

#ifndef ITF_DISABLE_DEBUGINFO
        ITF_INLINE long getCurrentCountLoad() {return m_currentCountLoad;}
#endif//ITF_DISABLE_DEBUGINFO

    private:

        void    callResourceDestructor(Resource *_res);

        ITF_INLINE ITF_MAP<StringID, Resource*>& getResourceMapFromType(Resource::ResourceType _type) {ITF_ASSERT((u32)_type < (u32)Resource::ResourceType_Count); return mResourcesById[(u32)_type];}

        enum QueryOpType
        {
            QueryOp_AddPhysicClient,
            QueryOp_RemovePhysicClient,
            QueryOp_AddLogicClient,
            QueryOp_RemoveLogicClient,
            QueryOp_AddResourceToGroup,
            ENUM_FORCE_SIZE_32(QueryOpType)
        };

        enum QueryUpdateType
        {
            QueryUpdate_AddOnly,
            QueryUpdate_RemoveOnly,
            QueryUpdate_All,
            ENUM_FORCE_SIZE_32(QueryUpdateType)
        };

        typedef struct QueryAddOp
        {
//            QueryAddOp( const ResourceID& _target, QueryOpType _op, const ResourceID& _secondArg) {m_target = _target; m_op = _op; m_secondaryArg = _secondArg;}

            Resource*  m_target;
            Resource*  m_secondaryArg;
            QueryOpType m_op;
        } QueryAddOp;

        typedef struct QueryRemoveOp
        {
  //          QueryRemoveOp( const ResourceID& _target, QueryOpType _op) {m_target = _target; m_op = _op;}

            Resource*  m_target;
            QueryOpType m_op;
        } QueryRemoveOp;

        void            addQueryOp(const ResourceID& _id, QueryOpType _op, const ResourceID& _secondArg = ResourceID::Invalid);
        void            updateQueryOps(QueryUpdateType _type = QueryUpdate_All);
        void            updateQueryOpsAddLogical();
        void            executeQueryOp(Resource* _res, QueryOpType _op, Resource* _secondArg = NULL);
        void            setLinearMode(bbool _mode)  { m_useLinearMode = _mode;}
        ITF_INLINE      bbool isLinearMode()        {return m_useLinearMode;}

        SafeArray<QueryAddOp> m_queryAddOps;
        SafeArray<QueryRemoveOp> m_queryRemoveOps;

        friend class XMLAll;    // loading class can access members directly

        void    updateCurrentResourcePending();

        void    requestFileLoadingInternal( const ResourceID& _id );
        void    flushStaticResources(bbool _parentsOnly);

        ITF_INLINE ResourceID newResourceIDFromFile_Normalized(Resource::ResourceType type, const Path &_relativePath);

        // resource loader:
        ResourceLoader*                        mResourceLoader;

        CookingFunc                            m_cookFunc;
        CookingFunc                            m_cookFuncHRes;
        CookingNeededFunc                      m_cookNeededFunc;

        ResourceID*                            m_ResourceDefaultID;

        ITF_MAP<StringID, Resource*>           mResourcesById[Resource::ResourceType_Count];    // Links resource to all of their possible IDs.
        ITF_VECTOR<ResourceID>                 m_resourcesToLoad;
        typedef ITF_VECTOR<ResourceLoadCallbackData> VResourceLoadCallback;
        VResourceLoadCallback                  m_resourceLoadCallbacks;
        VResourceLoadCallback                  m_resourceLoadCallbacksThread[maxThread]; //each "loading" thread owns its callback vector

        VResourceLoadCallback                  m_CBtoSend;


        //Asynchronous section
        Thread*                                 m_pAsyncThread;

        bbool                                   m_bAllowTextureCooked;

        bbool                                   m_bEnableAsync;
        volatile long                           m_paused;
        bbool                                   m_loadAllowed;
        bbool                                   m_deleteLogicalAllowed;
		bbool									m_bModeFinal;
        bbool                                   m_bDisableDefaultResourceUsage;
        bbool                                   m_useLinearMode;
        u32                                     m_DelayToLoad; //in ms
        bbool                                   m_hotReloading;

#ifndef ITF_DISABLE_DEBUGINFO
        volatile long                           m_currentCountLoad;
#endif


        volatile bbool                          m_stopAsync;
        volatile bbool                          m_exitedAsync;

        bbool                                   m_bImmediateFlushMode; //true to remove a physical resource immediately

        ITF_VECTOR<ResourceID>                  m_ResourceLoadingPending;
        ITF_VECTOR<ResourceID>                  m_ResourceAsLoaded; //resource is physically loaded;
        ITF_VECTOR<ResourceID>                  m_resourceToProcessFromMainThread;
        ITF_VECTOR<ResourceID>                  m_ResourceToProcessFromMainThread;
        ITF_VECTOR<ResourceID>                  m_vResourceAsync;
        ITF_VECTOR<ResourceID>                  m_vResourceAsyncFromScene; //resource added by async scene
        
        ITF_VECTOR<ResourceID>                  m_vResourceForDestroy;
        ITF_VECTOR<ResourceID>                  m_vResourceForDestroyLogical;

        ITF_THREAD_CRITICAL_SECTION             m_csResourceCurrentLoaded;//CS to protect resource currently loaded
        ITF_THREAD_CRITICAL_SECTION             m_csResourceAsLoaded; //CS to protect resource set as loaded
        ITF_THREAD_CRITICAL_SECTION             m_csResourceToProcess;
        ITF_THREAD_CRITICAL_SECTION             m_csResourceAsyncAdded;//CS to protect new resource added from the resource manager thread
        ITF_THREAD_CRITICAL_SECTION             m_csResourceAsyncSceneAdded;//CS to protect new resource added from the scene manager thread
        ITF_THREAD_CRITICAL_SECTION             m_csResourceQueryOps;
        ITF_THREAD_CRITICAL_SECTION             m_csResourceError;
        ITF_THREAD_CRITICAL_SECTION             m_csResourceCallbackThread[maxThread];//CS to protect callback resources add
        ITF_THREAD_CRITICAL_SECTION             m_csResourceCreation;

        ITF_THREAD_EVENT                        m_exitEvent; 
        ITF_THREAD_EVENT                        m_activeloadEvent; 

        ResourceID                              m_defaultResourceGroup;
        ITF_VECTOR<ResourceError>               m_errors;

    #ifdef ITF_NINTENDO
        const bool                                    m_loadBundleDynamically = false;
    #else
        const bool                                    m_loadBundleDynamically = true;
    #endif
		
        void                                    addError(const ResourceID& _id, ResourceError::ResourceErrorType _type);
    };

#define RESOURCE_MANAGER ResourceManager::getptr()

#ifndef ITF_DISABLE_DEBUGINFO
#define SETCURRENTCOUNDLOAD(_val) m_currentCountLoad = _val;
#define ADDCURRENTCOUNDLOAD(_val) m_currentCountLoad += _val;
#else
#define SETCURRENTCOUNDLOAD(_val)
#define ADDCURRENTCOUNDLOAD(_val)
#endif //ITf_FINAL

} // namespace ITF

#endif //_ITF_RESOURCEMANAGER_H_

