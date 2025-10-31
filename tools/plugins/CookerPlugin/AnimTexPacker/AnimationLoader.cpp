#include "core/enginecommon.h"

#ifdef ITF_WINDOWS

#ifndef _ITF_ANIMATIONLOADER_H_
#include "tools/plugins/CookerPlugin/AnimTexPacker/AnimationLoader.h"
#endif

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_ASSERTMANAGER_H_
#include "core/AssertManager.h"
#endif //_ITF_ASSERTMANAGER_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_XMLALL_H_
#include "engine/XML/XML_All.h"
#endif //_ITF_XMLALL_H_

#ifndef _ITF_ANIMATIONTRACK_H_
#include "engine/animation/AnimationTrack.h"
#endif //_ITF_ANIMATIONTRACK_H_

#ifndef _ITF_ANIMATIONSKELETON_H_
#include    "engine/animation/AnimationSkeleton.h"
#endif // _ITF_ANIMATIONSKELETON_H_

#ifndef _ITF_ANIMATIONPATCHBANK_H_
#include    "engine/animation/AnimationPatchBank.h"
#endif // _ITF_ANIMATIONPATCHBANK_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_ANIMATIONRES_H_
#include "engine/animation/AnimationRes.h"
#endif //_ITF_ANIMATIONRES_H_

#ifndef _ITF_ANIMATIONLOADPATCHBANK_H_
#include "tools/plugins/CookerPlugin/AnimLoader/AnimationLoadPatchBank.h"
#endif //_ITF_ANIMATIONLOADPATCHBANK_H_

#ifndef _ITF_ANIMATIONLOADTRACK_H_
#include "tools/plugins/CookerPlugin/AnimLoader/AnimationLoadTrack.h"
#endif //_ITF_ANIMATIONLOADTRACK_H_

#ifndef _ITF_FILEMANAGER_ITF_H_
#include "adapters/FileManager_ITF/FileManager_ITF.h"
#endif //_ITF_FILEMANAGER_ITF_H_

#ifndef _ITF_SOURCECONTROLADAPTER_H_
#include "engine/AdaptersInterfaces/SourceControlAdapter.h"
#endif // _ITF_SOURCECONTROLADAPTER_H_

//#define _DONT_RESIZE_SUBTEXTURES

namespace ITF
{
    namespace AnimTexPacker
    {
        ///////////////////////////////////////////////////////////////////////////////////////////
        AnimationLoader::AnimationLoader() : m_skeleton(NULL)
        {
            LOG("-------------AnimTexPacker : CURRENTLY, TEXTURES ARE LOADED TOO");
            m_resourceGroup = (ResourceGroup*)((RESOURCE_MANAGER->newResourceIDFromFile(Resource::ResourceType_ResourceGroup, Path::EmptyPath)).getResource());
            ITF_ASSERT(m_resourceGroup);
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        AnimationLoader::~AnimationLoader()
        {
            unloadAnimations();
            clearSecondaryPatchDepot();
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        void AnimationLoader::mayUpdateBoneCount(u32 _boneCount)
        {
            while(_boneCount>m_boneMaxSizes.size())
            {
                BoneMaxSize s;
                m_boneMaxSizes.push_back(s);
            }
        }

#if !defined(ITF_FINAL) && defined(ITF_WINDOWS) && defined(ITF_DEBUG_STRINGID)
        ///////////////////////////////////////////////////////////////////////////////////////////
        void AnimationLoader::exportAnimations(const String8 &_directory)
        {
            for (u32 index=0; index<m_loadedAnimations.size(); index++)
            {
                AnimTrack *track = m_loadedAnimations[index];
                if (!track)
                    continue;
                String8 filePath = _directory+String8(FilePath::getFilename(m_loadedAnimationPathes[index]).cStr());
                AnimationLoadTrack::writeAnimTrack(track, filePath.cStr());
            }
        }
#endif
        ///////////////////////////////////////////////////////////////////////////////////////////
        void AnimationLoader::cookAnimations(const String8 &_directory, const String & _platform)
        {
            for (u32 index=0; index<m_loadedAnimations.size(); index++)
            {
                AnimTrack *track = m_loadedAnimations[index];
                if (!track)
                    continue;
                String8 filePath = _directory+String8(FilePath::getFilename(m_loadedAnimationPathes[index]).cStr());
                String cookFile = FILESERVER->getCookedNamePlatform(filePath,_platform);
                
                track->saveBinary(cookFile);
            }
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        void AnimationLoader::modifyAnimations(const AnimPackerConfiguration &_config)
        {
            for (u32 index=0; index<m_loadedAnimations.size(); index++)
            {
                AnimTrack *track = m_loadedAnimations[index];
                if (!track)
                    continue;

                ITF_SET<String8> patchBanks;

                //patch BML (Bitmap match list)
                for (u32 bmlIndex=0; bmlIndex<track->m_bmlList.size(); bmlIndex++)
                {
                    AnimTrackBML &bml = track->m_bmlList[bmlIndex];
                    for (u32 templateIndex=0; templateIndex<bml.m_templateRefs.size(); templateIndex++)
                    {
                        TemplateRef &templte = bml.m_templateRefs[templateIndex];
                        PbkMerger *merger = m_mergerList.getPbkMergerFromTemplateID(templte.m_patchName);
                        if (merger)
                        {
                            const String8 &patchBankName = merger->getCompositeName();
                            patchBanks.insert(patchBankName);
                            templte.m_bankName = StringID(patchBankName);
                        }
                        else
                        {
                            templte.m_bankName = StringID::Invalid;
                        }
                    }
                }
                //modify patchbankList
                track->m_nameToPatch.clear();
                track->m_patchBankPathList.clear();
                for (ITF_SET<String8>::const_iterator pbkNameIter=patchBanks.begin();
                    pbkNameIter!=patchBanks.end();
                    pbkNameIter++)
                {
                    const String8 &name=*pbkNameIter;

                    i32 patchIndex=(i32)track->m_patchBankPathList.size();
                    track->m_nameToPatch.setID(name, patchIndex);
                    track->m_patchBankPathList.push_back(String8(_config.m_mainPath.cStr())+name);
                }
            }

        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        StringID AnimationLoader::findSeedBone(const StringID &_boneID)
        {
            for (PatchDepot::iterator iter=m_patchDepot.begin();
                iter!=m_patchDepot.end();
                iter++)
            {
                //For each patch
                PatchList &patchList = *iter;
                for (ITF_LIST<Patch*>::iterator patchIter = patchList.m_patches.begin();
                    patchIter!=patchList.m_patches.end();
                    patchIter++)
                {
                    Patch *patch = *patchIter;
                    //find ID in bone IDs
                    if (std::find(patch->getBoneIDs().begin(), patch->getBoneIDs().end(), _boneID)!=patch->getBoneIDs().end())
                    {
                        return patch->getBoneIDs()[0];
                    }
                }
            }
            ITF_ASSERT_MSG(bfalse, "could not find bone");
            return _boneID;
        }

        struct PatchBankTemplate {
            AnimPatchBank            *m_bank;
            ITF_VECTOR<StringID> m_templateUsed;
        };

        ///////////////////////////////////////////////////////////////////////////////////////////
        AnimTrack * AnimationLoader::parseAnimation(ResourceID _trackRes)
        {
            AnimMeshScene animMeshScene;

            AnimInfo &animInfo = animMeshScene.m_AnimInfo;
            AnimTrackExt animTrackExt;

            animTrackExt.m_trackResourceID = _trackRes;
            animInfo.m_Pos   = Vec3d::Zero;
            animInfo.m_Scale = Vec2d::One;

            ITF_VECTOR<AnimFrameInfo> frameInfoList;
            AnimFrameInfo animFrame;
            animFrame.m_fFrame = 0;
            animFrame.m_pTrackExt = &animTrackExt;
            animFrame.m_percent = 1.f;
            animFrame.m_useVisuals = btrue;

            frameInfoList.push_back(animFrame);
            animInfo.m_frameInfo         = frameInfoList;
            animInfo.m_useRootRotation   = bfalse;

            AnimTrack *track = animTrackExt.m_trackResourceID.getResource() ? 
                ((AnimTrackResource *) animTrackExt.m_trackResourceID.getResource())->getTrack() :  NULL; 
            if (!track)
                return NULL;
            AnimSkeleton *skeletonPtr    = track->getSkeleton();
            u32 boneCount = skeletonPtr->m_BonesCurFrame.size();
            m_skeleton = skeletonPtr;
            mayUpdateBoneCount(boneCount);

            //Iterate for each patch bank (actually to each patch bank image)
            ITF_VECTOR<PatchBankTemplate> trackPatches;
            
            VectorAnim<AnimTrackBML>::iterator bmlIter = track->m_bmlList.begin();
            for (; bmlIter != track->m_bmlList.end(); ++bmlIter)
            {
                AnimTrackBML & bml = *bmlIter;
                VectorAnim<TemplateRef>::iterator tempRefIter = bml.m_templateRefs.begin();
                for (; tempRefIter != bml.m_templateRefs.end(); ++tempRefIter)
                {
                    TemplateRef & templateRef = *tempRefIter;
                    NamesMap::Iterator ntp(track->m_nameToPatch);
                    ntp.findKey(templateRef.m_bankName);
                    if (!ntp.isEnd())
                    {
                        AnimPatchBank *bank = track->getPatchBank(ntp.second());
                        if (bank)
                        {
                            NamesMap::Iterator ntt(bank->m_NameToTemplate);
                            ntt.findKey(templateRef.m_patchName);
                            if (!ntt.isEnd())
                            {
                                AnimTemplate * animTemplate = bank->m_templateList[ntt.second()];
                                if (animTemplate)
                                {
                                    PatchBankTemplate *trackPatche = NULL;
                                    for (ITF_VECTOR<PatchBankTemplate>::iterator it = trackPatches.begin();
                                        it != trackPatches.end(); it++)
                                    {
                                        if (it->m_bank == bank)
                                        {
                                            trackPatche = &(*it);
                                            break;
                                        }
                                    }
                                    if (!trackPatche)
                                    {
                                        trackPatches.resize(trackPatches.size()+1);
                                        trackPatche = &trackPatches[trackPatches.size()-1];
                                        trackPatche->m_bank = bank;
                                    }
                                    bbool templateAlreadyUsed = bfalse;
                                    for (ITF_VECTOR<StringID>::iterator itt = trackPatche->m_templateUsed.begin();
                                        itt != trackPatche->m_templateUsed.end(); itt++)
                                    {
                                        if (*itt == templateRef.m_patchName)
                                        {
                                            templateAlreadyUsed = btrue;
                                            break;
                                        }
                                    }
                                    if (!templateAlreadyUsed)
                                        trackPatche->m_templateUsed.push_back(templateRef.m_patchName);
                                }
                            }
                        }
                    }
                }
            }

            PathString_t pathName;

            for (ITF_VECTOR<PatchBankTemplate>::iterator it = trackPatches.begin();
                it != trackPatches.end(); it++)
            {
                PatchBankTemplate & trackPatche  =(*it);
                Texture * texture = (Texture *)trackPatche.m_bank->m_texture.getResource();
                    
                ITF_ASSERT(texture);
                if (texture)
                {
                    texture->getPath().getString(pathName);
                    m_patchDepot.addToPatchBankToPatchDepot(pathName, trackPatche.m_bank, skeletonPtr, &trackPatche.m_templateUsed);
                }
            }
            //List important frames for given animation
            const FastArray<AnimTrackBonePAS> &boneList = animTrackExt.getTrack()->m_trackListPAS;
            ITF_SET<f32> frames;

            for (u32 trackIndex=0; trackIndex<boneList.size(); trackIndex++)
            {
                const AnimTrackBonePAS &animBone = boneList[trackIndex];
                frames.insert((f32)animBone.m_frame);
            }
            //For each important frame
            for (ITF_SET<f32>::const_iterator iter=frames.begin(); iter!=frames.end(); iter++)
            {
                //Play anim at this frame
                AnimFrameInfo &animFrame = animInfo.m_frameInfo[0];
                animFrame.m_fFrame = *iter;
                animInfo.SetFrame();
                animMeshScene.m_pAnimPatches.processPatchs(&animMeshScene);

                //Initialize, for each bone of the skeleton, the boneExtents[boneIndex]
                ITF_VECTOR<BoneExtent> boneExtents;
                boneExtents.reserve(boneCount);
                for (u32 i=0; i<boneCount; i++)
                {
                    BoneExtent e;
                    e.initEmpty();
                    boneExtents.push_back(e);
                }

                //for each AnimPatch of anim scene, update size of related bone
                for (u32 patchIndex=0; patchIndex<animMeshScene.m_pAnimPatches.m_allPatchs.size(); patchIndex++)
                {
                    AnimPatch* patch = animMeshScene.m_pAnimPatches.m_allPatchs[patchIndex].m_patch;
                    AnimPatchPointDyn *patchPointDynTable = animInfo.getTemplatePatchPointBuffer(patch->m_PatchBankPtr->m_templateList[patch->m_TemplateIdx], btrue);
                    //Get 
                    for (u32 pointIndex=0; pointIndex<patch->m_nbPatchPoints; pointIndex++)
                    {
                        AnimPatchPoint *point = patch->m_AnimPatchPoints[pointIndex];
                        AnimPatchPointDyn *pointDyn = &patchPointDynTable[point->m_Index];


                        AnimBone *bone = point->m_mainRefs.m_BonePtr;
                        StringID boneName = findSeedBone(bone->m_Name);
                        u32 boneIndex = skeletonPtr->getBoneIndex(boneName);
                        //LOG("--Bone %p Index=%d", point->m_mainRefs.m_BonePtr, boneIndex);
                        ITF_ASSERT(boneIndex<boneCount);
                        boneExtents[boneIndex].grow(pointDyn->m_Pos, point->m_PosUV);

                    }
                }

                //and compute m_boneMaxSizes[bone index]
                for (u32 i=0; i<boneCount; i++)
                {
                    BoneExtent &b=boneExtents[i];
                    if (b.m_worldExtent.isValid() && b.m_uvExtent.isValid())
                    {
                        m_boneMaxSizes[i].grow(
                            b.m_worldExtent.getMax()-b.m_worldExtent.getMin(),
                            b.m_uvExtent.getMax()-b.m_uvExtent.getMin());

                    }
                }

            }
            //
            return track;
        }


        ///////////////////////////////////////////////////////////////////////////////////////////
        void AnimationLoader::unloadAnimations()
        {            
            if (m_resourceGroup)
                RESOURCE_MANAGER->removeUserToResourcePhysicalData(m_resourceGroup);
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        ResourceGroup *AnimationLoader::loadAnimations(const ITF_VECTOR<String> &animPathList)
        {
            if (m_resourceGroup)
            {
                ITF_LIST<ResourceID> anmResourceList;
                for (ITF_VECTOR<String>::const_iterator iter=animPathList.begin();
                    iter!=animPathList.end();
                    iter++)
                {
                    const String &animPath=*iter;
                    ResourceID trkRes = m_resourceGroup->addResource(Resource::ResourceType_AnimTrack, animPath);
                    AnimTrackResource *res = (AnimTrackResource*)trkRes.getResource();
                    if (res)
                    {
                        res->setCanLoadBin(bfalse);
                    }

                    anmResourceList.push_back(trkRes);
                }

                m_resourceGroup->addLogicalDataUser();
                
                RESOURCE_MANAGER->queryAddUserToResourcePhysicalData ( m_resourceGroup->getID() );
                do {
                    RESOURCE_MANAGER->flushPendingOps();
                    RESOURCE_MANAGER->loadResourcesPending();
                    RESOURCE_MANAGER->synchronizeResourcesAsLoaded();
                    RESOURCE_MANAGER->synchronizeResourcesToProcessFromMainThread();
                } while (RESOURCE_MANAGER->hasQueryOps());
                int rscIndex=0;
                PathString_t pathName;

                for (ITF_LIST<ResourceID>::const_iterator iter=anmResourceList.begin();
                    iter!=anmResourceList.end();
                    iter++, rscIndex++)
                {
                    ResourceID trkRes = *iter;
                    AnimTrackResource *res = (AnimTrackResource*)trkRes.getResource();
                    if (res!=NULL)
                    {
                        AnimTrack *track = parseAnimation(trkRes);
                        if (track)
                        {
                            res->getPath().getString(pathName);

                            m_loadedAnimationPathes.push_back(pathName);
                            m_loadedAnimations.push_back(track);
                        }
                    }
                    else
                    {
                        ITF_ERROR("Impossible to load animation : %ls", animPathList[rscIndex].cStr());
                    }
                }

            }
            //Match bone sizes with patches
            for (PatchDepot::iterator iter=m_patchDepot.begin();
                iter!=m_patchDepot.end();
                iter++)
            {
                //For each patch
                PatchList &patchList = *iter;
                for (ITF_LIST<Patch*>::iterator patchIter = patchList.m_patches.begin();
                    patchIter!=patchList.m_patches.end();
                    patchIter++)
                {
                    Patch *patch = *patchIter;
                    //Iterate each of its bones
                    for (u32 patchBoneIndex=0; patchBoneIndex<patch->getBoneIndices().size(); patchBoneIndex++)
                    {
                        u32 patchBone = patch->getBoneIndices()[patchBoneIndex];
                        //And grow the patch with the bone size
                        if (patchBone<m_boneMaxSizes.size())
                        {
                            Patch *patchToGrow = patch->getSourceOfAlias();
                            if (patchToGrow==NULL)
                                patchToGrow = patch;
                            patchToGrow->growPatchMaxSize(m_boneMaxSizes[patchBone]);
                        }
                    }
                }
            }

            return m_resourceGroup;
        }

        void  AnimationLoader::fillSecondaryPatchDepot(int & serialMergerIndex, const AnimPackerConfiguration &_config, const String &_platform)
        {
            ITF_VECTOR<String>      formatList;
            ITF_VECTOR<const PatchList*>  mainPatchList;
            // We need a skeleton, we take the first - and only one
            AnimSkeleton           *skeleton = m_loadedAnimations.size() > 0 ? m_loadedAnimations[0]->getSkeleton() : NULL;

            if (!skeleton)
                return;

            // search for file in depot with index value
            for (PatchDepot::const_iterator it = m_patchDepot.begin();
                it != m_patchDepot.end(); ++it)
            {
                const PatchList &patchList = *it;
                String imgPath  = FilePath::getDirectory(patchList.m_patchImagePath);
                String imgTitle = FilePath::getFilenameWithoutExtension(patchList.m_patchImagePath);
                String imgExt   = FilePath::getFilename(patchList.m_patchImagePath);
                       imgExt   = imgExt.substr(imgTitle.getLen(), imgExt.getLen() - imgTitle.getLen());
                // take only images like xxx1.png / xxx1.tga
                if (imgTitle.substr(imgTitle.getLen()-1, 1) != "1")
                    continue;

                String strFormat;
                imgTitle = imgTitle.substr(0, imgTitle.getLen()-1);
                strFormat.setTextFormat("%S%S%%i%S", imgPath.cStr(), imgTitle.cStr(), imgExt.cStr());
                if (std::find(formatList.begin(), formatList.end(), strFormat) == formatList.end())
                {
                    formatList.push_back(strFormat);
                    mainPatchList.push_back(&patchList);
                }
            }

            u32   currentIndex  = 1;
            bbool somethingDone = bfalse;
            String patchBankImage;
            do 
            {
                // create secondary depot
                PatchDepot * patchDepot = newAlloc(mId_Resource, PatchDepot);

                currentIndex++;
                somethingDone = bfalse;

                ITF_VECTOR<String>::iterator formatIter = formatList.begin();
                ITF_VECTOR<const PatchList*>::const_iterator mainPatchIter = mainPatchList.begin();
                for (; formatIter != formatList.end(); ++formatIter, ++mainPatchIter)
                {
                    patchBankImage.setTextFormat(StringConverter(*formatIter).getChar(), currentIndex);
                    String patchBankFile = FilePath::getDirectory(patchBankImage) + FilePath::getFilenameWithoutExtension(patchBankImage) + ".pbk";

                    AnimPatchBank * patchBank = newAlloc(mId_Resource, AnimPatchBank);
                    if (FILEMANAGER->fileExists(patchBankFile) &&
                         patchBank->loadFile(patchBankFile, _platform, bfalse, bfalse))
                    {
                        patchDepot->addToPatchBankToPatchDepot(patchBankImage, patchBank, skeleton, &(*mainPatchIter)->m_usedPatches);
                        somethingDone = btrue;                            
                    } else
                    {
                        SF_DEL(patchBank);
                    }
                }
                if (somethingDone)
                {
                    PbkMergerList::buildPbkMergerList(serialMergerIndex, _config, m_mergerList, *patchDepot);
                    m_secondaryPatchDepot.push_back(patchDepot);
                } else
                {
                    SF_DEL(patchDepot);
                }
            } while (somethingDone);
        }

        void  AnimationLoader::clearSecondaryPatchDepot()
        {
            for (u32 i=0; i<m_secondaryPatchDepot.size(); i++)
            {
                PatchDepot * pDepot = m_secondaryPatchDepot[i];
                for (PatchDepot::iterator iter=pDepot->begin();
                     iter != pDepot->end(); iter++)
                {
                    SF_DEL((*iter).m_patchBank);
                }
                SF_DEL(pDepot);
            }
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        bbool AnimationLoader::buildPackedMap(const AnimPackerConfiguration &_config, const String &_platform)
        {
            if (!m_skeleton)
                return bfalse;

            int serialMergerIndex = 0;
            PbkMergerList::buildPbkMergerList(serialMergerIndex, _config, m_mergerList, m_patchDepot);

            if (_config.m_processBankChange)
                fillSecondaryPatchDepot(serialMergerIndex, _config, _platform);

            //compute sizes, using animations
//            for (PatchDepot::const_iterator iter=m_patchDepot.begin();
//                iter!=m_patchDepot.end();
//                iter++)
//            {
//
//                float interestPatchSize=0;
//                Patch *interestPatch=NULL;
//                i32  importantBoneSize=0;
//
//                ITF_LIST<Patch*>::const_iterator patchIter;
//                for (u32 importantBoneIndex=0; importantBoneIndex<_config.m_importantBones.size(); importantBoneIndex++)
//                {
//                    const AnimPackerConfiguration::ImportantBone &importantBone=_config.m_importantBones[importantBoneIndex];
//
//                    for (patchIter=iter->m_patches.begin(); patchIter!=iter->m_patches.end(); patchIter++)
//                    {
//                        Patch *patch = (*patchIter);
//
//                        if (patch->getSourceOfAlias())
//                            continue;
//
//                        const BoneMaxSize &boneMaxSize = patch->getPatchMaxSize();
//                        float size = boneMaxSize.getMaxWorldSize();
//
//                        //search for the patch bones, check if it contains given bone
//                        bbool isFound=bfalse;
//                        for (u32 boneIndex=0; boneIndex<patch->getBoneIDs().size(); boneIndex++)
//                        {
//                            if (patch->getBoneIDs()[boneIndex]==StringID(importantBone.m_boneName.cStr()))
//                            {
//                                isFound=btrue;
//                                interestPatchSize = size;
//                                interestPatch = patch;
//                                importantBoneSize = importantBone.m_sizeInPixel;
//                                break;
//                            }
//                        }
//                        if (isFound)
//                        {
//                            break;
//                        }
//                    }
//
//                    if (interestPatch!=NULL)
//                        break;
//                }
//
//                if (!interestPatch)
//                {
//                    ITF_ERROR("Could not find any of the bones which were declared in anim packer configuration for %ls", iter->m_patchImagePath.cStr());
//                    continue;
//                }
//
//                //compute meters per texel density
//                ITF_ASSERT(importantBoneSize>0);
//                f32 density = interestPatchSize/(f32)importantBoneSize;
//
//                //
//                for (patchIter=iter->m_patches.begin(); patchIter!=iter->m_patches.end(); patchIter++)
//                {
//                    Patch *patch = (*patchIter);
//                    if (patch->getSourceOfAlias())
//                        continue;
//                    const BoneMaxSize &boneMaxSize = patch->getPatchMaxSize();
//                    float size = boneMaxSize.getMaxWorldSize();
//                    int textureSize = 0;
//                    if (density!=0)
//                        textureSize = (int)(size/density);
//                    if (_config.m_maxElementSize>0)
//                        textureSize = std::min(textureSize, _config.m_maxElementSize);
//                    textureSize = std::max(textureSize, 4);
//                    int currentSize = std::max(patch->getSubImageWidth(), patch->getSubImageHeight());
//
//#ifndef _DONT_RESIZE_SUBTEXTURES
//                    if (textureSize<currentSize)
//                        patch->resizeTexture(textureSize);
//#endif
//                }               
//            }
//
//
            String fullPath;
            for (PbkMergerList::iterator iter=m_mergerList.begin();
                iter!=m_mergerList.end();
                iter++)
            {
                PbkMerger &merger = *iter;

                bbool isOk = merger.buildPackedMap(_config);
                if (!isOk)
                {
                    ITF_ERROR("Could not build packed map");
                    return bfalse;
                }

                merger.createCompositePatchBank(&m_patchDepot);

#if !defined(ITF_FINAL) && defined(ITF_WINDOWS) && defined(ITF_DEBUG_STRINGID)
                String8 imagePath(String8(_config.m_mainPath.cStr())+iter->getCompositeName()+".pbk");

                if (FILEMANAGER->TranslatePath(fullPath, imagePath, 0) && SOURCECONTROL_ADAPTER)
                    SOURCECONTROL_ADAPTER->checkout(fullPath);

                AnimationLoadPatchBank::writeAnimPatchBank(iter->m_compositeAnimPatchBank, m_skeleton, imagePath.cStr());                

                if (SOURCECONTROL_ADAPTER)
                    SOURCECONTROL_ADAPTER->add(fullPath);
#endif
            }

            ITF::PbkRedirect redirect;
            m_mergerList.fillPbkRedirect(_config, redirect);

            if (FILEMANAGER->TranslatePath(fullPath, _config.m_mainPath + AnimTrack::s_redirectPatchBank, 0) && SOURCECONTROL_ADAPTER)
                SOURCECONTROL_ADAPTER->checkout(fullPath);

            redirect.saveBinary(_config.m_mainPath + AnimTrack::s_redirectPatchBank);

            if (SOURCECONTROL_ADAPTER)
                SOURCECONTROL_ADAPTER->add(fullPath);

            return btrue;
        }

        struct GlobalPositionning
        {
            PatchList * m_patchList;
            nv::Image   m_image;
            int         m_posX;
            int         m_posY;
        };

        bbool AnimationLoader::buildBasicPack(const AnimPackerConfiguration &_config, const String &_platform)
        {
            if (!m_skeleton)
                return bfalse;

            int serialMergerIndex = 0;
            PbkMergerList::buildPbkMergerList(serialMergerIndex, _config, m_mergerList, m_patchDepot);

            if (_config.m_processBankChange)
                fillSecondaryPatchDepot(serialMergerIndex, _config, _platform);

            for (PbkMergerList::iterator mergeIt = m_mergerList.begin(); 
                mergeIt != m_mergerList.end(); ++mergeIt)
            {
                PbkMerger & merger = *mergeIt;
                ITF_VECTOR<GlobalPositionning> globalPositionningList;
                u32 surface   = 0;
                u32 minX      = 16384;
                u32 minY      = 16384;
                u32 maxX      = 0;
                u32 maxY      = 0;
                for (ITF_LIST<PatchList*>::iterator patchList = merger.m_listOfPatchLists.begin();
                    patchList != merger.m_listOfPatchLists.end(); ++patchList)
                {
                    PatchList* ptList = *patchList;
                    GlobalPositionning gp;
                    String  imagePath = ptList->m_patchImagePath;
                    String  fullImagePath;
                    String8 imagePath8;
                    bbool   isImageOk = FILEMANAGER->TranslatePath(fullImagePath, imagePath, 0);
                    if (isImageOk)
                    {
                        imagePath8 = String8(fullImagePath.cStr());
                        isImageOk  = gp.m_image.load(imagePath8.cStr());
                    }
                    if (isImageOk)
                    {
                        gp.m_patchList   = ptList;
                        gp.m_posX        = -1;
                        gp.m_posY        = -1;
                        u32 localSurface = gp.m_image.width()* gp.m_image.height();
                        surface         += localSurface;

                        if (minX > gp.m_image.width())
                            minX = gp.m_image.width();
                        if (minY > gp.m_image.height())
                            minY = gp.m_image.height();
                        if (maxX < gp.m_image.width())
                            maxX = gp.m_image.width();
                        if (maxY < gp.m_image.height())
                            maxY = gp.m_image.height();
                        globalPositionningList.push_back(gp);
                    }
                }
                
                for (u32 i=0; i<globalPositionningList.size(); i++)
                {
                    for (u32 j=i+1; j<globalPositionningList.size(); j++)
                    {
                        if (globalPositionningList[i].m_image.width()*globalPositionningList[i].m_image.height() < 
                            globalPositionningList[j].m_image.width()*globalPositionningList[j].m_image.height())
                        {
                            GlobalPositionning swap = globalPositionningList[i];
                            globalPositionningList[i] = globalPositionningList[j];
                            globalPositionningList[j] = swap;
                        }
                    }
                }
                
                bbool allMatches;
                do
                {
                    allMatches = btrue;
                    u32 currentSurface = maxX*maxY;
                    while (currentSurface < surface)
                    {
                        if (maxX <= maxY)   maxX *= 2;
                        else                maxY *= 2;
                        currentSurface = maxX*maxY;
                    }

                    SafeArray<bbool> cellsTab;
                    u32 xCells  = maxX / minX;
                    u32 yCells  = maxY / minY;
                    u32 nbCells = xCells*yCells;
                    cellsTab.resize(nbCells);
                    for (u32 i=0; i<nbCells; i++)
                        cellsTab[i] = bfalse;

                    for (u32 i=0; i<globalPositionningList.size();i++)
                    {
                        bbool found     = bfalse;
                        u32   cellXLen   = globalPositionningList[i].m_image.width()  / minX;
                        u32   cellYLen   = globalPositionningList[i].m_image.height() / minY;
                        for (u32 y=0; !found && y<yCells; y++)
                        {
                            for (u32 x=0; !found && x<xCells; x++)
                            {
                                bbool isEmpty = btrue;
                                for (u32 cy=0; isEmpty && cy<cellYLen; cy++)
                                {
                                    for (u32 cx=0; isEmpty && cx<cellXLen; cx++ )
                                    {
                                        if ((x+cx) >= xCells || (y+cy) >= yCells)
                                        {
                                            isEmpty = bfalse;
                                        } else
                                            isEmpty = !cellsTab[(x+cx)+(y+cy)*xCells];
                                    }
                                }
                                if (isEmpty)
                                {
                                    for (u32 cy=0; isEmpty && cy<cellYLen; cy++)
                                    {
                                        for (u32 cx=0; isEmpty && cx<cellXLen; cx++ )
                                        {
                                            cellsTab[(x+cx)+(y+cy)*xCells] = btrue;
                                        }
                                    }
                                    found = btrue;
                                    globalPositionningList[i].m_posX = x*minX;
                                    globalPositionningList[i].m_posY = y*minY;
                                }
                            }
                        }
                        if (!found)
                            allMatches = bfalse;
                    }
                    if (!allMatches)
                        surface *= 2;
                } while (!allMatches);
                

                nv::Image finalImage;
                finalImage.setFormat(nv::Image::Format_ARGB);
                finalImage.allocate(maxX, maxY);

                BoxI        finalBox;
                bbool       isRotated = bfalse;
                transform2d uvTransform;
                uvTransform.setIdentity();

                for (u32 i=0; i<globalPositionningList.size(); i++)
                {
                    GlobalPositionning & gp = globalPositionningList[i];

                    for (u32 y=0; y<gp.m_image.height(); y++)
                    {
                        for (u32 x=0; x<gp.m_image.width(); x++)
                        {
                            finalImage.pixel(x+gp.m_posX, y+gp.m_posY) = gp.m_image.pixel(x, y);
                        }
                    }


                    ITF_SET<AnimPatchPoint*> alreadyProcessedPoints;

                    SafeArray<AnimTemplate *> templateList;
                    templateList.clear();
                    templateList.reserve(gp.m_patchList->m_patches.size());

                    for (ITF_LIST<Patch*>::iterator patchIt = gp.m_patchList->m_patches.begin();
                        patchIt != gp.m_patchList->m_patches.end(); ++patchIt)
                    {
                        Patch * patch = *patchIt;
                        AnimTemplate *tmplt = patch->getAnimTemplate();
                        if (templateList.find(tmplt) >= 0)
                            continue;
                        templateList.push_back(tmplt);
                        tmplt->ComputeBonesGlobalPos();
                    }

                    SafeArray<AnimBoneDyn *> seedsList;
                    seedsList.clear();
                    seedsList.reserve(gp.m_patchList->m_patches.size());
                    for (ITF_LIST<Patch*>::iterator patchIt = gp.m_patchList->m_patches.begin();
                        patchIt != gp.m_patchList->m_patches.end(); ++patchIt)
                    {
                        Patch * patch = *patchIt;
                        AnimTemplate *tmplt = patch->getAnimTemplate();

                        i32         boneIDIndex = tmplt->m_NameToBone.find(patch->getBoneIDs()[0]);
                        i32         boneIndex   = tmplt->m_NameToBone.getAt(boneIDIndex);
                        AnimBoneDyn *seedBone   = &tmplt->m_BonesCurFrameDyn[boneIndex];
                        
                        if (seedsList.find(seedBone) >= 0)
                            continue;
                        seedsList.push_back(seedBone);

                        finalBox = patch->getUVBox();
                        finalBox.minx += gp.m_posX;
                        finalBox.maxx += gp.m_posX;
                        finalBox.miny += gp.m_posY;
                        finalBox.maxy += gp.m_posY;

                        patch->setFinalPos(finalBox, isRotated, uvTransform);
                        patch->modifyUVs(maxX, maxY, alreadyProcessedPoints);
                    }

                    templateList.clear();
                    templateList.reserve(gp.m_patchList->m_patches.size());
                    for (ITF_LIST<Patch*>::iterator patchIt = gp.m_patchList->m_patches.begin();
                        patchIt != gp.m_patchList->m_patches.end(); ++patchIt)
                    {
                        Patch * patch = *patchIt;
                        AnimTemplate *tmplt = patch->getAnimTemplate();
                        if (templateList.find(tmplt) >= 0)
                            continue;
                        templateList.push_back(tmplt);
                        tmplt->ComputeBonesLocalPos(btrue);
                        tmplt->m_lenghtFactor *= (f32) maxX / (f32) patch->getInitialImageWidth();
                    }       
                }

                String relativPath = _config.m_mainPath+ merger.getCompositeName()+".tga";
                String fullPath;
                if(FILEMANAGER->TranslatePath(fullPath, relativPath))
                {
                    if (SOURCECONTROL_ADAPTER)
                        SOURCECONTROL_ADAPTER->checkout(fullPath);

                    nv::ImageIO::save(String8(fullPath.cStr()).cStr(), &finalImage);

                    // we are not sure that the file is in perforce, so we add it
                    if (SOURCECONTROL_ADAPTER)
                        SOURCECONTROL_ADAPTER->add(fullPath);
                }

                merger.createCompositePatchBank(&m_patchDepot);
                String8 pbkPath(String8(_config.m_mainPath.cStr())+merger.getCompositeName()+".pbk");

                if (FILEMANAGER->TranslatePath(fullPath, pbkPath, 0) && SOURCECONTROL_ADAPTER)
                    SOURCECONTROL_ADAPTER->checkout(fullPath);
#if !defined(ITF_FINAL) && defined(ITF_DEBUG_STRINGID) && defined(ITF_WINDOWS)
                AnimationLoadPatchBank::writeAnimPatchBank(merger.m_compositeAnimPatchBank, m_skeleton, pbkPath.cStr());                
#endif //!defined(ITF_FINAL) && defined(ITF_DEBUG_STRINGID) && defined(ITF_WINDOWS)

                if (SOURCECONTROL_ADAPTER)
                    SOURCECONTROL_ADAPTER->add(fullPath);
            }

            ITF::PbkRedirect redirect;
            m_mergerList.fillPbkRedirect(_config, redirect);

            String fullPath;

            if (FILEMANAGER->TranslatePath(fullPath, _config.m_mainPath + AnimTrack::s_redirectPatchBank, 0) && SOURCECONTROL_ADAPTER)
                SOURCECONTROL_ADAPTER->checkout(fullPath);

            redirect.saveBinary(_config.m_mainPath + AnimTrack::s_redirectPatchBank);

            if (SOURCECONTROL_ADAPTER)
                SOURCECONTROL_ADAPTER->add(fullPath);

            return bfalse;
        }
    }
}

#endif
