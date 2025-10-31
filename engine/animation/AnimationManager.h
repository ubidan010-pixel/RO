#ifndef _ITF_ANIMATIONMANAGER_H_
#define _ITF_ANIMATIONMANAGER_H_

#ifndef _ITF_TEMPLATESINGLETON_H_
#include "core/templateSingleton.h"
#endif //_ITF_TEMPLATESINGLETON_H_

#ifdef ITF_WII
#define ITF_COMPRESSED_ANIM
#endif //ITF_WII

namespace ITF
{   
    class   AnimationResource;

    class   AnimSkeletonResource;
    class   AnimPatchBankResource;
    class   AnimTrackResource;
    class   AnimInfo;
	class	AnimTrack;

    class   Patch32Data;

    class AnimManager:public TemplateSingleton<AnimManager>
    {
    protected:
        struct AnimData
        {
            enum ProcessType
            {
                anim_synchronize = 0,
                anim_unloadResource
            };

            ProcessType m_type;
            ObjectRef   m_actor;
            AnimInfo *  m_info;

            AnimData()
            : m_info(NULL), m_type(anim_synchronize)
            {}
            
            AnimData(ProcessType _type, ObjectRef _actor, AnimInfo * _info)
            : m_type(_type), m_actor(_actor), m_info(_info)
            {}

        };

        ITF_MAP<StringID, String>   m_idToString;
        Patch32Data*                m_PData;
        SafeArray<AnimData>         m_animSyncroList;   


#if defined(ITF_WINDOWS) && !defined(ITF_FINAL)
        ITF_MAP<StringID, ITF_VECTOR<Path>> m_skeletonToTrack;

    public:
        bbool getAnimTrackUsingSkeleton(StringID _skeletonPath, ITF_VECTOR<Path> & list)
        {
            ITF_MAP<StringID, ITF_VECTOR<Path> >::iterator iter = m_skeletonToTrack.find(_skeletonPath);
            if (iter == m_skeletonToTrack.end())
                return bfalse;

            list = iter->second;
            return btrue;
        }
#endif

#ifdef ITF_COMPRESSED_ANIM
	protected:
		struct CachedAnim
		{
			AnimTrack*	m_track;
			u32			m_frameCounter;
		};
		Vector<CachedAnim>		m_compressionCache;
		u32						m_frameCounter;

	public:
		void UseTrack(AnimTrack *track);
#endif //ITF_COMPRESSED_ANIM

    public:
        AnimManager();
        virtual ~AnimManager();
        
        void        dumpData();
        
        StringID        getStringID(const String & _str);
        const String &  getString(const StringID & _str);
        void            setStringStringID(const String & _str, const StringID & _strId);

        // new
        /////////// Skeleton
        bbool                   loadAnimSkeleton(AnimSkeletonResource * _animSkeletonResource, const Path& _path,const String& _platform, bbool _tryLoadingBin);
        void                    unloadAnimSkeleton(AnimSkeletonResource * _animSkeletonResource);

        /////////// PatchBank
        bbool                   loadAnimPatchBank(AnimPatchBankResource * _animPatchBankResource, const Path& _path, bbool _tryLoadingBin);
        void                    unloadAnimPatchBank(AnimPatchBankResource * _animPatchBankResource);

        /////////// Tracks
        bbool                   loadAnimTrack(AnimTrackResource * _animTrackResource, const Path& _path, ResourceGroup *_resGroup, bbool _tryLoadingBin);
        void                    unloadAnimTrack(AnimTrackResource * _animTrackResource);

        Patch32Data*            getPatchDataBuffer() { return m_PData; };

        void                    registerAnimForSyncro(AnimInfo * _info, ObjectRef _actor) { m_animSyncroList.push_back(AnimData(AnimData::anim_synchronize, _actor, _info));}
        void                    registerAnimForResourceUnload(ObjectRef _actor) { m_animSyncroList.push_back(AnimData(AnimData::anim_unloadResource, _actor, NULL)); }
        void                    unregisterAnimForResourceUnload(ObjectRef _actor);


        bbool                   IsInfoInAnimForSyncro(AnimInfo * _info, ObjectRef * _actor = NULL);
        void                    synchronize();
        bbool                   isEmptySynchroList() const { return m_animSyncroList.size() == 0; };
    };

#define ANIM_MANAGER AnimManager::getptr()
};

#endif //_ITF_ANIMATIONMANAGER_H_ 
