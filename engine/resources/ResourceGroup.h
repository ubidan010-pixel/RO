#ifndef _ITF_RESOURCEGROUP_H_
#define _ITF_RESOURCEGROUP_H_

#ifndef _ITF_RESOURCE_H_   
#include "engine/resources/Resource.h"
#endif //_ITF_RESOURCE_H_   

#ifndef _ITF_STRINGID_H_
#include "core/StringID.h"
#endif //_ITF_STRINGID_H_

namespace ITF
{

    //////////////////////////////////////////////////////////////////////////
    ///
    /// A ResourceGroup references a list of resources. The same group can be loaded several 
    /// times, provided it's deleted the same amount of times (can be useful to have 
    /// independant modules).
    class ResourceGroup : public Resource
    {
    public:
        typedef ITF_VECTOR<ResourceID>  ResourceList;

        ITF_INLINE bool                 isEmpty() { return m_ResourcesIDs.size()==0; }
        ResourceID                      addResource( Resource::ResourceType _type, const Path& _path );
        bbool                           addResource( Resource* _res, bbool _addLogicUser = btrue, bbool _checkDoublons = btrue );
        bbool                           removeResource (const ResourceID &_id, bbool _recurseSubGroups = bfalse);
        void                            clearAll(bbool _recurseSubGroups = bfalse);
        virtual void    load            ();
        virtual void    unload          ();

        ITF_INLINE const ResourceList&  getResourcesList() const {return m_ResourcesIDs;}
        bbool                           isPhysicallyLoaded(); // not const because some variables may be updated here to avoid systematic re-evaluation
        void                            updatePhysicallyLoaded();
        void                            addPhysicalUserFromMainThread();

        ResourceType                    getType()const     {return ResourceType_ResourceGroup;}
        void                            flushPhysicalData        ();
        bbool                           tryLoadFile    ();
        bbool                           isPhysicalExist();
        bbool                           delayPhysicalFlush() {return bfalse;}
        bbool                           usesResource(const ResourceID& _id, bbool _recurseSubGroups = bfalse);
        ResourceGroup*                  getGroup() {return this;}
        ITF_INLINE bbool				loadingQuerried() const {return m_alreadyLoaded;}
        void                            dumpUnloadedResources(String8& _msg);
        virtual void                    setStatic(bbool _static);

        ITF_INLINE void                 forcePhysicallyReadyRecompute() { m_physicallyReady = bfalse; }

    private: // methods reserved to the ResourceManager
        friend class ResourceManager;

        ResourceGroup();
        virtual ~ResourceGroup();

    private:
        // maps resouce ids to resource paths:
        ResourceList        m_ResourcesIDs;

        static void         initStaticData();
        static void         deleteStaticData();
        bbool               m_alreadyLoaded, m_physicallyReady;
    };
} // namespace ITF

#endif // _ITF_RESOURCEGROUP_H_