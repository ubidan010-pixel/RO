#include "core/enginecommon.h"

#ifndef _ITF_ANIMATIONLOADPATCHBANK_H_
#include "tools/plugins/CookerPlugin/AnimLoader/AnimationLoadPatchBank.h"
#endif //_ITF_ANIMATIONLOADPATCHBANK_H_

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_ANIMATIONPATCHBANK_H_
#include    "engine/animation/AnimationPatchBank.h"
#endif // _ITF_ANIMATIONPATCHBANK_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif // _ITF_FILESERVER_H_

#ifndef _ITF_FILEMANAGER_ITF_H_
#include "adapters/FileManager_ITF/FileManager_ITF.h"
#endif //_ITF_FILEMANAGER_ITF_H_

namespace ITF
{
    AnimPatchBank * AnimationLoadPatchBank::readAnimPatchBank(  const Path& _XMLFile_Path,const String& _platform, bbool _raiseError, bbool _bSaveBin)
    {
        PugiXMLWrap::ErrorBehavior errBehavior = _raiseError ? PugiXMLWrap::RaiseError : PugiXMLWrap::DoNothing;
        PathString_t _szXMLFile;
        
        _XMLFile_Path.getString(_szXMLFile);

        PugiXMLWrap XML;

        String          cookFile        = FILESERVER->getCookedNamePlatform(_szXMLFile,_platform);
        AnimPatchBank * animPatchBank   = newAlloc(mId_Animation,AnimPatchBank());

        if (!XML.openFile(_szXMLFile,bfalse, errBehavior))
        {
            SF_DEL(animPatchBank);
            return NULL;
        }

        PtrToStrID  ptrToStrID;
        NamesMap    boneIDToIndex;

        Map<String, i32> uidToTemplate; //Converts template names to indices (used at loading time)
        animPatchBank->m_Ratio       = XML.readFloatAttribute("Ratio/value", 1, errBehavior);

		if (XML.enterElement("List"))
		{
			if (XML.enterFirstElement())
				do 
				{
					String Id = XML.readElementName();
					if (Id == "Element")
						Id = XML.readAttribute("index", "", errBehavior);

					AnimTemplate *scn = newAlloc(mId_Animation,AnimTemplate());
					scn->m_lenghtFactor = XML.readFloatAttribute("LenghtFactor/value", 1.f);
					animPatchBank->m_NameToTemplate.setID(Id, animPatchBank->m_templateList.size());

					xmlReadTemplateBoneListT(&XML, scn, &ptrToStrID, &boneIDToIndex, errBehavior);
					xmlReadAnimPatchPoints(&XML, scn, &ptrToStrID, &boneIDToIndex, errBehavior);

					animPatchBank->m_templateList.push_back(scn);
				} while (XML.nextElement());
				XML.exitElement();
		}

        animPatchBank->ComputePatchs();

        if (_bSaveBin)//Used in cooker mode
        {
            animPatchBank->saveBinary(cookFile);
        }
        return animPatchBank;
    }

    bbool AnimationLoadPatchBank::xmlReadTemplateBoneListT(  PugiXMLWrap * _XML, AnimTemplate* _template, PtrToStrID * _ptrToStrID, NamesMap * _boneIDToIndex, PugiXMLWrap::ErrorBehavior errBehavior)
    {
        if (_XML->enterElement("BonesListT"))
        {
            int pos = 0;
			u32 boneNb = _XML->getNumChildren("Element");
			_template->m_BonesCurFrame.resize(boneNb);

            if (_XML->enterElement("Element"))
			{
                do
                {
                    AnimBone*   bone =  &_template->m_BonesCurFrame[pos];
                    String      boneUID = AnimationLoadCommon::xmlReadBone(_XML, bone, _ptrToStrID, errBehavior);

                    if (bone->m_Name.isValid())
                        _template->m_NameToBone.setID(bone->m_Name, pos);
                    _boneIDToIndex->setID(boneUID, pos++);

                    AnimBoneDyn boneDyn;
                    AnimationLoadCommon::xmlReadBoneDyn(_XML, &boneDyn,errBehavior);
                    // set dynamic default value
#ifndef NO_RUNTIME_LOCK
                    boneDyn.m_LockedIndex = U32_INVALID;
#endif
                    _template->m_BonesCurFrameDyn.push_back(boneDyn);
                } while (_XML->nextSameElement());
			}
			ITF_ASSERT(boneNb == pos);
            _XML->exitElement();
        }

        // resolve ptr
        for (VectorAnim<AnimBone>::iterator iter = _template->m_BonesCurFrame.begin();
            iter != _template->m_BonesCurFrame.end(); ++iter)
        {
            PtrToStrID::iterator ptrIter = _ptrToStrID->find(&(*iter).m_BoneParent);
            if (ptrIter != _ptrToStrID->end())
            {
                NamesMap::Iterator parentIter(*_boneIDToIndex);
                parentIter.findKey((*ptrIter).second);
                ITF_ASSERT(!parentIter.isEnd());
                if (!parentIter.isEnd())
                {
                    (*iter).m_BoneParent = &_template->m_BonesCurFrame[parentIter.second()];
                }
            }
        }
        return btrue;
    }

    bbool AnimationLoadPatchBank::xmlReadAnimPatchPoints(PugiXMLWrap * _XML, AnimTemplate* _template, PtrToStrID *_ptrToStrID, NamesMap * _boneIDToIndex, PugiXMLWrap::ErrorBehavior errBehavior)
    {
        NamesMap    strIDToIndex;

        if (_XML->enterElement("PatchPointList"))
        {
            String UID;
            u32 nbPatchPoint = _XML->getNumChildren("Element");

			int pos = 0;
            if (nbPatchPoint)
            {
				_template->m_PatchPoints.resize(nbPatchPoint);
                if (_XML->enterElement("Element"))
				{
                    do
                    {
                        AnimPatchPoint* patchPoint = &_template->m_PatchPoints[pos]; 

                        UID.setText(_XML->readAttribute("UID/value", "error", errBehavior));

                        patchPoint->m_Index           = pos;
                        patchPoint->m_PosUV.m_x       = _XML->readFloatAttribute("PosUV/x", 0, errBehavior);
                        patchPoint->m_PosUV.m_y       = _XML->readFloatAttribute("PosUV/y", 0, errBehavior);
                        patchPoint->m_NormaleUV.m_x   = _XML->readFloatAttribute("NormaleUV/x", 0, errBehavior);
                        patchPoint->m_NormaleUV.m_y   = _XML->readFloatAttribute("NormaleUV/y", 0, errBehavior);

                        if (_XML->enterElement("LocalData"))
                        {
                            String boneUID; 
                            if (_XML->enterElement("Element"))
                                do
                                {
                                    AnimPatchPointLocal ppl;
                                    boneUID.setText(_XML->readAttribute("BoneUID/value", "error", errBehavior));
                                    NamesMap::Iterator boneIter(*_boneIDToIndex);
                                    boneIter.findKey(StringID(boneUID));

                                    ITF_ASSERT(!boneIter.isEnd());
                                    ppl.m_Pos.m_x       = _XML->readFloatAttribute("Pos/x", 0, errBehavior);
                                    ppl.m_Pos.m_y       = _XML->readFloatAttribute("Pos/y", 0, errBehavior);
                                    ppl.m_Normale.m_x   = _XML->readFloatAttribute("Normale/x", 0, errBehavior);
                                    ppl.m_Normale.m_y   = _XML->readFloatAttribute("Normale/y", 0, errBehavior);
                                    ppl.m_BonePtr       = &_template->m_BonesCurFrame[boneIter.second()];

#ifndef ITF_UNIQUE_PATCH_POINT_REF
                                    ppl.m_Influence     = _XML->readFloatAttribute("Influence/value", 0);

                                    patchPoint->m_LocalRefs.push_back(ppl);
#else
                                    patchPoint->m_mainRefs = ppl;
                                    // We assume that the data are compatible, only ONE ref
#endif
                                } while (_XML->nextSameElement());
                                _XML->exitElement();
                        }

                        strIDToIndex.setID(UID, pos++);
                    } while (_XML->nextSameElement());
				}
            }
			ITF_ASSERT(nbPatchPoint == pos);
            _XML->exitElement();
        }

        // resolve ptr
        for (VectorAnim<AnimBone>::iterator iter = _template->m_BonesCurFrame.begin();
            iter != _template->m_BonesCurFrame.end(); ++iter)
        {
            for (u32 i=0; i<(*iter).m_PatchPointsPtr.size(); i++)
            {
                PtrToStrID::iterator ptrIter = _ptrToStrID->find(&(*iter).m_PatchPointsPtr[i]);
                ITF_ASSERT(ptrIter!= _ptrToStrID->end());
                (*iter).m_PatchPointsPtr[i] = &_template->m_PatchPoints[strIDToIndex.get((*ptrIter).second)];
            }
        }

        return btrue;
    }

#if !defined(ITF_FINAL) && defined(ITF_DEBUG_STRINGID) && defined(ITF_WINDOWS)

    ///////////////////////////////////////////////////////////////////////////////////////////
    void AnimationLoadPatchBank::xmlWriteTemplateBoneList(pugi::xml_node _parentNode, AnimTemplate *_template, AnimSkeleton *_skeleton)
    {
        pugi::xml_node boneListNode = _parentNode.append_child("BonesListT");

        for (u32 boneIndex=0; boneIndex<_template->m_BonesCurFrame.size(); boneIndex++)
        {
            AnimBone *bone = &_template->m_BonesCurFrame[boneIndex];
            AnimBoneDyn *boneDyn = &_template->m_BonesCurFrameDyn[boneIndex];

            //const char *boneName=_template->m_BonesCurFrame[boneIndex]->m_Name.c_str();
            pugi::xml_node boneNode = boneListNode.append_child("Element");
            boneNode.append_attribute("index").set_value(boneIndex+1);

            AnimationLoadCommon::xmlWriteBoneDyn(boneNode, boneDyn, _template);        
            AnimationLoadCommon::xmlWriteBone(boneNode, bone, _skeleton);
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void AnimationLoadPatchBank::xmlWriteAnimPatchPoints(pugi::xml_node _parentNode, AnimTemplate *_template, const String8 &_templateName)
    {
        pugi::xml_node pointListNode = _parentNode.append_child("PatchPointList");
        for (u32 pointIndex=0; pointIndex<_template->m_PatchPoints.size(); pointIndex++)
        {
            pugi::xml_node pointNode = pointListNode.append_child("Element");
            pointNode.append_attribute("index").set_value((int)pointIndex+1);

            AnimPatchPoint *point = &_template->m_PatchPoints[pointIndex];

            {
                AnimationLoadCommon::xmlWriteFloat(pointNode, "Angle", 0.f);

                AnimationLoadCommon::xmlWriteFloat(pointNode, "Hiden", 0.f);

                AnimationLoadCommon::xmlWriteVector(pointNode, "PosUV", point->m_PosUV);

                AnimationLoadCommon::xmlWriteVector(pointNode, "NormaleUV", point->m_NormaleUV);

                AnimationLoadCommon::xmlWriteBool(pointNode, "SetPicking", btrue);

                AnimationLoadCommon::xmlWriteFloat(pointNode, "Zorder", 0.f); ///////////////?

                AnimationLoadCommon::xmlWriteBool(pointNode, "isInversed", bfalse);

                AnimationLoadCommon::xmlWriteFloat(pointNode, "Type", 0.f);

                AnimationLoadCommon::xmlWritePointer(pointNode, "UID", point);

                AnimationLoadCommon::xmlWritePointer(pointNode, "BrotherUID", point->m_mainRefs.m_BonePtr);

                AnimationLoadCommon::xmlWritePointer(pointNode, "TemplateUID", _template);

                pugi::xml_node colorNode = pointNode.append_child("Color");
                {
                    AnimationLoadCommon::xmlWriteFloat(colorNode, "A", 255);
                    AnimationLoadCommon::xmlWriteFloat(colorNode, "B", 255);
                    AnimationLoadCommon::xmlWriteFloat(colorNode, "G", 255);
                    AnimationLoadCommon::xmlWriteFloat(colorNode, "R", 255);
                }


                pugi::xml_node localData = pointNode.append_child("LocalData");

                pugi::xml_node localElement = localData.append_child("Element");
                localElement.append_attribute("index").set_value(1);
                {
                    const AnimPatchPointLocal &local=point->m_mainRefs;

                    AnimBone *b = local.m_BonePtr;

                    AnimationLoadCommon::xmlWritePointer(localElement, "BoneUID", b);

                    AnimationLoadCommon::xmlWriteFloat(localElement, "Influence", 1.f);

                    AnimationLoadCommon::xmlWriteVector(localElement, "Normale", local.m_Normale);

                    AnimationLoadCommon::xmlWriteVector(localElement, "Pos", local.m_Pos);                            
                }
            }
        }

    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void AnimationLoadPatchBank::writeAnimPatchBank(AnimPatchBank *_pbk, AnimSkeleton *_skeleton, const char *_path)
    {
        pugi::xml_document doc;
        pugi::xml_node rootNode = doc.append_child("root");
        

        {
            pugi::xml_node listNode = rootNode.append_child("List");
            {
                NamesMap::Iterator nameIter(_pbk->m_NameToTemplate);
                for (;!nameIter.isEnd(); nameIter.increment())
                {
                    //String8 keyName = nameIter.dbg();
                    String8 keyName = StringID(nameIter.first()).getDebugString();
                    int keyIndex = nameIter.second();
                    AnimTemplate *templte = _pbk->m_templateList[keyIndex];

                    pugi::xml_node templateNode = listNode.append_child(keyName.cStr());

                    xmlWriteTemplateBoneList(templateNode, templte, _skeleton);
                    xmlWriteAnimPatchPoints(templateNode, templte, keyName);
                    AnimationLoadCommon::xmlWritePointer(templateNode, "UID", templte);
                    AnimationLoadCommon::xmlWriteString(templateNode, "Name", keyName.cStr());
                    AnimationLoadCommon::xmlWriteFloat(templateNode, "LenghtFactor", templte->GetLenghtFactor());

                }
            }
            AnimationLoadCommon::xmlWriteFloat(rootNode, "MediaWidth", 1);
            AnimationLoadCommon::xmlWriteFloat(rootNode, "Ratio", 1);
        }

        String fullPath;
        if (FILEMANAGER->TranslatePath(fullPath, _path, 0))
            doc.save_file(StringConverter(fullPath).getChar());
    }
#endif
};
