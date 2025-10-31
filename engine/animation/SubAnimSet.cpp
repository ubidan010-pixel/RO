#include "precompiled_engine.h"

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_SUBANIMLIGHTSET_H_
#include    "engine/animation/SubAnimSet.h"
#endif //_ITF_SUBANIMLIGHTSET_H_

#ifndef _ITF_METRONOME_H_
#include "engine/sound/Metronome.h"
#endif // _ITF_METRONOME_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_ANIMATIONRES_H_
#include "engine/animation/AnimationRes.h"
#endif //_ITF_ANIMATIONRES_H_

#ifndef _ITF_RESOURCEGROUP_H_
#include "engine/resources/ResourceGroup.h"
#endif //_ITF_RESOURCEGROUP_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif // _ITF_FILESERVER_H_

#ifndef _ITF_ANIMATIONPATCHBANK_H_
#include    "engine/animation/AnimationPatchBank.h"
#endif // _ITF_ANIMATIONPATCHBANK_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_ANIMATIONDEPENDENCIES_H_
#include "engine/animation/AnimationDependencies.h"
#endif //_ITF_ANIMATIONDEPENDENCIES_H_

namespace ITF
{
BEGIN_SERIALIZATION(SubAnim_Template) 
    SERIALIZE_MEMBER("friendlyName",m_friendlyName);
    SERIALIZE_MEMBER("name",m_animPath); 
    SERIALIZE_MEMBER("playRate",m_playRate); 
    SERIALIZE_MEMBER("loop",m_loop); 
    SERIALIZE_MEMBER("useRootRotation",m_useRootRotation); 
    SERIALIZE_MEMBER("reverse",m_reversed); 
    SERIALIZE_MEMBER("defaultFlip",m_defaultFlip); 
    SERIALIZE_MEMBER("markerStart",m_markerStart); 
    SERIALIZE_MEMBER("markerStop",m_markerStop); 
    SERIALIZE_MEMBER("procedural",m_procedural);
    SERIALIZE_MEMBER("sync",m_sync);
    SERIALIZE_MEMBER("syncEighthNote",m_syncEighthNote);
    SERIALIZE_MEMBER("syncRatio",m_syncRatio);
    SERIALIZE_MEMBER("allowSyncOffset",m_allowSyncOffset);
	SERIALIZE_MEMBER("shadowMul",m_shadowMul);
    SERIALIZE_METRONOMETYPE("metronome",m_metronomeType);
END_SERIALIZATION()

SubAnim_Template::SubAnim_Template()
: m_playRate(1.f)
, m_syncRatio(1.0f)
, m_syncEighthNote(0)
, m_loop(bfalse)
, m_useRootRotation(btrue)
, m_reversed(bfalse)
, m_defaultFlip(bfalse)
, m_procedural(bfalse)
, m_sync(bfalse)
, m_shadowMul(1.f)
, m_metronomeType(METRONOME_TYPE_DEFAULT)
, m_start(0.f)
, m_stop(0.f)
, m_allowSyncOffset(btrue)
{
}


SubAnim_Template::~SubAnim_Template()
{
    if (m_trackExt.m_trackResourceID.isValidResourceId())
    {
        RESOURCE_MANAGER->queryRemoveUserToResourceLogicalData(m_trackExt.m_trackResourceID);
    }

    ITF_VECTOR<BankRedirect>::iterator iter = m_trackExt.m_redirectStates.begin();
    for (; iter != m_trackExt.m_redirectStates.end(); ++iter)
    {
        ITF_VECTOR<ResourceID>::iterator texIter = iter->m_textureResourceIDList.begin();
        ITF_VECTOR<ResourceID>::iterator patIter = iter->m_patchBankResourceIDList.begin();

        for (;texIter != iter->m_textureResourceIDList.end() &&
              patIter != iter->m_patchBankResourceIDList.end();
              ++texIter, ++patIter)
        {
            RESOURCE_MANAGER->queryRemoveUserToResourceLogicalData(*texIter);
            RESOURCE_MANAGER->queryRemoveUserToResourceLogicalData(*patIter);
        }
    }
}


AnimTrack * SubAnim_Template::resolveMarkers( const String & _animPath )
{
    f32 start = m_start;
    f32 stop  = m_stop;

    //check anims
    String animPath(_animPath);
    PathString_t animPathFilename;

    getAnimPath().getString(animPathFilename);

    animPath += animPathFilename;

    ResourceID  res;
    AnimTrack * track = NULL;

    if (!RESOURCE_MANAGER->getResourceIdFromFile(res, animPath, Resource::ResourceType_AnimTrack))
    {
        LOG("ANIM %s referenced by %s does not exist", StringToUTF8(animPath).get(), getFriendlyName().getDebugString());
        return NULL;
    }

    AnimTrackResource* pAnimTrackRes = NULL;

    pAnimTrackRes = static_cast<AnimTrackResource*>(res.getResource());
    if (pAnimTrackRes)
        track = pAnimTrackRes->getTrack();

    ITF_ASSERT(pAnimTrackRes && track);
    if (!pAnimTrackRes || !track)
        return NULL;

    // Set resource is Done HERE !!!
    setTrack(res);

    if (start <= MTH_EPSILON && getStartMarker().isValid())
    {
        f32 frame = track->GetEventFrame(AnimAnimationEvent::GetClassCRCStatic(),getStartMarker());
        ITF_WARNING_CATEGORY(Anim,NULL,frame>=0.0f,"Animation %s (%s) markerStart %s not found",
            getFriendlyName().getDebugString(),
            StringToUTF8(animPath).get(),
            getStartMarker().getDebugString() );
        if (frame >= 0.f)
            start = frame * ANIMATION_FRAMERATE;
    }
    if (stop <= MTH_EPSILON)
    {
        if (getStopMarker().isValid())
        {
            f32 frame = track->GetEventFrame(AnimAnimationEvent::GetClassCRCStatic(),getStopMarker())+1.f;
            ITF_WARNING_CATEGORY(Anim,NULL,frame>=1.0f," Animation %s (%s) markerStop %s not found",
                getFriendlyName().getDebugString(),
                StringToUTF8(animPath).get(),
                getStopMarker().getDebugString());
            if (frame >= 0.f)
                stop = frame * ANIMATION_FRAMERATE;
            else
                stop = track->getDuration();
        }
        else
        {
            if (getStartMarker().isValid())
            {
                LOG("ANIM %s referenced by %s doesn't have a stopMarker", StringToUTF8(animPath).get(), getFriendlyName().getDebugString());
            }
            stop = track->getDuration();
        }
    }

    setStart(start);
    setStop(stop);

    ITF_WARNING_CATEGORY(Anim,
        NULL, !(start == stop || (start == 0.0f && stop == 0.0f)),
        " Animation [ %s (%s) ] has a duration of 0.0 seconds",
        getFriendlyName().getDebugString(), StringToUTF8(animPath).get());

    return track;
}

ITF_INLINE void SubAnim_Template::setTrack( ResourceID _trackResource )
{
    if (m_trackExt.m_trackResourceID.isValidResourceId())
        RESOURCE_MANAGER->queryRemoveUserToResourceLogicalData(m_trackExt.m_trackResourceID);

    m_trackExt.m_trackResourceID = _trackResource;
    RESOURCE_MANAGER->queryAddUserToResourceLogicalData(m_trackExt.m_trackResourceID);
}


BEGIN_SERIALIZATION(BankChange_Template)
    SERIALIZE_MEMBER("friendlyName",m_friendlyName); 
    SERIALIZE_MEMBER("bankName",m_bankName); 
    SERIALIZE_MEMBER("bankPath",m_bankPath); 
    SERIALIZE_MEMBER("state",m_state); 
END_SERIALIZATION()

BankChange_Template::BankChange_Template()
: m_state(U32_INVALID)
{
}

//---------------------------------------------------------------------------------
BEGIN_SERIALIZATION(SubAnimSet_Template)
    SERIALIZE_CONTAINER_OBJECT("animations",m_subAnimList);
    SERIALIZE_CONTAINER_OBJECT("banks",m_bankChangeList);
    SERIALIZE_CONTAINER("skipFiles", m_skipFiles);

    // This data are cooking process
    SERIALIZE_MEMBER("redirectDone",m_redirectDone);
    SERIALIZE_CONTAINER("resourceList", m_resourceList);
    SERIALIZE_CONTAINER("resourceTypeList", m_resourceTypeList);
    SERIALIZE_OBJECT("animAABB", m_animsAABB);
    SERIALIZE_CONTAINER("nameId", m_nameId);
    SERIALIZE_CONTAINER("nameResIdx", m_nameResIdx);
END_SERIALIZATION()

SubAnimSet_Template::SubAnimSet_Template() 
: m_markerResolved(bfalse)
, m_redirectDone(bfalse)
{
    m_animsAABB.invalidate();
}

void SubAnimSet_Template::postSerialize(const String & _path)
{
    m_markerResolved = bfalse;
    m_nameToSubAnim.clear();
    ITF_VECTOR<SubAnim_Template>::iterator it = m_subAnimList.begin();
    for (u32 idx = 0; it != m_subAnimList.end(); ++it, ++idx)
    {
        SubAnim_Template &animLight = *it;
        ITF_ASSERT(m_nameToSubAnim.find(animLight.getFriendlyName()) < 0);
        m_nameToSubAnim.setID(animLight.getFriendlyName(), idx);
    }

    if (m_redirectDone)
        return;

    m_redirectDone = btrue;

    // process redirect bank change
    ITF_VECTOR<BankChange_Template>::iterator itcb =  m_bankChangeList.begin();
    PathString_t patchBankPath;

    for (; itcb != m_bankChangeList.end(); ++itcb)
    {
        PbkRedirect pbkRedirect;

        BankChange_Template & bankChangeTmplt = *itcb;

        bankChangeTmplt.getBankPath().getString(patchBankPath);

        String redirectPatchBank    = FilePath::getDirectory(patchBankPath) +  AnimTrack::s_redirectPatchBank;

        pbkRedirect.openBinary(redirectPatchBank, bfalse);
        String      newPath;
        if (pbkRedirect.findPatchBank(patchBankPath, newPath))
        {
            bankChangeTmplt.setBankPath(newPath);
        }
    }

    if (!m_subAnimList.size())
        return;

    String dependencyFile = _path;
    PathString_t subAnimFilename;

    m_subAnimList[0].getAnimPath().getString(subAnimFilename);

    dependencyFile       += subAnimFilename;
    Path dependencyPath   = AnimationDependenciesMap::getDependenciesFile(dependencyFile, FILESERVER->getPlatform());


    ITF_VECTOR<SubAnim_Template>::const_iterator subAnimTemplateIt = m_subAnimList.begin();
    for (; subAnimTemplateIt != m_subAnimList.end(); ++subAnimTemplateIt)
    {
        const SubAnim_Template subAnimTemplate = *subAnimTemplateIt;

        const PathChar_t *animPathBasename = subAnimTemplate.getAnimPath().getBasename();
        u32 filenameWithoutExt_Len;
        const PathChar_t *lastDot = (const PathChar_t*)PATHSTRINGCHR((PathCharStdlib_t*)animPathBasename, PATHSTRING_T('.'));
        if (lastDot)
        {
            filenameWithoutExt_Len = u32(lastDot-animPathBasename);
        }
        else
        {
            filenameWithoutExt_Len = u32(PATHSTRINGLEN((PathCharStdlib_t*)animPathBasename));
        }
        StringID id = StringID::StrToCRC(animPathBasename, filenameWithoutExt_Len); ///IMPORTANT : THIS IS FOR OPTIMIZATION. IF YOU WANT TO CHANGE IT, CHECK WITH SEB

        PathChar_t fullName[PATH_C_BUFFERSIZE];
        PathChar_t partialName[PATH_C_BUFFERSIZE];
        char pathBuffer[512];
        _path.charCopyToBuffer(pathBuffer, sizeof(pathBuffer));
        subAnimTemplate.getAnimPath().getString(partialName);
        Path::concatenateStrings(fullName, pathBuffer, partialName);

        Path track = fullName;
        if (std::find(m_resourceList.begin(), m_resourceList.end(), track) != m_resourceList.end())
            continue;

        m_nameId.push_back(id);
        m_nameResIdx.push_back(m_resourceList.size());
        m_resourceList.push_back(track);
        m_resourceTypeList.push_back((u32)Resource::ResourceType_AnimTrack);

        AABB trackAABB;
        // Also resources contained inside animation BEFORE the animation to prevent cook problems

        bbool aabbFound = AnimDependenciesMapResource::fillPathList( m_resourceList, m_resourceTypeList, fullName, dependencyPath, &trackAABB );

        if (!aabbFound)
            aabbFound = AnimDependenciesMapResource::fillPathList( m_resourceList, m_resourceTypeList, fullName, AnimationDependenciesMap::getDependenciesFile(_path, FILESERVER->getPlatform()), &trackAABB );

        ITF_WARNING_CATEGORY(Anim,NULL, aabbFound, "Animation %s: Invalid bounding box - please reload data with GenAnim to compute bounding box", StringToUTF8(_path).get());
        if (trackAABB.isValid())
        {
            if (m_animsAABB.isValid())
                m_animsAABB.grow(trackAABB);
            else
                m_animsAABB = trackAABB;
        }
    }
}

void SubAnimSet_Template::addBankInfoToSubAnimTemplate(SubAnim_Template *_subAnimTemplate, AnimTrack * track, const ITF_VECTOR<BankChangeResourceID> & _bankResources)
{
    ITF_VECTOR<BankChange_Template>::const_iterator bItTemplate = getBankChangeList().begin();
    for (u32 i=0; bItTemplate != getBankChangeList().end(); ++bItTemplate, ++i )
    {
        const BankChange_Template &bankChangeTemplate = *bItTemplate;

        if (!bankChangeTemplate.getFriendlyName().isValid() || bankChangeTemplate.getFriendlyName() == _subAnimTemplate->getFriendlyName())
        {
            AnimTrackExt *  trackExt    = &_subAnimTemplate->m_trackExt;
            NamesMap::Iterator nIter(track->m_nameToPatch);
            nIter.findKey(bankChangeTemplate.getBankName());
            if (!nIter.isEnd())
            {
                const BankChangeResourceID & bankRes = _bankResources[i];
                if (bankRes.m_textureResourceID.isValidResourceId() && bankRes.m_patchBankResourceID.isValidResourceId())
                {
                    i32 index = trackExt->m_stateToRedirect.find(bankChangeTemplate.getState());
                    if (index < 0)
                    {
                        index = trackExt->m_redirectStates.size();
                        trackExt->m_stateToRedirect.set(bankChangeTemplate.getState(), index);
                        BankRedirect emptyBank;
                        trackExt->m_redirectStates.push_back(emptyBank);
                    }

                    BankRedirect * redirect = &trackExt->m_redirectStates[index];

                    redirect->m_nameToPatch.setID(bankChangeTemplate.getBankName(), redirect->m_textureResourceIDList.size());
                    redirect->m_patchBankResourceIDList.push_back(bankRes.m_patchBankResourceID);
                    redirect->m_textureResourceIDList.push_back(bankRes.m_textureResourceID);

                    // we also add logical ref
                    RESOURCE_MANAGER->queryAddUserToResourceLogicalData(bankRes.m_patchBankResourceID);
                    RESOURCE_MANAGER->queryAddUserToResourceLogicalData(bankRes.m_textureResourceID);
                }
            }
        }
    }
}


bbool SubAnimSet_Template::resolveMarkers(const String & _animPath)
{
    m_markerResolved = btrue;

    // pre-process bank changes
    ITF_VECTOR<BankChangeResourceID> bankResources;
    bankResources.resize(getBankChangeList().size());

    ITF_VECTOR<BankChange_Template>::const_iterator bItTemplate = getBankChangeList().begin();
    ITF_VECTOR<BankChangeResourceID>::iterator      bItTemplateResource = bankResources.begin();
    Path pathBank;
    for (; bItTemplate != getBankChangeList().end(); ++bItTemplate, ++bItTemplateResource  )
    {
        const BankChange_Template &bankChangeTemplate = *bItTemplate;
        ResourceID  patchBankResourceID;
        ResourceID  textureResourceID;
        pathBank = bankChangeTemplate.getBankPath();
        pathBank.changeExtension(AnimationDependencies::PBK_EXT);

        if (!RESOURCE_MANAGER->getResourceIdFromFile(bItTemplateResource->m_patchBankResourceID, pathBank.getStringID(), Resource::ResourceType_AnimPatchBank) ||
            !RESOURCE_MANAGER->getResourceIdFromFile(bItTemplateResource->m_textureResourceID, bankChangeTemplate.getBankPath().getStringID(), Resource::ResourceType_Texture))
        {
            bItTemplateResource->m_patchBankResourceID.invalidateResourceId();
            bItTemplateResource->m_textureResourceID.invalidateResourceId();
        }
    }

    for (ITF_VECTOR<SubAnim_Template>::iterator it = m_subAnimList.begin(); it != m_subAnimList.end();++it)
    {            
        SubAnim_Template & subAnimTemplate = (*it);
        AnimTrack * track = subAnimTemplate.resolveMarkers(_animPath);
        if (track)
            addBankInfoToSubAnimTemplate(&subAnimTemplate, track, bankResources);
        else
            m_markerResolved = bfalse;
    }
    
    return m_markerResolved;
}

void SubAnim::setTemplate( const SubAnim_Template* _template )
{
    m_template = _template;
    SF_DEL(m_templateInternal);
}

void SubAnim::setSyncOffset( f32 _val )
{
    if (m_template->allowSyncOffset())
        m_syncOffset = _val;
}


SubAnimSet::SubAnimSet()
: m_template(NULL)
, m_addedSubAnimMarkerResolved(bfalse)
, m_syncOffset(0.f)
{
}

SubAnimSet::~SubAnimSet()
{
    clear();
}

void SubAnimSet::clear()
{
    //delete all anims if present :)
    for (ITF_VECTOR<SubAnim*>::iterator it = m_subAnimList.begin(); it != m_subAnimList.end();++it)
    {
        delete *it;
    }

    m_subAnimList.clear();

    if ( m_template )
    {
        // remove bank resources
        ITF_VECTOR<BankChange>::iterator bIt = m_bankChangeList.begin();
        ITF_VECTOR<BankChange_Template>::const_iterator bItTemplate = m_template->getBankChangeList().begin();
        ResourceGroup* grp = (ResourceGroup*)(m_resourceGroup.getResource());
        if (grp)
        {
            for (;bIt != m_bankChangeList.end() && bItTemplate != m_template->getBankChangeList().end(); ++bIt, ++bItTemplate )
            {
                BankChange &bankChange = *bIt;
                grp->removeResource(bankChange.m_patchBankResourceID);
                grp->removeResource(bankChange.m_textureResourceID);
            }
        }
        m_template = NULL;
    }

    m_bankChangeList.clear();
}

SubAnim* SubAnimSet::getSubAnim(const StringID &_subAnimName) const
{
    u32 subAnimIndex = getSubAnimIndex(_subAnimName);

    if ( subAnimIndex != -1 )
    {
        return m_subAnimList[subAnimIndex];
    }
    else
    {
        return NULL;
    }
}

i32 SubAnimSet::getSubAnimIndex(const StringID &_subAnimName) const
{
    NamesMap::Iterator itTemplate(m_template->getNameToSubAnim());
    itTemplate.findKey(_subAnimName);
    if (!itTemplate.isEnd() && itTemplate.second() < (i32)m_subAnimList.size())
    {
        return itTemplate.second();
    }

    NamesMap::Iterator it(m_nameToSubAnim);
    it.findKey(_subAnimName);
    if (!it.isEnd() && it.second() < (i32)m_subAnimList.size())
    {
        return it.second();
    }
    return -1;
}

f32 SubAnimSet::getAnimDuration(StringID _animName, bbool _full) const
{
    const SubAnim *anim = getSubAnim(_animName);
    if (anim)
        return _full ? anim->getTrack()->getDuration() : anim->getDuration();
    
    return 0.f;
}

f32 SubAnimSet::getTrackDuration(const String &_path) const
{
    AnimTrack *track = getTrack(FilePath::getFilenameWithoutExtension(_path));
    if (track)
        return track->getDuration();

    return 0.f;
}

void SubAnimSet::addSubAnimList(SubAnim* _subAnim, const String & path)
{
    u32 newPos = m_subAnimList.size();
    m_subAnimList.push_back(_subAnim);
    m_addedSubAnim.push_back(newPos);
    m_nameToSubAnim.setID(_subAnim->getFriendlyName(), m_nameToSubAnim.size());
    m_addedSubAnimMarkerResolved = bfalse;

    String dependencyFile = path;
    PathString_t animPathFilename;

    _subAnim->getAnimPath().getString(animPathFilename);

    dependencyFile       += animPathFilename;

    computeSubAnimAtPos(newPos, path, AnimationDependenciesMap::getDependenciesFile(dependencyFile, FILESERVER->getPlatform()));
}

void SubAnimSet::computeSubAnimAtPos( int _pos, const String & path, const Path & _dependencyFile )
{
    SubAnim *subAnim = m_subAnimList[_pos];

    const PathChar_t *animPathBasename = subAnim->getAnimPath().getBasename();
#ifdef ITF_DEBUG_STRINGID
    StringID id = FilePath::getFilenameWithoutExtension(animPathBasename);
#else
    u32 filenameWithoutExt_Len;
    const PathChar_t *lastDot = (const PathChar_t*)PATHSTRINGCHR((PathCharStdlib_t*)animPathBasename, PATHSTRING_T('.'));
    if (lastDot)
    {
        filenameWithoutExt_Len = u32(lastDot - animPathBasename);
    }
    else
    {
        filenameWithoutExt_Len = u32(PATHSTRINGLEN((PathCharStdlib_t*)animPathBasename));
    }
    StringID id = StringID::StrToCRC(animPathBasename, filenameWithoutExt_Len); ///IMPORTANT : THIS IS FOR OPTIMIZATION. IF YOU WANT TO CHANGE IT, CHECK WITH SEB
#endif
    PathChar_t fullName[PATH_C_BUFFERSIZE];
    PathChar_t partialName[PATH_C_BUFFERSIZE];
    char pathBuffer[512];
    path.charCopyToBuffer(pathBuffer, sizeof(pathBuffer));
    subAnim->getAnimPath().getString(partialName);
    Path::concatenateStrings(fullName, pathBuffer, partialName);
    
    ITF_WARNING(NULL, _dependencyFile.isEmpty() || AnimationDependenciesMap::getDependenciesFile(fullName, FILESERVER->getPlatform()) == _dependencyFile, "using dependency %s with file %s", _dependencyFile.getString8().cStr(), fullName);
    addTrack(id, fullName, _dependencyFile);
    //subAnim->setTrackName(id);    
}

//////////////////////////////////////////////////////////////////////////
void SubAnimSet::linkSubAnimToTrack(ResourceGroup *_resourceGroup, const Path & path) // used on serialization, after collecting animList data
//////////////////////////////////////////////////////////////////////////
{
    setParentResourceGroup(_resourceGroup);
    u32 nbSubAnims = m_subAnimList.size();
    
    if (nbSubAnims > 0)
    {
        // for dependencies, take first animation to have the path
        Path dependencyPath = Path::EmptyPath;
        const ITF_VECTOR<Path> & resourcePath = m_template->getResourceList();
        const ITF_VECTOR<u32> & resourceType = m_template->getResourceTypeList();
        if (resourcePath.size() && m_template->getAnimAABB().isValid())
        {
            ResourceGroup* grp = (ResourceGroup*)m_resourceGroup.getResource();
            ITF_VECTOR<Path>::const_iterator pathIt = resourcePath.begin();
            ITF_VECTOR<u32>::const_iterator pathTypeIt = resourceType.begin();
            
            SafeArray<ResourceID> arrayTab;
            arrayTab.resize(resourcePath.size());
            for (u32 i=0; pathIt != resourcePath.end(); ++pathIt, ++pathTypeIt, ++i)
            {
                ResourceID resID = RESOURCE_MANAGER->newResourceIDFromFile((Resource::ResourceType)*pathTypeIt, *pathIt);
                grp->addResource(resID.getResource(), bfalse, bfalse); // don't add logical data user, it's already done in newResourceIDFromFile
                arrayTab[i] = resID;
            }
            m_globalBoundingBox = m_template->getAnimAABB();
            
            const SafeArray<u32> &mapId     = m_template->getNameIdList();
            const SafeArray<i32> &mapResIdx = m_template->getNameResIdxList();
            
            u32 mapSize = mapId.size();
            for (u32 i=0; i<mapSize; i++)
            {
                m_nameToTrack.setID(mapId[i], 
                                    arrayTab[mapResIdx[i]].getResourceRef());
            }
        } else
        {
            String pathString;
            
            path.getString(pathString);

            String dependencyFile = pathString;
            PathString_t subAnimPathname;

            m_subAnimList[0]->getAnimPath().getString(subAnimPathname);

            dependencyFile       += subAnimPathname;
            dependencyPath        = AnimationDependenciesMap::getDependenciesFile(dependencyFile, FILESERVER->getPlatform());

            for (u32 idx = 0; idx<nbSubAnims; ++idx)
            {
                computeSubAnimAtPos( idx, pathString, dependencyPath);
            }
        }


    }
    
    // Add bank resources !!
    ITF_VECTOR<BankChange>::iterator bIt = m_bankChangeList.begin();
    ITF_VECTOR<BankChange_Template>::const_iterator bItTemplate = m_template->getBankChangeList().begin();
    const ITF_VECTOR<String8> & skipFiles = m_template->getSkipFiles();

    Path pathBank;
    String8  pathBankStr8;
    for (;bIt != m_bankChangeList.end() && bItTemplate != m_template->getBankChangeList().end(); ++bIt, ++bItTemplate )
    {
        BankChange &bankChange = *bIt;
        const BankChange_Template &bankChangeTemplate = *bItTemplate;
        pathBank = bankChangeTemplate.getBankPath();
        if (!skipFiles.empty())
        {
            pathBank.getString(pathBankStr8);
             if (std::find(skipFiles.begin(),skipFiles.end(), pathBankStr8) != skipFiles.end())
             {
                 continue; // skip it
             }
        }
        pathBank.changeExtension(AnimationDependencies::PBK_EXT);
        bankChange.m_patchBankResourceID = _resourceGroup->addResource(Resource::ResourceType_AnimPatchBank, pathBank);
        bankChange.m_textureResourceID   = _resourceGroup->addResource(Resource::ResourceType_Texture, bankChangeTemplate.getBankPath());
    }
    
    ResourceGroup* grp = (ResourceGroup*)m_resourceGroup.getResource();
    ITF_VECTOR<String8>::const_iterator sIt = skipFiles.begin();
    for (;sIt != skipFiles.end(); ++sIt)
    {
        pathBankStr8 = *sIt;
        FilePath::changeExtension(pathBankStr8, AnimationDependencies::PBK_EXT);
        ResourceID      resourceID;

        if (RESOURCE_MANAGER->getResourceIdFromFile(resourceID, pathBankStr8, Resource::ResourceType_Texture))
        {
            grp->removeResource(resourceID);
        }
        if (RESOURCE_MANAGER->getResourceIdFromFile(resourceID, *sIt, Resource::ResourceType_AnimPatchBank))
        {
            grp->removeResource(resourceID);
        }
    }    
}


void SubAnimSet::setTemplate( const class SubAnimSet_Template* _template )
{
    clear();

    m_template = _template;

    const ITF_VECTOR<SubAnim_Template>& subAnims = m_template->getSubAnimList();
    const ITF_VECTOR<BankChange_Template>& banksTemplate = m_template->getBankChangeList();

    for ( ITF_VECTOR<SubAnim_Template>::const_iterator itAnims = subAnims.begin(); itAnims != subAnims.end(); ++itAnims )
    {
        SubAnim* newSubAnim = newAlloc(mId_Animation,SubAnim);

        newSubAnim->setTemplate(&(*itAnims));
        m_subAnimList.push_back(newSubAnim);
    }

    m_bankChangeList.resize(banksTemplate.size());

    ITF_VECTOR<BankChange_Template>::const_iterator itBanksTemplate = banksTemplate.begin();
    ITF_VECTOR<BankChange>::iterator itBanks = m_bankChangeList.begin();

    for ( ; itBanksTemplate != banksTemplate.end() && itBanks != m_bankChangeList.end(); ++itBanksTemplate, ++itBanks )
    {
        (*itBanks).m_template = &(*itBanksTemplate);
    }
}


ITF::bbool SubAnimSet::isResolveMarkersOk()
{
    bbool resolved = m_template->markerResolded();
    if (m_addedSubAnim.size())
        resolved = resolved && m_addedSubAnimMarkerResolved;

    return resolved;
}

bbool SubAnimSet::resolveMarkers( const String & _path )
{
    if ( m_template->markerResolded())
        m_addedSubAnimMarkerResolved = btrue;
    else
        m_addedSubAnimMarkerResolved = ((SubAnimSet_Template*)m_template)->resolveMarkers(_path);
    
    u32 addedCount = m_addedSubAnim.size();
    for (u32 i=0; i < addedCount; i++)
    {
        m_addedSubAnimMarkerResolved = ((SubAnim_Template *)m_subAnimList[m_addedSubAnim[i]]->getTemplate())->resolveMarkers(_path) && m_addedSubAnimMarkerResolved;
    }
    return m_addedSubAnimMarkerResolved;
}

// apply a sync offset to all the subanims (offset is given by the owner AnimLightComponent)
void SubAnimSet::setGlobalSyncOffset( const f32 _syncOffset )
{
    if (f32_Abs(_syncOffset - m_syncOffset) < MTH_EPSILON)
        return;

    m_syncOffset = _syncOffset;
    ITF_VECTOR<SubAnim*>::iterator subAnimIter = m_subAnimList.begin();
    for (; subAnimIter != m_subAnimList.end(); ++subAnimIter)
    {
        (*subAnimIter)->setSyncOffset(_syncOffset);
    }
}


}
