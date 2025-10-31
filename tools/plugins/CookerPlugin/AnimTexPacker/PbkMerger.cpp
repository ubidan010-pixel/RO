#include "core/enginecommon.h"

#ifdef ITF_WINDOWS

#ifndef _ITF_PBKMERGER_H_
#include "tools/plugins/CookerPlugin/AnimTexPacker/PbkMerger.h"
#endif

#ifndef _ITF_ANIMTEXPACKER_PATCH_H_
#include "tools/plugins/CookerPlugin/AnimTexPacker/Patch.h"
#endif

#ifndef _ITF_ANIMATIONPATCHBANK_H_
#include  "engine/animation/AnimationPatchBank.h"
#endif // _ITF_ANIMATIONPATCHBANK_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_ASSERTMANAGER_H_
#include "core/AssertManager.h"
#endif //_ITF_ASSERTMANAGER_H_

#ifndef _ITF_ANIMPACKERCONFIGURATION_H_
#include "tools/plugins/CookerPlugin/AnimTexPacker/AnimPackerConfiguration.h"
#endif

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_FILEMANAGER_H_
#include "core/AdaptersInterfaces/FileManager.h"
#endif //_ITF_FILEMANAGER_H_

#ifndef _ITF_SOURCECONTROLADAPTER_H_
#include "engine/AdaptersInterfaces/SourceControlAdapter.h"
#endif // _ITF_SOURCECONTROLADAPTER_H_

#ifndef _ITF_PATCHDEPOT_H_
#include "tools/plugins/CookerPlugin/AnimTexPacker/PatchDepot.h"
#endif

//#define _ONLY_SQUARE_TEXTURES

namespace ITF
{
    namespace AnimTexPacker
    {
        ///////////////////////////////////////////////////////////////////////////////////////////
        static inline int getUpperPowerOf2(int x)
        {
            x -= 1;
            x |= (x >> 1);
            x |= (x >> 2);
            x |= (x >> 4);
            x |= (x >> 8);
            x |= (x >> 16);

            return x + 1;
        }


        ///////////////////////////////////////////////////////////////////////////////////////////
        //for sorting map by area with qsort
        int mapSizeComparator(const void *_p1, const void *_p2)
        {
            BoxI &b1=*(BoxI*)_p1;
            BoxI &b2=*(BoxI*)_p2;

            int a1 = b1.maxx*b1.maxy;
            int a2 = b2.maxx*b2.maxy;
            if (a1<a2)
                return -1;
            else if (a1==a2)
            {
                int aspect1 = abs(b1.maxx-b1.maxy);
                int aspect2 = abs(b2.maxx-b2.maxy);
                if (aspect1<aspect2)
                    return -1;
                else if (aspect1==aspect2)
                    return 0;
                return 1;
            }
            return 1;

        }
        //For sorting by perimeter with qsort
        static int patchComparator(const void *_p1, const void *_p2)
        {
            Patch* patch1 = *(Patch**)_p1;
            Patch* patch2 = *(Patch**)_p2;

            int halfPerim1 = patch1->getSubImageWidth()+patch1->getSubImageHeight();
            int halfPerim2 = patch2->getSubImageWidth()+patch2->getSubImageHeight();

            if (halfPerim1<halfPerim2)
                return 1;
            else if (halfPerim1==halfPerim2)
                return 0;
            return -1;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        void PbkMerger::buildSortedPatches(SafeArray<Patch*> &_sortedPatchList)
        {
            _sortedPatchList.reserve(500);

            ITF_LIST<PatchList*>::iterator patchListIter;

            for (patchListIter=m_listOfPatchLists.begin();
                patchListIter!=m_listOfPatchLists.end();
                patchListIter++)
            {

                PatchList *patchList = *patchListIter;
                ITF_LIST<Patch*>::const_iterator patchIter;
                for (patchIter=patchList->m_patches.begin();
                    patchIter!=patchList->m_patches.end();
                    patchIter++)
                {
                    _sortedPatchList.push_back(*patchIter);
                }
            }

            if (_sortedPatchList.size())
                qsort(&_sortedPatchList[0], _sortedPatchList.size(), sizeof(_sortedPatchList[0]), patchComparator);
        }


        ///////////////////////////////////////////////////////////////////////////////////////////
        Patch *PbkMerger::findPatch(const StringID &_templateID)
        {
            ITF_LIST<PatchList*>::iterator patchListIter;

            for (patchListIter=m_listOfPatchLists.begin();
                patchListIter!=m_listOfPatchLists.end();
                patchListIter++)
            {
                PatchList *patchList = *patchListIter;

                ITF_LIST<Patch*>::iterator patchIter;
                for (patchIter=patchList->m_patches.begin(); patchIter!=patchList->m_patches.end(); patchIter++)
                {
                    Patch *patch = *patchIter;
                    if (patch->getTemplateID()==_templateID)
                    {
                        return patch;
                    }
                }
            }
            return NULL;
        }


        ///////////////////////////////////////////////////////////////////////////////////////////
        void PbkMerger::computeFinalMapStartupSize(int &mapWidth, int &mapHeight, int &totalArea)
        {
            totalArea=0;        
            mapWidth = 8;
            mapHeight = 8;

            ITF_LIST<PatchList*>::iterator patchListIter;

            for (patchListIter=m_listOfPatchLists.begin();
                patchListIter!=m_listOfPatchLists.end();
                patchListIter++)
            {

                PatchList *patchList = *patchListIter;
                ITF_LIST<Patch*>::const_iterator patchIter;
                for (patchIter=patchList->m_patches.begin();
                    patchIter!=patchList->m_patches.end();
                    patchIter++)
                {
                    Patch *patch = *patchIter;
                    if (patch->getSourceOfAlias())
                        continue;

                    int w = patch->getSubImageWidth();
                    int h = patch->getSubImageHeight();
                    totalArea += w*h;
                    mapWidth = std::max(mapWidth, w);
                    mapHeight = std::max(mapHeight, h);
                }
            }
            mapWidth = getUpperPowerOf2(mapWidth);
            mapHeight = getUpperPowerOf2(mapHeight);
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        void PbkMerger::createCompositePatchBank(PatchDepot *_patchDepot)
        {
            ITF_ASSERT(m_compositeAnimPatchBank==NULL);
            ITF_LIST<PatchList*>::iterator patchListIter;

            m_compositeAnimPatchBank = newAlloc(mId_Temporary, AnimPatchBank);

            for (patchListIter=m_listOfPatchLists.begin(); patchListIter!=m_listOfPatchLists.end(); patchListIter++)
            {
                PatchList *patchList = *patchListIter;
                AnimPatchBank * patchBank = patchList->m_patchBank;


                for (u32 nameIndex=0; nameIndex<patchBank->m_NameToTemplate.size(); nameIndex++)
                {
                    u32 key = patchBank->m_NameToTemplate.getKeyAt(nameIndex);
                    if (_patchDepot)
                    {
                        bbool isUsed = bfalse;
                        for (ITF_LIST<PatchList>::iterator pIt = _patchDepot->begin(); 
                            pIt != _patchDepot->end(); ++pIt)
                        {
                            PatchList & pList = *pIt;
                            if (std::find(pList.m_usedPatches.begin(), pList.m_usedPatches.end(), StringID(key)) != pList.m_usedPatches.end())
                            {
                                isUsed = btrue;
                                break;
                            }
                        }
                        if (!isUsed)
                            continue;
                    }

                    u32 templateInitialIndex = patchBank->m_NameToTemplate.getAt(nameIndex);
                    if(m_compositeAnimPatchBank->m_NameToTemplate.find(key)>=0)
                    {
#ifdef ITF_DEBUG_STRINGID
                        const String8& debugStr = patchBank->m_NameToTemplate.getDebugAt(nameIndex);
                        ITF_ERROR("2 patch banks contain templates %s\nOne of then will be trashed", debugStr.cStr());
#else
                        ITF_ERROR("2 patch banks have templates with the same name - One of then will be trashed");
#endif
                    }
                    u32 templateMergedIndex=m_compositeAnimPatchBank->m_templateList.size();

#ifdef ITF_DEBUG_STRINGID
                    const String8& debugStr = patchBank->m_NameToTemplate.getDebugAt(nameIndex);
                    m_compositeAnimPatchBank->m_NameToTemplate.setDBG(key, templateMergedIndex, debugStr);
#else
                    m_compositeAnimPatchBank->m_NameToTemplate.set(key, templateMergedIndex);
#endif
                    m_compositeAnimPatchBank->m_templateList.push_back(patchBank->m_templateList[templateInitialIndex]);
                }
            }

        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        void PbkMerger::destroyCompositePatchBank()
        {
            //Actual templates will be freed by the patch depot, so we empty the values before destroying
            //to avoid freeing memory twice
            if (m_compositeAnimPatchBank)
                m_compositeAnimPatchBank->m_templateList.clear();
            SF_DEL(m_compositeAnimPatchBank);
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        void PbkMerger::buildSortedMapSizes(SafeArray<BoxI> &_sortedMapSizes, int mapWidth/*must be pow of 2*/, int mapHeight, int maxUV)
        {
            _sortedMapSizes.resize(0);
            _sortedMapSizes.reserve(16);
            
            for (int v=mapHeight/2; v<=maxUV; v*=2)
                for (int u=mapWidth/2; u<=maxUV; u*=2)
                {
                    BoxI dim(0,0,u,v);
                    _sortedMapSizes.push_back(dim);
                }

            if (_sortedMapSizes.size())
                qsort(&_sortedMapSizes[0], _sortedMapSizes.size(), sizeof(_sortedMapSizes[0]), mapSizeComparator);
        }

    
        ///////////////////////////////////////////////////////////////////////////////////////////
        nv::Image PbkMerger::blitFinalImage(int mapWidth, int mapHeight, const SafeArray<Patch*> &_inputPatchList)
        {
            nv::Image newImage;
            ITF_SET<AnimPatchPoint*> alreadyProcessedPoints;

            newImage.setFormat(nv::Image::Format_ARGB);
            newImage.allocate(mapWidth, mapHeight);
            newImage.fill(nv::Color32(0,0,0,0));

            for (u32 patchIndex=0; patchIndex<_inputPatchList.size(); patchIndex++)
            {
                Patch *patch = _inputPatchList[patchIndex];
                if (!patch->getSourceOfAlias())
                    patch->blitToFinalPos(newImage);
                patch->modifyUVs(mapWidth, mapHeight, alreadyProcessedPoints);
            }
            
            SafeArray<AnimTemplate *> templateList;
            templateList.reserve(_inputPatchList.size());
            for (u32 patchIndex=0; patchIndex<_inputPatchList.size(); patchIndex++)
            {
                Patch *patch = _inputPatchList[patchIndex];
                AnimTemplate *tmplt = patch->getAnimTemplate();
                if (templateList.find(tmplt) >= 0)
                    continue;
                templateList.push_back(tmplt);
                tmplt->ComputeBonesLocalPos(btrue);
                tmplt->m_lenghtFactor *= (f32) mapWidth / (f32) patch->getInitialImageWidth();
            }            

            return newImage;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        bbool PbkMerger::buildPackedMap(const AnimPackerConfiguration &_config)
        {
            SafeArray<Patch*> sortedPatchList;
            SafeArray<BoxI> sortedMapSizes;

            int minMapWidth=0, minMapHeight=0;
            int totalArea=0;
            computeFinalMapStartupSize(minMapWidth, minMapHeight, totalArea);
            buildSortedMapSizes(sortedMapSizes, minMapWidth, minMapHeight, _config.m_maxTextureSize);
            buildSortedPatches(sortedPatchList);

            for (u32 sizeIndex=0; sizeIndex<sortedMapSizes.size(); sizeIndex++)
            {

                BoxI &mapSize = sortedMapSizes[sizeIndex];

#ifdef _ONLY_SQUARE_TEXTURES
                if (mapSize.maxx!=mapSize.maxy)
                    continue;
#endif
                PackedTextureArea area(mapSize.maxx, mapSize.maxy);

                //Place patches (excepted aliases)
                bbool isOk = btrue;
                for (u32 patchIndex=0; patchIndex<sortedPatchList.size(); patchIndex++)
                {
                    Patch *patch = sortedPatchList[patchIndex];
                    if (patch->getSourceOfAlias())
                        continue;
                    BoxI result;
                    bbool isRotated=bfalse;
                    transform2d uvTransform;
                    if (patch->Place(&area, result, isRotated, uvTransform))
                    {
                        //to do
                        patch->setFinalPos(result, isRotated, uvTransform);
                    }
                    else
                    {
                        isOk = bfalse;
                        break;
                    }


                }

                //Then, aliases
                if (isOk)
                {
                    for (u32 patchIndex=0; patchIndex<sortedPatchList.size(); patchIndex++)
                    {
                        Patch *patch = sortedPatchList[patchIndex];
                        if (!patch->getSourceOfAlias())
                            continue;
                        patch->setFinalPosFromAlias();
                    }
                }

                if (isOk)
                {
                    nv::Image finalImage = blitFinalImage(mapSize.maxx, mapSize.maxy, sortedPatchList);
                    bbool isOk=btrue;

                    bbool saved = bfalse;
                    String relativPath = _config.m_mainPath+getCompositeName()+".tga";
                    String imagePath;
                    if(FILEMANAGER->TranslatePath(imagePath, relativPath))
                    {
                        if (SOURCECONTROL_ADAPTER)
                            SOURCECONTROL_ADAPTER->checkout(imagePath);

                        saved = nv::ImageIO::save(String8(imagePath.cStr()).cStr(), &finalImage);

                        // we are not sure that the file is in perforce, so we add it
                        if (SOURCECONTROL_ADAPTER)
                            SOURCECONTROL_ADAPTER->add(imagePath);
                    }

                    if (!saved)
                    {
                        ITF_ERROR("Could not write %S", relativPath.cStr());
                    }
                    isOk=saved;
                    return isOk;
                }
            }

            return bfalse;
        }


    }
}
#endif
