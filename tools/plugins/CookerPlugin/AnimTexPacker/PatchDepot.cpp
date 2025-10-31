#include "core/enginecommon.h"

#ifdef ITF_WINDOWS

#ifndef _ITF_PATCHDEPOT_H_
#include "tools/plugins/CookerPlugin/AnimTexPacker/PatchDepot.h"
#endif

#ifndef _ITF_ANIMATIONPATCHBANK_H_
#include    "engine/animation/AnimationPatchBank.h"
#endif // _ITF_ANIMATIONPATCHBANK_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_ASSERTMANAGER_H_
#include "core/AssertManager.h"
#endif //_ITF_ASSERTMANAGER_H_

#ifndef _ITF_FILEMANAGER_ITF_H_
#include "adapters/FileManager_ITF/FileManager_ITF.h"
#endif //_ITF_FILEMANAGER_ITF_H_

namespace ITF
{
    namespace AnimTexPacker
    {
        ///////////////////////////////////////////////////////////////////////////////////////////
        ///Destructor
        PatchDepot::~PatchDepot()
        {
            for (iterator iter=begin();
                iter!=end();
                iter++)
            {
                iter->destroy();
            }
        }
        ///////////////////////////////////////////////////////////////////////////////////////////
        const PatchList * PatchDepot::getInPatchDepot(const String &_patchImagePath)
        {
            for (const_iterator iter=begin();
                iter!=end();
                iter++)
            {
                String s1 = iter->m_patchImagePath;
                String s2 = _patchImagePath;
                s1.toLower();
                s2.toLower();

                if (s1==s2)
                    return &(*iter);
            }
            return NULL;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        void PatchDepot::processTemplate(PatchList *_patchList, AnimPatchBank *_patchBank, nv::Image &image, const String8 &imagePath, AnimSkeleton *_skeleton, const ITF_VECTOR<StringID> * templateUsed)
        {
            int PatchIndex=0;

            VectorAnim<AnimTemplate*> templateList;
            if (!templateUsed)
            {
				templateList = _patchBank->m_templateList;
            } else
            {
                ITF_VECTOR<StringID>::const_iterator tIt = templateUsed->begin();
                for (; tIt != templateUsed->end(); tIt++)
                {
                    NamesMap::Iterator ntt(_patchBank->m_NameToTemplate);
                    ntt.findKey(*tIt);
                    if (!ntt.isEnd())
                    {
                         templateList.push_back(_patchBank->m_templateList[ntt.second()]);
                    }
                }
            }

            VectorAnim<AnimTemplate*>::const_iterator patchIter;
            for (patchIter=templateList.begin(); 
                patchIter!=templateList.end(); 
                patchIter++, PatchIndex++)          
            {
                AnimTemplate *animtemplate = *patchIter;
                animtemplate->ComputeBonesGlobalPos();

                StringID templateID;
                NamesMap::Iterator nameIter(_patchBank->m_NameToTemplate);
                for (;!nameIter.isEnd(); nameIter.increment())
                {
                    if (_patchBank->m_templateList[nameIter.second()]==animtemplate)
                        templateID = nameIter.first();

                }

                if (std::find(_patchList->m_usedPatches.begin(), _patchList->m_usedPatches.end(), templateID ) == _patchList->m_usedPatches.end())
                    _patchList->m_usedPatches.push_back(templateID);

                //Find linked items from each animTemplate
                ITF_LIST<ChainedBones> _chainedBoneList;

                BuildChainedBoneList(_chainedBoneList, animtemplate);
                //animtemplate->m_Patch[patchIndex].m_AnimPatchPoints->m_mainRefs.m_BonePtr            

                ITF_LIST<ChainedBones>::iterator chainIter;

                for (chainIter=_chainedBoneList.begin(); chainIter!=_chainedBoneList.end(); chainIter++)
                {
                    ChainedBones &chain = *chainIter;


                    Patch *newPatch = newAlloc(mId_Temporary, Patch);

                    newPatch->initialize(templateID, animtemplate, chain, image, imagePath, _skeleton);
                    Patch *sourceOfAlias = NULL;
                    ITF_LIST<Patch*>::const_iterator existingIter;
                    for (existingIter=_patchList->m_patches.begin();
                        existingIter!=_patchList->m_patches.end();
                        existingIter++)
                    {
                        if (newPatch->isEqual(*existingIter))
                        {
                            sourceOfAlias = *existingIter;
                            break;
                        }
                    }
                    if (existingIter!=_patchList->m_patches.end())
                        newPatch->setAliasOf(sourceOfAlias);

                    _patchList->m_patches.push_back(newPatch);

                }
            }
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        bbool PatchDepot::isVisuallyRootBone(AnimBone *_bone)
        {
            if (_bone->m_BoneParent==NULL)
                return btrue;
            if (_bone->m_PatchFlags & BONE_PARENT_CUT)
                return btrue;
            return bfalse;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        bbool PatchDepot::BuildChainedBoneList(ITF_LIST<ChainedBones> &_chainedBoneList, AnimTemplate *_template)
        {
            VectorAnim<AnimBone*> remainingBones;
			remainingBones.reserve(_template->m_BonesCurFrame.size());
			for (u32 i=0;i<_template->m_BonesCurFrame.size(); i++)
				remainingBones.push_back(&_template->m_BonesCurFrame[i]);
			
            //Initialize list with seeds (root bones)
            for (int index=0; index<(int)remainingBones.size(); index++)
            {
                AnimBone *bone = remainingBones[index];
                bbool isRoot = isVisuallyRootBone(bone);
                if (isRoot)
                {
                    ChainedBones seed;
                    seed.m_AnimBones.push_back(bone);
                    remainingBones.erase(remainingBones.begin()+index);
                    index--;
                    _chainedBoneList.push_back(seed);
                }
            }

            //Add other bones
            while(remainingBones.size())
            {
                bbool somethingFound=bfalse;
                for (int boneToAddIndex=0; boneToAddIndex<(int)remainingBones.size(); boneToAddIndex++)
                {
                    AnimBone *boneToAdd = remainingBones[boneToAddIndex];
                    bbool boneWasFound=bfalse;

                    //Find already registered bone in _chainedBoneList
                    ITF_LIST<ChainedBones>::iterator iter;
                    for (iter =_chainedBoneList.begin(); iter!=_chainedBoneList.end(); iter++)
                    {
                        if (iter->m_AnimBones.find(boneToAdd->m_BoneParent)>=0)
                        {
                            somethingFound=btrue;
                            boneWasFound = btrue;
                            iter->m_AnimBones.push_back(boneToAdd);
                        }
                    }
                    if (boneWasFound)
                    {
                        remainingBones.erase(remainingBones.begin()+boneToAddIndex);
                        boneToAddIndex--;
                    }
                }
                if (!somethingFound)
                {
                    ITF_FATAL_ERROR("Inconsistent skeleton found when trying to pack an animation");
                    return bfalse;
                }
            }
            return btrue;
        }
        ///////////////////////////////////////////////////////////////////////////////////////////
        void PatchDepot::addToPatchBankToPatchDepot(
            const String &imagePath,
            AnimPatchBank *patchBank,
            AnimSkeleton *skeleton,
            const ITF_VECTOR<StringID> * templateUsed)
        {
            PatchList * newPatchList = (PatchList *)getInPatchDepot(imagePath);
            if (!newPatchList)
            {
                PatchList tmpNewPatchList;
                this->push_back(tmpNewPatchList);

                newPatchList = &this->back();
                newPatchList->m_patchImagePath = imagePath;
                newPatchList->m_patchBank = patchBank;
            } else if (!templateUsed)
                return;

            LOG("---Loading bank %ls", imagePath.cStr());
            if (patchBank)
            {
                nv::Image image;
                String  fullImagePath;
                String8 imagePath8;
                bbool isImageOk = FILEMANAGER->TranslatePath(fullImagePath, imagePath, 0);
                if (isImageOk)
                {
                    imagePath8 = String8(fullImagePath.cStr());
                    isImageOk  = image.load(imagePath8.cStr());
                }
                                

                if (!isImageOk)
                {
                    LOG("Image %s was not found", imagePath.cStr());
                }
                else
                {
                    processTemplate(newPatchList, patchBank, image, imagePath8, skeleton, templateUsed);
                }
            }
        }


    }
}
#endif
