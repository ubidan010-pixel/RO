#ifndef _ITF_PBKMERGER_H_
#define _ITF_PBKMERGER_H_

#ifndef _ITF_PACKEDTEXTUREAREA_H_
#include "tools/plugins/CookerPlugin/AnimTexPacker/PackedTextureArea.h"
#endif

#ifndef _ITF_SAFEARRAY_H_
#include "core/container/SafeArray.h"
#endif //_ITF_SAFEARRAY_H_



namespace nv {class Image;}

namespace ITF
{
    class AnimPatchBank;

    namespace AnimTexPacker
    {
        class Patch;
        class PatchList;
        class AnimPackerConfiguration;
        class PatchDepot;

        ///////////////////////////////////////////////////////////////////////////////////////////
        ///Class for merging PBKs
        class PbkMerger
        {
        public:
            PbkMerger(int _serial)
            : m_compositeAnimPatchBank(NULL)
            , m_serialNumber(_serial)
            {
                m_compositeName.setTextFormat("pbk%03d",_serial+1);
            }

            ITF_LIST<PatchList*>    m_listOfPatchLists;
            AnimPatchBank *         m_compositeAnimPatchBank;
            int                     m_serialNumber;
            String8                 m_compositeName;

            ///////////////////////////////////////////////////////////////////////////////////////////
            const String8 &getCompositeName() const {return m_compositeName;}
            ///////////////////////////////////////////////////////////////////////////////////////////
            Patch *findPatch(const StringID &_templateID);

            ///////////////////////////////////////////////////////////////////////////////////////////
            void computeFinalMapStartupSize(int &mapWidth, int &mapHeight, int &totalArea);

            ///////////////////////////////////////////////////////////////////////////////////////////
            void buildSortedPatches(SafeArray<Patch*> &_sortedPatchList);

            ///////////////////////////////////////////////////////////////////////////////////////////
            void createCompositePatchBank(PatchDepot *_patchDepot);

            ///////////////////////////////////////////////////////////////////////////////////////////
            void destroyCompositePatchBank();

            ///////////////////////////////////////////////////////////////////////////////////////////
            bbool buildPackedMap(const AnimPackerConfiguration &_config);

        private:
            static void buildSortedMapSizes(SafeArray<BoxI> &_sortedMapSizes, int mapWidth/*must be pow of 2*/, int mapHeight, int maxUV);
            static nv::Image blitFinalImage(int mapWidth, int mapHeight, const SafeArray<Patch*> &_inputPatchList);
        };
    }

}

#endif