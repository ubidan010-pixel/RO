#include "core/enginecommon.h"

#ifdef ITF_WINDOWS

#ifndef _ITF_PBKMERGERLIST_H_
#include "tools/plugins/CookerPlugin/AnimTexPacker/PbkMergerList.h"
#endif

#ifndef _ITF_ANIMPACKERCONFIGURATION_H_
#include "tools/plugins/CookerPlugin/AnimTexPacker/AnimPackerConfiguration.h"
#endif

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_ANIMATIONTRACK_H_
#include "engine/animation/AnimationTrack.h"
#endif //_ITF_ANIMATIONTRACK_H_

namespace ITF
{
    namespace AnimTexPacker
    {
        ///////////////////////////////////////////////////////////////////////////////////////////
        PbkMergerList::~PbkMergerList()
        {
            for (PbkMergerList::iterator iter=begin();
                iter!=end();
                iter++)
            {
                PbkMerger &merger = *iter;
                merger.destroyCompositePatchBank();
            }
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        PbkMerger *PbkMergerList::getPbkMergerFromTemplateID(const StringID &_templateID)
        {
            for (PbkMergerList::iterator iter=begin();
                iter!=end();
                iter++)
            {
                PbkMerger &merger = *iter;
                if (merger.findPatch(_templateID))
                    return &merger;
            }
            return NULL;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        void PbkMergerList::buildPbkMergerList(int & _serialMergerIndex, const AnimPackerConfiguration &_config, PbkMergerList &_mergers, PatchDepot &_depot)
        {
            SafeArray<bbool> isPatchBankMerged;
            isPatchBankMerged.reserve(_depot.size());
            for (u32 i=0; i<_depot.size(); i++)
            {
                isPatchBankMerged.push_back(bfalse);
            }

            AnimPackerConfiguration::PbkConcatDescList::const_iterator concatIter;
            for (concatIter = _config.m_concatenatedPbkList.begin();
                concatIter !=  _config.m_concatenatedPbkList.end();
                concatIter++)
            {
                const AnimPackerConfiguration::PbkConcatDesc &pbkConcatDesc=*concatIter;
                //build 1 PbkMerger from a PbkConcatDesc
                PbkMerger merger(_serialMergerIndex++);

                PatchDepot::iterator iter;
                int pbkIndex=0;
                for (iter=_depot.begin(); iter!=_depot.end(); iter++, pbkIndex++)
                {
                    if (isPatchBankMerged[pbkIndex])
                    {
                        continue;
                    }

                    PatchList &patchList = *iter;
                    String8 filename(FilePath::getFilenameWithoutExtension(patchList.m_patchImagePath).cStr());
                    if (pbkConcatDesc.checkFileMatch(filename))
                    {
                        isPatchBankMerged[pbkIndex] = btrue;
                        merger.m_listOfPatchLists.push_back(&patchList);
                    }
                }
                //
                if (merger.m_listOfPatchLists.size()!=0)            
                {
                    _mergers.push_back(merger);
                }
            }
        }

        void PbkMergerList::fillPbkRedirect( const AnimPackerConfiguration &_config, ITF::PbkRedirect & _redirect )
        {
            for (PbkMergerList::const_iterator iter=begin();
                iter!=end();
                iter++)
            {
                const PbkMerger &merger = *iter;
                ITF_LIST<PatchList*>::const_iterator patchListIter;

                _redirect.m_patchBankPathList.push_back(_config.m_mainPath+merger.getCompositeName()+".tga");
                u32 bankIndex = _redirect.m_patchBankPathList.size()-1;

                for (patchListIter=merger.m_listOfPatchLists.begin();
                    patchListIter!=merger.m_listOfPatchLists.end();
                    patchListIter++)
                {
                    PatchList *patchList = *patchListIter;

                    ITF_LIST<Patch*>::iterator patchIter;
                    for (patchIter=patchList->m_patches.begin(); patchIter!=patchList->m_patches.end(); patchIter++)
                    {
                        Patch    *patch     = *patchIter;
                        String   imagePath  = patch->getImagePath();

                        FilePath::normPath(imagePath);
                        FilePath::transformToRelative(imagePath);

                        StringID imageId    = imagePath;
                        KeyArray<i32>::Iterator it(_redirect.m_oldPatchBankToNew);
                        it.findKey(imageId);
                        if (it.isEnd())
                        {
                            _redirect.m_oldPatchBankToNew.setID(imageId, bankIndex);
                        }
                    }
                }
            }
        }
    }
}
#endif
