#ifndef _ITF_ANIMATIONLOADER_H_
#define _ITF_ANIMATIONLOADER_H_

#ifndef _ITF_ANIMPACKERCONFIGURATION_H_
#include "tools/plugins/CookerPlugin/AnimTexPacker/AnimPackerConfiguration.h"
#endif

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_PATCHDEPOT_H_
#include "tools/plugins/CookerPlugin/AnimTexPacker/PatchDepot.h"
#endif

#ifndef _ITF_PBKMERGERLIST_H_
#include "tools/plugins/CookerPlugin/AnimTexPacker/PbkMergerList.h"
#endif

namespace ITF
{
    class ResourceGroup;
    class AnimTrack;

    namespace AnimTexPacker
    {
        ///////////////////////////////////////////////////////////////////////////////////////////
        ///Loads, parses, packs a list of animations
        class AnimationLoader
        {
        public:
            ///////////////////////////////////////////////////////////////////////////////////////////
            ///Construction
            AnimationLoader();

            ///////////////////////////////////////////////////////////////////////////////////////////
            ///Destruction
            ~AnimationLoader();

            ///////////////////////////////////////////////////////////////////////////////////////////
            ///Loads and parse
            ResourceGroup *loadAnimations(const ITF_VECTOR<String> &animPathList);

            ///////////////////////////////////////////////////////////////////////////////////////////
            /// Secondary depot
            void  fillSecondaryPatchDepot(int & serialMergeIndex, const AnimPackerConfiguration &_config, const String &_platform);

            ///////////////////////////////////////////////////////////////////////////////////////////
            ///Packs
            bbool buildPackedMap(const AnimPackerConfiguration &_config, const String &_platform);

            ///////////////////////////////////////////////////////////////////////////////////////////
            ///Packs
            bbool buildBasicPack(const AnimPackerConfiguration &_config, const String &_platform);

#if !defined(ITF_FINAL) && defined(ITF_WINDOWS)
            ///////////////////////////////////////////////////////////////////////////////////////////
            ///Export GenAnim animations to directory
            void exportAnimations(const String8 &_directory);
#endif
            void cookAnimations(const String8 &_directory, const String & _platform);

            ///////////////////////////////////////////////////////////////////////////////////////////
            u32 getMergeListCount() { return u32(m_mergerList.size()); }

            ///////////////////////////////////////////////////////////////////////////////////////////
            void modifyAnimations(const AnimPackerConfiguration &_config);

        private:
            ///////////////////////////////////////////////////////////////////////////////////////////
            void mayUpdateBoneCount(u32 _boneCount);

            ///////////////////////////////////////////////////////////////////////////////////////////
            StringID findSeedBone(const StringID &_boneID);

            ///////////////////////////////////////////////////////////////////////////////////////////
            AnimTrack * parseAnimation(ResourceID _trackRes);

            ///////////////////////////////////////////////////////////////////////////////////////////
            void unloadAnimations();

            ///////////////////////////////////////////////////////////////////////////////////////////
            void  clearSecondaryPatchDepot();

        private:
            PatchDepot m_patchDepot;
            ITF_VECTOR<PatchDepot *> m_secondaryPatchDepot;
            ResourceGroup *m_resourceGroup;
            ITF_VECTOR<BoneMaxSize> m_boneMaxSizes;
            SafeArray<AnimTrack*> m_loadedAnimations;
            ITF_VECTOR<String> m_loadedAnimationPathes;
            PbkMergerList m_mergerList;
            AnimSkeleton *m_skeleton;



        };
    }
}

#endif
