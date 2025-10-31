#ifndef _ITF_ANIMATIONRES_H_
#define _ITF_ANIMATIONRES_H_

#ifndef _ITF_RESOURCE_H_
#include "engine/resources/resource.h"
#endif //_ITF_RESOURCE_H_

#ifndef _ITF_KEYARRAY_H_
#include "core/container/KeyArray.h"
#endif //_ITF_KEYARRAY_H_

#ifndef _ITF_ANIMATION_SERIALIZE_H_
#include "engine/animation/AnimationSerialize.h"
#endif //_ITF_ANIMATION_SERIALIZE_H_

namespace ITF
{
    class AnimSkeleton;
    class AnimSkeletonResource :  public Resource
    {
        DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(AnimSkeletonResource, Resource,1209914415);

    public:

        ResourceType getType()          const                       { return ResourceType_AnimSkeleton;}
        AnimSkeleton*   getOwnerData()                              { return m_pOwnerData;}
        void            setOwnerData(AnimSkeleton* _pAnimSkeleton)  { m_pOwnerData = _pAnimSkeleton;}

        void            setCanLoadBin(bbool _canLoadBin)            { m_canLoadBin = _canLoadBin;}
        bbool           getCanLoadBin(void) const                   { return m_canLoadBin;} 
        
        bbool           tryLoadFileByPlatform       (const String& _platform);

    private:
        friend class ResourceLoader; // Don't add more friends here. You MUST use RESOURCE_MANAGER to instanciate a resource

        AnimSkeletonResource  (const Path &path);
        virtual ~AnimSkeletonResource    ();
        void    flushPhysicalData (                        );
        bbool   tryLoadFile     (                        );


#ifdef ITF_SUPPORT_COOKING
        virtual bbool mustBeCooked()      const   {return btrue;}
#endif //ITF_SUPPORT_COOKING


        AnimSkeleton*     m_pOwnerData;
        bbool             m_canLoadBin;
    };

    class AnimPatchBank;
    class AnimPatchBankResource :  public Resource
    {
        DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(AnimPatchBankResource, Resource,534216680);

    public:

        ResourceType            getType()      const                         { return ResourceType_AnimPatchBank;}
        AnimPatchBank*          getOwnerData()                               { return m_pOwnerData;}
        void                    setOwnerData(AnimPatchBank* _pAnimPatchBank) { m_pOwnerData = _pAnimPatchBank;}
        void                    setCanLoadBin(bbool _canLoadBin)             { m_canLoadBin = _canLoadBin;}
        bbool                   getCanLoadBin(void) const                    { return m_canLoadBin;}

    private:
        friend class ResourceLoader; // Don't add more friends here. You MUST use RESOURCE_MANAGER to instanciate a resource

        AnimPatchBankResource(const Path &path) : Resource(path),  m_pOwnerData(0), m_canLoadBin(btrue){}
        void    flushPhysicalData (                        );
        bbool   tryLoadFile    (                        );

#ifdef ITF_SUPPORT_COOKING
        virtual bbool mustBeCooked()      const   {return btrue;}
#endif //ITF_SUPPORT_COOKING


        AnimPatchBank*     m_pOwnerData;
        bbool              m_canLoadBin;
    };

    typedef KeyArray<i32> NamesMap;
    class AnimTrack;
    class AnimTrackResource : public Resource
    {
    public:
        virtual ResourceType            getType             (   ) const {return ResourceType_AnimTrack;}

        StringID                        m_skeletonPath;         // will work because of ANIM_MANAGER stringID convert
        SafeArray<StringID>             m_texturePathList;

        NamesMap                        m_nameToPatch;
        bbool                           changeBank(const StringID &_name, const String &_path);   // step1

        ResourceGroup*                  getResourceGroup() const;
        bbool                           fillResources(ResourceGroup* _resGroup);           // step2

        void                            setTrack(AnimTrack * _trackPtr) { m_trackPtr = _trackPtr; }
        ITF_INLINE AnimTrack *          getTrack() { return m_trackPtr; }

        void                            setCanLoadBin(bbool _canLoadBin) {m_canLoadBin = _canLoadBin;}

        AnimTrackResource(const Path &path);
        ~AnimTrackResource();

#ifndef ITF_FINAL
        static  bbool                   m_loadRessourcesOnActor;
        virtual ResourceGroup*          getGroup() { return getResourceGroup(); }
#endif //ITF_FINAL
    
    private:
        friend class ResourceManager;

#ifndef ITF_FINAL
        ResourceID                      m_resourceGroup;
#endif //ITF_FINAL
        bbool                           m_canLoadBin; 

        ITF_VECTOR<ResourceID>          m_textureResourceIDList;
        ITF_VECTOR<ResourceID>          m_patchBankResourceIDList;

        AnimTrack *                     m_trackPtr;
        void                            clearLists();
        virtual void                    flushPhysicalData (   );
        virtual bbool                   tryLoadFile         (   );

#ifdef ITF_SUPPORT_COOKING
        virtual bbool mustBeCooked()      const   {return btrue;}
#endif //ITF_SUPPORT_COOKING

        static u32                      s_groupCount;
    };


    class AnimationDependenciesMap;
    class AnimDependenciesMapResource : public Resource
    {
        DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(AnimDependenciesMapResource,Resource,4011170951)
    public:
        virtual ResourceType                getType             (   ) const {return ResourceType_DependenciesMap;}
        static ResourceGroup *              getDenpendenciesGroup();
        static AnimationDependenciesMap *   getDependenciesMap(const Path & _animDepFile, ResourceGroup *_group);
        static bbool                        fillGroupDependencies( ResourceGroup *_group, const String & _anim, const Path & _depenencyFile, AABB *_aabb);
        static bbool                        fillPathList( ITF_VECTOR<Path> & _pathList, ITF_VECTOR<u32> & _pathTypeList, const String & _anim, const Path & _depenencyFile, AABB *_aabb );

        static void                         terminate();

        virtual bbool                       isPhysicalExist()                       { return btrue; }
        AnimationDependenciesMap *          getMap()                                { return m_animDependenciesMap; }
        void                                setMap(AnimationDependenciesMap *_map)  { m_animDependenciesMap = _map; }

        
        AnimDependenciesMapResource(Path _path)
            : Resource(_path), m_animDependenciesMap(NULL) {}

        ~AnimDependenciesMapResource();

    private:
        friend class ResourceManager;

        // Data are loaded on creation this is a false resource 
        virtual void                        flushPhysicalData (   ) {}
        virtual bbool                       tryLoadFile       (   );
        void                                clear             (   ) {}  

        AnimationDependenciesMap *          m_animDependenciesMap;

        static ResourceID                   s_dependencyGroup;

    };

} // namespace ITF

#endif // _ITF_ANIMATIONRES_H_
