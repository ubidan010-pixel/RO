#ifndef _ITF_PATCHDEPOT_H_
#define _ITF_PATCHDEPOT_H_

#ifndef _ITF_ANIMTEXPACKER_PATCH_H_
#include "tools/plugins/CookerPlugin/AnimTexPacker/Patch.h"
#endif

#ifndef _ITF_ANIMTEXPACKER_PATCH_H_
#include "tools/plugins/CookerPlugin/AnimTexPacker/Patch.h"
#endif

namespace nv {class Image;}

namespace ITF
{
    class AnimPatchBank;
    class AnimSkeleton;
    class AnimBone;
    class AnimTemplate;

    namespace AnimTexPacker
    {
        ///////////////////////////////////////////////////////////////////////////////////////////
        ///Depot : list of patchlists
        class PatchDepot : public ITF_LIST<PatchList>
        {
        public:
            ///////////////////////////////////////////////////////////////////////////////////////////
            ///Destructor
            ~PatchDepot();

            ///////////////////////////////////////////////////////////////////////////////////////////
            const PatchList * getInPatchDepot(const String &_patchImagePath);


            ///////////////////////////////////////////////////////////////////////////////////////////
            void addToPatchBankToPatchDepot(const String &imagePath, AnimPatchBank *patchBank, AnimSkeleton *skeleton, const ITF_VECTOR<StringID> * templateUsed);

        private:
            static bbool isVisuallyRootBone(AnimBone *_bone);
            static bbool BuildChainedBoneList(ITF_LIST<ChainedBones> &_chainedBoneList, AnimTemplate *_template);
            static void processTemplate(PatchList *_patchList, AnimPatchBank *_patchBank, nv::Image &image, const String8 &imagePath, AnimSkeleton *_skeleton, const ITF_VECTOR<StringID> * templateUsed);
        };
    }
}

#endif

