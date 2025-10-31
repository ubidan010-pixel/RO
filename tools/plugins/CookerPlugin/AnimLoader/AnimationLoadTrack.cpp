#include "engine/precompiled_engine.h"

#ifndef _ITF_ANIMATIONLOADTRACK_H_
#include "tools/plugins/CookerPlugin/AnimLoader/AnimationLoadTrack.h"
#endif //_ITF_ANIMATIONLOADTRACK_H_

#ifndef _ITF_ANIMATIONTRACK_H_
#include "engine/animation/AnimationTrack.h"
#endif //_ITF_ANIMATIONTRACK_H_

#ifndef _ITF_ANIMATIONMANAGER_H_
#include "engine/animation/AnimationManager.h"
#endif //_ITF_ANIMATIONMANAGER_H_

#ifndef _ITF_ANIMATIONRES_H_
#include "engine/animation/AnimationRes.h"
#endif //_ITF_ANIMATIONRES_H_

#ifndef _ITF_ANIMATIONSKELETON_H_
#include    "engine/animation/AnimationSkeleton.h"
#endif // _ITF_ANIMATIONSKELETON_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif // _ITF_FILESERVER_H_

#ifndef _ITF_FILEMANAGER_H_
#include "core/AdaptersInterfaces/FileManager.h"
#endif //_ITF_FILEMANAGER_H_

#ifndef _ITF_ANIMATIONDEPENDENCIES_H_
#include "engine/animation/AnimationDependencies.h"
#endif //_ITF_ANIMATIONDEPENDENCIES_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_ANIMATIONPOLYLINE_H_
#include    "engine/animation/AnimationPolyline.h"
#endif // _ITF_ANIMATIONPOLYLINE_H_

namespace ITF
{
    bool keySort(const AnimTrackBoneWithLock &b1, const AnimTrackBoneWithLock &b2) { 
        return b1.m_frame<b2.m_frame;
    }


    AnimTrack * AnimationLoadTrack::readAnimTrack(  AnimSkeleton *_skeleton, const Path& _XMLFile_Path,const String& _platform, bbool _raiseError, bbool _bSaveBin)
    {
        PugiXMLWrap::ErrorBehavior  errBehavior = _raiseError ? PugiXMLWrap::RaiseError : PugiXMLWrap::DoNothing;
        PathString_t                _szXMLFile;
        
        _XMLFile_Path.getString(_szXMLFile);

        PugiXMLWrap                 XML;

        if (!_skeleton)
            return NULL;

        String      cookFile        = FILESERVER->getCookedNamePlatform(_szXMLFile,_platform);
        AnimTrack * animTrack       =  newAlloc(mId_Animation,AnimTrack());


        if (!XML.openFile(_szXMLFile,bfalse, errBehavior))
        {
            ITF_ERROR_SHOW(_raiseError, "anim %ls not found !", _szXMLFile.cStr());
            SF_DEL(animTrack);
            return NULL;
        }

        // get skeleton path
        String  squeleton = XML.readAttribute("Squeleton/value", "");
        if (squeleton.isEmpty())
        {
            SF_DEL(animTrack);
            ITF_ERROR_SHOW(_raiseError, "no skeleton found in %ls\nplease reload animation with GenAnim and save", _szXMLFile.cStr());
            return NULL;
        }
        animTrack->m_skeletonPath = ANIM_MANAGER->getStringID(squeleton);

        // load default patch bank
        if (XML.enterElement("PatchBankList"))
        {
            if (XML.enterFirstElement())
                do
                {                   
                    String Id = XML.readElementName();
                    if (Id == "Element")
                        Id = XML.readAttribute("index", "", errBehavior);
                    
                    animTrack->m_nameToPatch.setID(Id, animTrack->m_patchBankPathList.size());

                    PbkRedirect pbkRedirect;
                    String      patchBankPath        = XML.readAttribute("value", "", errBehavior);
                    String      redirectPatchBank    = FilePath::getDirectory(patchBankPath) +  AnimTrack::s_redirectPatchBank;
                    String      newPath;

                    pbkRedirect.openBinary(redirectPatchBank, bfalse);
                    if (pbkRedirect.findPatchBank(patchBankPath, newPath))
                        animTrack->m_patchBankPathList.push_back(ANIM_MANAGER->getStringID(newPath));
                    else
                    {
                        animTrack->m_patchBankPathList.push_back(ANIM_MANAGER->getStringID(patchBankPath));
                    }

                    animTrack->m_redirectPatchBankFlushTimeList.push_back(FILEMANAGER->getLastTimeWriteAccess(redirectPatchBank));
                } while (XML.nextElement());
                XML.exitElement();
        }

        animTrack->m_skeletonFlushTime              = _skeleton->m_flushTime;

        if (_skeleton->m_BonesCurFrame.size() == 0)
        {
            SF_DEL(animTrack);

            ITF_ERROR_SHOW(_raiseError, "empty skeleton found in %ls\nplease check your datas", _szXMLFile.cStr());
            return NULL;
        }

        // Animation is on First Scene

        AnimTrackBonesList  emptyBoneList;
        for (u32 i=0; i<_skeleton->m_BonesCurFrame.size(); ++i)
            animTrack->m_bonesList.push_back(emptyBoneList);

        VectorAnim<VectorAnimTrackBoneWithLock> tempTrackList;
        tempTrackList.resize(_skeleton->m_BonesCurFrame.size());

        bbool setRemoveLocks = XML.readIntAttribute("RemoveLocks/value", 1);
        bbool saveRootPos = XML.readIntAttribute("SaveRootPos/value", 0);
        bbool addSkeletonAABB = XML.readIntAttribute("AddSkeletonAABB/value", 1);

        if (XML.enterElement("PostureTab"))
        {
            if (XML.enterElement("Element"))
                do
                {
                    f32 index = (f32)XML.readIntAttribute("index", 0) - 1.0f;
                    if (index < 0.f)
                        continue;
                    if (animTrack->m_endFrame < index)
                        animTrack->m_endFrame = index;
                    if (XML.enterElement("bones"))
                    {
                        if (XML.enterFirstElement())
                            do
                            {
                                const String &boneUID = XML.readElementName();
                                NamesMap::Iterator keyUID(_skeleton->m_UIDToBone);
                                keyUID.findKey(StringID(boneUID));
                                if (keyUID.isEnd())
                                    continue;

                                int                      idx   = keyUID.second();
                                AnimTrackBoneWithLock trackBone;

                                trackBone.m_PosLocal.m_x       = XML.readFloatAttribute("PosLocal/x", 0.f);
                                trackBone.m_PosLocal.m_y       = XML.readFloatAttribute("PosLocal/y", 0.f);
                                trackBone.m_AngleLocal         = XML.readFloatAttribute("AngleLocal/value", 0.f) * MTH_DEGTORAD;

                                trackBone.m_Scale.m_x          = XML.readFloatAttribute("ScaleX/value", 1.f);
                                trackBone.m_Scale.m_y          = XML.readFloatAttribute("ScaleY/value", 1.f);

                                trackBone.m_Zorder             = XML.readFloatAttribute("Zorder/value", 0.f);
                                trackBone.m_Alpha              = XML.readFloatAttribute("Alpha/value", 0.f);

                                if (!strcmp(XML.readAttribute("Locked/type", ""), "vector"))
                                {
                                    AnimLock    *animLock =  newAlloc(mId_Animation,AnimLock());
                                    animLock->m_details.m_Index           = 0;
                                    animLock->m_details.m_OrderedIndex    = 0;
                                    switch (XML.readIntAttribute("AngleSign/value", 0))
                                    {
                                    case 1:
                                        animLock->m_details.m_AngleSign = 0x1;
                                        break;
                                    case -1:
                                        animLock->m_details.m_AngleSign = 0x2;
                                        break;
                                    default:
                                        animLock->m_details.m_AngleSign = 0x0;
                                        break;
                                    }
                                    animLock->m_Locked.m_x       = XML.readFloatAttribute("Locked/x", 0.f, errBehavior);
                                    animLock->m_Locked.m_y      = XML.readFloatAttribute("Locked/y", 0.f, errBehavior);
                                    animLock->m_LockedAngle     = XML.readFloatAttribute("LockedAngle/value", 0.f, errBehavior) * MTH_DEGTORAD;
                                    trackBone.m_Lock            = animLock;
                                } else
                                    trackBone.m_Lock            = NULL;

                                trackBone.m_frame               = index;
                                tempTrackList[idx].push_back(trackBone);
                            } while (XML.nextElement());
                            XML.exitElement();
                    }

                    if (XML.enterElement("fpl")) // new pointing system
                    {
                        AnimTrackBML trackBML;
                        trackBML.m_frame = index;

                        if (XML.enterFirstElement())
                            do
                            {
                                String TemplateUID;
                                ITF_VERIFY(XML.enterFirstElement());
                                const char * bankId = XML.readAttribute("value", "", errBehavior);
                                ITF_VERIFY(XML.nextElement());
                                const char * listId = XML.readAttribute("value", "", errBehavior);
                                //ITF_VERIFY(!XML.nextElement());
                                XML.exitElement();
                                TemplateRef refObj;
                                refObj.m_bankName   = bankId;
                                refObj.m_patchName  = listId;

                                trackBML.m_templateRefs.push_back(refObj);
                            } while (XML.nextElement());

                            animTrack->m_bmlList.push_back(trackBML);
                            XML.exitElement();
                    }


                    if (XML.enterElement("pol")) // new pointing system
                    {
                        AnimTrackPolyline trackPolyline;
                        trackPolyline.m_frame = index;

                        if (XML.enterFirstElement())
                        do
                        {
                            StringID PolId = XML.readElementName();
                            if (_skeleton->m_PolylineBank->m_NameToPolyline.find(PolId) >= 0)
                                trackPolyline.m_polylineRefs.push_back(PolId);
                        } while (XML.nextElement());

                        animTrack->m_polylineList.push_back(trackPolyline);
                        XML.exitElement();
                    }

                    if (XML.enterElement("events"))
                    {
                        AnimTrackFrameEvents frameEvents;
                        frameEvents.m_frame = index;

                        if (XML.enterElement("Element"))
                        do
                        {
                            AnimEvtType type = (AnimEvtType)XML.readIntAttribute("Type/value", 0, errBehavior);
                            AnimMarkerEvent* newEvent = AnimMarkerEvent::createEvent(type);

                            if ( !newEvent )
                            {
                                continue;
                            }

                            bbool ret = newEvent->ReadXML(&XML, &_skeleton->m_UIDToBone, (int)errBehavior);
#ifdef ITF_WINDOWS
                            if (!ret && _raiseError)
                            {
                                char buf[256];
                                SPRINTF_S(buf,256,"Event with no name in file %s",StringConverter(_szXMLFile).getChar());
                                if (SYSTEM_ADAPTER)
                                    SYSTEM_ADAPTER->messageBox("Warning", buf, MB_OK,ITF_IDYES);
                            }
#endif

                            frameEvents.m_frameEvents.push_back(newEvent);
                        } while (XML.nextElement());
                        animTrack->m_eventList.push_back(frameEvents);
                        XML.exitElement();
                    }
                } while (XML.nextSameElement());
                XML.exitElement();

                // sort keys in tempTrackList
                for (VectorAnim<VectorAnimTrackBoneWithLock>::iterator it = tempTrackList.begin();
                     it != tempTrackList.end(); ++it)
                {
                    std::sort(it->begin(), it->end(), keySort);
                }

                if (!animTrack->fillSoundFromEvents())
                {
                    SF_DEL(animTrack);
                    return NULL;
                }

                // remove lock system -> needed for blending
                if (setRemoveLocks)
                {
                    animTrack->m_dummySkeleton = _skeleton;
                    animTrack->removeLock(tempTrackList);
                    animTrack->m_dummySkeleton = NULL;
                }

                // Use virtual root if present
                animTrack->m_dummySkeleton = _skeleton;
                animTrack->useMagicBoxAsRoot(tempTrackList);
                animTrack->m_dummySkeleton = NULL;


                animTrack->computeMaxValues(tempTrackList);

                // now fill "final" used struct
                VectorAnim<VectorAnimTrackBonePAS>    trackListPAS;
                VectorAnim<VectorAnimTrackBoneZAL>    trackListZAL;

                trackListPAS.resize(tempTrackList.size());
                trackListZAL.resize(tempTrackList.size());

                animTrack->fillPASZALListFromBoneList(tempTrackList, trackListPAS, trackListZAL);
                animTrack->cleanPASKeys(trackListPAS);
                animTrack->cleanZALKeys(trackListZAL);
                animTrack->fillPASZAL(trackListPAS, trackListZAL);
        }

        
        AABB bounding;
        bounding.invalidate();
        if (XML.enterElement("aabb"))
        {
            f32 xMin =  XML.readFloatAttribute("xMin/value",  FLT_MAX);
            f32 xMax =  XML.readFloatAttribute("xMax/value", -FLT_MAX);
            f32 yMin =  XML.readFloatAttribute("yMin/value",  FLT_MAX);
            f32 yMax =  XML.readFloatAttribute("yMax/value", -FLT_MAX);

            if (xMin <= xMax && yMin <= yMax)
            {
                bounding = AABB(Vec2d(xMin, -yMin));
                bounding.grow(Vec2d(xMax, -yMax));
            }
            XML.exitElement();
        } 

        if (!bounding.isValid())
            addSkeletonAABB = btrue;

        // after XML read, process track data
        animTrack->m_dummySkeleton = _skeleton;
        animTrack->processTagAndBounding(saveRootPos, addSkeletonAABB);
        animTrack->m_dummySkeleton = NULL;

        if (!(animTrack->m_tags & ANIM_TRACK_IS_DEFAULT_RIGHT))
            bounding.FlipHorizontaly(bfalse);

        if (bounding.isValid())
        {
            if (animTrack->m_bounding.isValid())
                animTrack->m_bounding.grow(bounding);
            else
                animTrack->m_bounding = bounding;
        }


        if (XML.enterElement("scissor"))
        {
            f32 xMin =  XML.readFloatAttribute("xMin/value",  FLT_MAX);
            f32 xMax =  XML.readFloatAttribute("xMax/value", -FLT_MAX);
            f32 yMin =  XML.readFloatAttribute("yMin/value",  FLT_MAX);
            f32 yMax =  XML.readFloatAttribute("yMax/value", -FLT_MAX);

            if (xMin <= xMax && yMin <= yMax)
            {
                if (animTrack->m_tags & ANIM_TRACK_IS_DEFAULT_RIGHT)
                {
                    animTrack->m_scissor = AABB(Vec2d(xMin, -yMin));
                    animTrack->m_scissor.grow(Vec2d(xMax, -yMax));
                } else
                {
                    animTrack->m_scissor = AABB(Vec2d(-xMax, -yMin));
                    animTrack->m_scissor.grow(Vec2d(-xMin, -yMax));
                }
            }
            XML.exitElement();
        }

        if (_bSaveBin)//Used in cooker mode
        {
            animTrack->saveBinary(cookFile);
        }

        // remove Skeleton if forced load

        return animTrack;
    }


    bbool AnimationLoadTrack::fillAnimTrackResources(  AnimationDependenciesMap * dependenciesMap, const Path& _XMLFile_Path, bbool _raiseError)
    {
        PugiXMLWrap::ErrorBehavior errBehavior = _raiseError ? PugiXMLWrap::RaiseError : PugiXMLWrap::DoNothing;
        PathString_t _szXMLFile;
        
        _XMLFile_Path.getString(_szXMLFile);

        PugiXMLWrap XML;
        if (!XML.openFile(_szXMLFile,bfalse, errBehavior))
        {
            return bfalse;
        }

        AnimationDependencies   newAnimDependencies;
        MapPath *               mapPath     = dependenciesMap->getPathMap();

        const char * squeleton  = XML.readAttribute("Squeleton/value", "");
        newAnimDependencies.setSkeletonPath(mapPath, squeleton);

        // load default patch bank
        if (XML.enterElement("PatchBankList"))
        {
            if (XML.enterFirstElement())
                do
                {                   
                    PbkRedirect pbkRedirect;
                    String      patchBankPath        = XML.readAttribute("value", "", errBehavior);
                    String      redirectPatchBank    = FilePath::getDirectory(patchBankPath) +  AnimTrack::s_redirectPatchBank;
                    String      newPath;

                    pbkRedirect.openBinary(redirectPatchBank, bfalse);
                    if (pbkRedirect.findPatchBank(patchBankPath, newPath))
                        newAnimDependencies.pushTexture(mapPath, newPath);
                    else
                        newAnimDependencies.pushTexture(mapPath, patchBankPath);
                } while (XML.nextElement());
                XML.exitElement();
        }
        newAnimDependencies.setFileTime(FILEMANAGER->getLastTimeWriteAccess(_szXMLFile));

        // compute runtime data
        newAnimDependencies.processPatchBankData();

        if (XML.enterElement("aabb"))
        {
            f32 xMin =  XML.readFloatAttribute("xMin/value", FLT_MAX);
            f32 xMax =  XML.readFloatAttribute("xMax/value", -FLT_MAX);
            f32 yMin =  XML.readFloatAttribute("yMin/value", FLT_MAX);
            f32 yMax =  XML.readFloatAttribute("yMax/value", -FLT_MAX);

            newAnimDependencies.setBoundingFound(btrue);
            if (xMin <= xMax && yMin <= yMax)
            {
                newAnimDependencies.setBoundingBox(AABB(Vec2d(xMin, -yMax), Vec2d(xMax, -yMin)));
            }
        } else
        {
            newAnimDependencies.setBoundingFound(bfalse);
        }

        // this will also override deprecated data
        dependenciesMap->getMap()[_XMLFile_Path.getStringID()] = newAnimDependencies;
        return btrue;
    }



    bbool AnimationLoadTrack::fillDependencies(AnimationDependenciesMap * &_mapDep, AnimationDependencies * & _dep, const String & _anim, const Path & _dependencyFile, ResourceGroup *_group)
    {
        _mapDep             = AnimDependenciesMapResource::getDependenciesMap(_dependencyFile, _group );
        if (!_mapDep)
            return bfalse;

        MapDependencies & depMap = _mapDep->getMap();
        if (_mapDep->getAnimVersion() < AnimationDependenciesMap::getVersion())
            depMap.clear();

        MapDependencies::iterator mapIter = depMap.find(_anim);
        if (mapIter == depMap.end() || mapIter->second.getFileTime() != FILEMANAGER->getLastTimeWriteAccess(_anim))
        {
            // file not referenced need to be added
            if (!fillAnimTrackResources(  _mapDep, _anim, bfalse))
                return bfalse;
            mapIter     = depMap.find(_anim);

            // save the changes
            if (!FILEMANAGER->isRemote())
            {
                PathString_t fileName;

                _dependencyFile.getString(fileName);

                _mapDep->saveBinary(fileName);
            }
        }

        _dep = &mapIter->second;
        return btrue;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
#if !defined(ITF_FINAL) && defined(ITF_WINDOWS) && defined(ITF_DEBUG_STRINGID)
    void AnimationLoadTrack::writeAnimTrack(AnimTrack *_track, const char *_path)
    {
        pugi::xml_document doc;
        pugi::xml_node rootNode = doc.append_child("root");

        String relativePath(_path);
        String8 fileDir, baseName;
        relativePath = FilePath::normalizePath(relativePath);
        FilePath::transformToRelative(relativePath);
        fileDir = String8(FilePath::getDirectory(relativePath).cStr());
        baseName = String8(FilePath::getFilename(relativePath).cStr());

        f32 radToDeg = 180.f/MTH_PI;

        //
        AnimationLoadCommon::xmlWriteString(rootNode, "Squeleton", _track->m_skeletonPath.getDebugString());

        //export patch banks
        pugi::xml_node patchBankListNode = rootNode.append_child("PatchBankList");
        {
            for (u32 patchBankIndex=0; patchBankIndex<_track->m_nameToPatch.size(); patchBankIndex++)
            {
                
                u32 index = _track->m_nameToPatch.getAt(patchBankIndex);
                String8 n = StringID(_track->m_nameToPatch.getKeyAt(patchBankIndex)).getDebugString();
                //const String8 &n = _track->m_nameToPatch.getDebugAt(patchBankIndex);
                AnimPatchBank *patchBank = _track->getPatchBank(index);
                if (!patchBank)
                {
                    ITF_ERROR("Error while writing anim : NULL patchbank");
                    return;
                }

                const char *defaultExtension=".tga"; //We dont use the texture path for practical reasons. We rebuild it.

                String8 patchBankPath8 = _track->m_patchBankPathList[index].getDebugString();
                String patchBankPath(patchBankPath8.cStr());
                String imagePath = FilePath::getDirectory(patchBankPath)+
                    FilePath::getFilenameWithoutExtension(patchBankPath)+
                    defaultExtension;
                AnimationLoadCommon::xmlWriteString(patchBankListNode, n.cStr(), String8(imagePath.cStr()).cStr() );
            }
        }

        const VectorAnim<AnimTrackBonesList> &boneList = _track->m_bonesList;
        FastArray<AnimTrackBonePAS> & pas = _track->m_trackListPAS;
        VectorAnim<AnimTrackBoneZAL> & zal = _track->m_trackListZAL;

        pugi::xml_node postureTab = rootNode.append_child("PostureTab");
        {
            //export bones by frame
            AnimSkeleton *skeleton = _track->getSkeleton();
            {
                //For each posture
                for (u32 postureIndex=0; postureIndex<=(u32)_track->m_endFrame; postureIndex++)
                {
                    u16 frameTime = (u16)postureIndex;
                    pugi::xml_node postureElementNode = postureTab.append_child("Element");
                    postureElementNode.append_attribute("index").set_value(postureIndex+1);

                    pugi::xml_node boneTab = postureElementNode.append_child("bones");

                    for (u32 animBoneIndex=0; animBoneIndex<boneList.size(); animBoneIndex++)
                    {
                        const AnimTrackBonesList &abl=boneList[animBoneIndex];
                        if (abl.m_trackPASSize + abl.m_trackZALSize > 0)
                        {
                        	//Search bone index in m_UIDToBone map
                        	StringID boneID; ///////////////////
                        	for (u32 idIndex=0; idIndex<skeleton->m_UIDToBone.size(); idIndex++)
                        	{
                        		if ((i32)animBoneIndex==skeleton->m_UIDToBone.getAt(idIndex))
                        		{
                        			boneID = skeleton->m_UIDToBone.getKeyAt(idIndex);
                        			break;
                        		}
                        	}
                        	AnimBone *bone = skeleton->getBoneAt(animBoneIndex);
                        	String8 boneName;
                        	boneName.setTextFormat("%p", bone);
                        	//

                            bbool keyFound = bfalse;
                            for (u16 pasIdx = abl.m_trackPASIndex; !keyFound && pasIdx < abl.m_trackPASIndex + abl.m_trackPASSize; pasIdx ++ )
                            {
                                keyFound = pas[pasIdx].m_frame == frameTime;
                            }
                            for (u16 zalIdx = abl.m_trackZALIndex; !keyFound && zalIdx < abl.m_trackZALIndex + abl.m_trackZALSize; zalIdx ++ )
                            {
                                keyFound = zal[zalIdx].m_frame == frameTime;
                            }

                            if (keyFound)
                            {
                                AnimTrackBone abone;
#ifdef ANIM_TRACK_CACHE
                                AnimTrackBonePASCut cacheTab[ANIM_TRACK_CACHE_SIZE];
                                for (int cacheIdx = 0; cacheIdx<ANIM_TRACK_CACHE_SIZE; cacheIdx++)
                                    cacheTab[cacheIdx].m_frame = -1.f;
#endif

#ifdef ANIM_TRACK_CACHE
#ifndef NO_RUNTIME_LOCK
                                if (_track->fillTrackAtIndex(abone, cacheTab, abl, frameTime, NULL ))
#else
                                if (_track->fillTrackAtIndex(abone, cacheTab, abl, frameTime ))
#endif
#else
#ifndef NO_RUNTIME_LOCK
                                if (_track->fillTrackAtIndex(abone, abl, frameTime, NULL ))
#else
                                if (_track->fillTrackAtIndex(abone, abl, frameTime ))
#endif
#endif
                                {
                                    pugi::xml_node boneNode = boneTab.append_child(boneID.getDebugString());
                                    {
                                        AnimationLoadCommon::xmlWriteFloat(boneNode, "Alpha", abone.m_Alpha);
                                        AnimationLoadCommon::xmlWriteFloat(boneNode, "AngleLocal", abone.m_AngleLocal*radToDeg);
                                        AnimationLoadCommon::xmlWriteVector(boneNode, "PosLocal", abone.m_PosLocal);
                                        AnimationLoadCommon::xmlWriteFloat(boneNode, "ScaleX", abone.m_Scale.m_x);
                                        AnimationLoadCommon::xmlWriteFloat(boneNode, "ScaleY", abone.m_Scale.m_y);
                                        AnimationLoadCommon::xmlWriteFloat(boneNode, "Zorder", abone.m_Zorder);
                                    }
                                }
                            }
                        }
                    } //for animBoneIndex

                    //events
                    bbool dataFound=bfalse;
                    pugi::xml_node eventsList = postureElementNode.append_child("events");
                    {
                        for (u32 evtIndex=0; evtIndex<_track->m_eventList.size(); evtIndex++)
                        {
                            AnimTrackFrameEvents &evt = _track->m_eventList[evtIndex];
                            if (evt.m_frame!=postureIndex)
                                continue;

                            dataFound = btrue;
                            for (u32 pIndex=0; pIndex<evt.m_frameEvents.size(); pIndex++)
                            {
                                pugi::xml_node elementNode = eventsList.append_child("Element");
                                elementNode.append_attribute("index").set_value(pIndex+1);

                                evt.m_frameEvents[pIndex]->WriteXML(elementNode, skeleton);
                            }
                        }
                    }
                    if (!dataFound)
                        postureElementNode.remove_child(eventsList);

                    //polylines
                    dataFound = bfalse;
                    pugi::xml_node polylinesList = postureElementNode.append_child("pol");
                    {
                        //search in bmlList : AnimTrackBML with same frame index
                        for (u32 polIndex=0; polIndex<_track->m_polylineList.size(); polIndex++)
                        {
                            AnimTrackPolyline &pol = _track->m_polylineList[polIndex];
                            if (pol.m_frame!=postureIndex)
                                continue;

                            dataFound = btrue;
                            for (u32 pIndex=0; pIndex<pol.m_polylineRefs.size(); pIndex++)
                            {
                                const char * polyName = pol.m_polylineRefs[pIndex].getDebugString();
                                AnimationLoadCommon::xmlWriteBool(polylinesList, polyName, btrue);
                            }
                        }
                    }
                    if (!dataFound)
                        postureElementNode.remove_child(polylinesList);

                    //export FPL
                    dataFound = bfalse;
                    pugi::xml_node patchList = postureElementNode.append_child("fpl");
                    {
                        //search in bmlList : AnimTrackBML with same frame index
                        for (u32 bmlIndex=0; bmlIndex<_track->m_bmlList.size(); bmlIndex++)
                        {
                            AnimTrackBML &bml = _track->m_bmlList[bmlIndex];
                            if (bml.m_frame!=postureIndex)
                                continue;

                            dataFound = btrue;
                            for (u32 templateIndex=0; templateIndex<bml.m_templateRefs.size(); templateIndex++)
                            {
                                const TemplateRef &templte = bml.m_templateRefs[templateIndex];
                                pugi::xml_node elementNode = patchList.append_child("Element");
                                elementNode.append_attribute("index").set_value(templateIndex+1);
                                {
                                    pugi::xml_node e1 = AnimationLoadCommon::xmlWriteString(elementNode, "Element", templte.m_bankName.getDebugString());
                                    pugi::xml_node e2 = AnimationLoadCommon::xmlWriteString(elementNode, "Element", templte.m_patchName.getDebugString());
                                    e1.append_attribute("index").set_value(1);
                                    e2.append_attribute("index").set_value(2);
                                }
                            }
                        }

                    }
                    if (!dataFound)
                        postureElementNode.remove_child(patchList);

                } //for (ITF_SET<f32>::const_iterator frameIter = frames.begin();


            }
        }
        
        if (_track->m_bounding.isValid())
        {
            pugi::xml_node aabbNode = rootNode.append_child("aabb");
            {
                AnimationLoadCommon::xmlWriteFloat(aabbNode, "xMin", _track->m_bounding.getMin().m_x);
                AnimationLoadCommon::xmlWriteFloat(aabbNode, "yMin", _track->m_bounding.getMin().m_y);
                AnimationLoadCommon::xmlWriteFloat(aabbNode, "xMax", _track->m_bounding.getMax().m_x);
                AnimationLoadCommon::xmlWriteFloat(aabbNode, "yMax", _track->m_bounding.getMax().m_y);
            }
        }

        if (_track->m_scissor.isValid())
        {
            pugi::xml_node aabbNode = rootNode.append_child("scissor");
            {
                AnimationLoadCommon::xmlWriteFloat(aabbNode, "xMin", _track->m_scissor.getMin().m_x);
                AnimationLoadCommon::xmlWriteFloat(aabbNode, "yMin", _track->m_scissor.getMin().m_y);
                AnimationLoadCommon::xmlWriteFloat(aabbNode, "xMax", _track->m_scissor.getMax().m_x);
                AnimationLoadCommon::xmlWriteFloat(aabbNode, "yMax", _track->m_scissor.getMax().m_y);
            }
        }


        String fullPath;
        if (FILEMANAGER->TranslatePath(fullPath, _path, 0))
            doc.save_file(StringConverter(fullPath).getChar());

    }
#endif

};
