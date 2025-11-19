#include "core/enginecommon.h"

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_ANIMATIONCOOKER_H_
#include "tools/plugins/CookerPlugin/AnimationCooker.h"
#endif //_ITF_ANIMATIONCOOKER_H_

#ifndef _ITF_ANIMATIONLOADSKELETON_H_
#include "tools/plugins/CookerPlugin/AnimLoader/AnimationLoadSkeleton.h"
#endif //_ITF_ANIMATIONLOADSKELETON_H_

#ifndef _ITF_ANIMATIONLOADPATCHBANK_H_
#include "tools/plugins/CookerPlugin/AnimLoader/AnimationLoadPatchBank.h"
#endif //_ITF_ANIMATIONLOADPATCHBANK_H_

#ifndef _ITF_ANIMATIONLOADPOLYLINE_H_
#include "tools/plugins/CookerPlugin/AnimLoader/AnimationLoadPolyline.h"
#endif //_ITF_ANIMATIONLOADPOLYLINE_H_

#ifndef _ITF_ANIMATIONLOADTRACK_H_
#include "tools/plugins/CookerPlugin/AnimLoader/AnimationLoadTrack.h"
#endif //_ITF_ANIMATIONLOADTRACK_H_

#ifndef _ITF_ANIMATIONRES_H_
#include "engine/animation/AnimationRes.h"
#endif //_ITF_ANIMATIONRES_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_XMLALL_H_
#include "engine/XML/XML_All.h"
#endif //_ITF_XMLALL_H_

#ifndef _ITF_VERSIONING_H_
#include "core/versioning.h"
#endif  //_ITF_VERSIONING_H_

#ifndef _ITF_ANIMATIONSKELETON_H_
#include    "engine/animation/AnimationSkeleton.h"
#endif // _ITF_ANIMATIONSKELETON_H_

#ifndef _ITF_ANIMATIONPATCHBANK_H_
#include    "engine/animation/AnimationPatchBank.h"
#endif // _ITF_ANIMATIONPATCHBANK_H_

#ifndef _ITF_ANIMATIONTRACK_H_
#include    "engine/animation/AnimationTrack.h"
#endif // _ITF_ANIMATIONTRACK_H_

#ifndef _ITF_ANIMATIONPOLYLINE_H_
#include    "engine/animation/AnimationPolyline.h"
#endif // _ITF_ANIMATIONPOLYLINE_H_

#ifndef _ITF_ANIMATIONDEPENDENCIES_H_
#include "engine/animation/AnimationDependencies.h"
#endif //_ITF_ANIMATIONDEPENDENCIES_H_

#ifndef _ITF_COOKERFACTORY_H_
#include "tools/plugins/CookerPlugin/CookerFactory.h"
#endif //_ITF_COOKERFACTORY_H_

#ifndef _ITF_FILEMANAGER_H_
#include "core/AdaptersInterfaces/FileManager.h"
#endif //_ITF_FILEMANAGER_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_DEP_COLLECTOR_H_
#include "engine/dependencies/DepCollector.h"
#endif //_ITF_DEP_COLLECTOR_H_

namespace ITF
{

bbool animSkeletonCooker::cookFile(const String& _szPlatform,const String& _szFilename)
{
    return animSkeletonCooker::cookForced( _szPlatform, _szFilename, bfalse, bfalse);
}

bbool animSkeletonCooker::cook(const String& _szPlatform,const String& _szFilename, bbool _bRaiseError)
{
    return animSkeletonCooker::cookForced( _szPlatform, _szFilename, _bRaiseError, bfalse);
}

bbool animSkeletonCooker::cookForced(const String& _szPlatform,const String& _szFilename, bbool _bRaiseError, bbool _forceCook)
{
    String normalizedPath   = FilePath::normalizePath(_szFilename);
    String cookFile         = FILESERVER->getCookedNamePlatform(_szFilename,_szPlatform);

    String result;

	bbool bResult = btrue;
    result.setTextFormat("[animSkeletonCooker] Cook file animation skeleton... %S", _szFilename.cStr());
    if (cookerFactory::fileNeedsCook(normalizedPath, cookFile,_szPlatform,Versioning::animSkeletonVer()) || _forceCook)
    {
        AnimSkeleton *animSkeleton = AnimationLoadSkeleton::readAnimSkeleton(_szFilename,_szPlatform,_bRaiseError,btrue);

        if (!animSkeleton)
        {
            result+= " FAILED ";
            result+= _szFilename;
            ITF_ERROR_SHOW(_bRaiseError, StringConverter(result).getChar());
			bResult = bfalse;
        }
        else
        {
            result += " COOKED";
            SF_DEL(animSkeleton);
            FILEMANAGER->flushTimeWriteAccess(cookFile, FILEMANAGER->getLastTimeWriteAccess(normalizedPath));
        }

        LOG_COOKER("[ANIMATION_COOKER] %ls",result.cStr());
        return bResult;
    }

    return bResult;
}


bbool animPatchBankCooker::cookFile(const String& _szPlatform,const String& _szFilename)
{
    return animPatchBankCooker::cookForced(_szPlatform,_szFilename, bfalse, bfalse);
}

bbool animPatchBankCooker::cook(const String& _szPlatform,const String& _szFilename, bbool _bRaiseError)
{
    return animPatchBankCooker::cookForced(_szPlatform,_szFilename, _bRaiseError, bfalse);
}

bbool animPatchBankCooker::cookForced(const String& _szPlatform,const String& _szFilename, bbool _bRaiseError, bbool _forceCook)
{
    String normalizedPath   = FilePath::normalizePath(_szFilename);
    String cookFile         = FILESERVER->getCookedNamePlatform(_szFilename,_szPlatform);

    String result;

    bbool bresult = btrue;
    result.setTextFormat("%s %S","[animPatchBankCooker] Cook file animation patch bank... ", _szFilename.cStr());
    if (cookerFactory::fileNeedsCook(normalizedPath, cookFile, _szPlatform,Versioning::animPatchBankVer()) || _forceCook)
    {
        AnimPatchBank * pBank = AnimationLoadPatchBank::readAnimPatchBank(_szFilename,_szPlatform,_bRaiseError,btrue);
        if (!pBank)
        {
            result+= " FAILED ";
            result+= _szFilename;
            ITF_ERROR_SHOW(_bRaiseError, StringConverter(result).getChar());
            bresult =bfalse;
        }
        else
        {
            result += " COOKED";
            SF_DEL(pBank);
            FILEMANAGER->flushTimeWriteAccess(cookFile, FILEMANAGER->getLastTimeWriteAccess(normalizedPath));
        }

         LOG_COOKER("[ANIMATION_COOKER] %ls",result.cStr());
    }

    return bresult;
}


bbool animTrackCooker::cookFile(const String& _szPlatform,const String& _szFilename)
{
    return animTrackCooker::cookForced(_szPlatform,_szFilename, bfalse, bfalse);
}

bbool animTrackCooker::cook(const String& _szPlatform,const String& _szFilename, bbool _bRaiseError)
{
    return animTrackCooker::cookForced(_szPlatform,_szFilename, _bRaiseError, bfalse);
}

bbool animTrackCooker::cookForced(const String& _szPlatform,const String& _szFilename, bbool _bRaiseError, bbool _forceCook)
{
    String normalizedPath   = FilePath::normalizePath(_szFilename);
    if (!FILEMANAGER->fileExists(normalizedPath))
    {
        LOG_COOKER("[ANIMATION_COOKER] NOT FOUND %ls",normalizedPath.cStr());
        return false;
    }
    String cookFile         = FILESERVER->getCookedNamePlatform(_szFilename,_szPlatform);
    Path   dependencyFile   = AnimationDependenciesMap::getDependenciesFile(_szFilename,_szPlatform);

    String  result;
	bbool bResult = btrue;
    bbool isMainThread = (Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID);

    AnimationDependenciesMap * mapDep   = NULL;
    AnimationDependencies    * dep      = NULL;

    result.setTextFormat("%s %S","[animTrackCooker] Cook file animation track... ", _szFilename.cStr());

    bbool needRecook = AnimationLoadTrack::fillDependencies(mapDep, dep, _szFilename, dependencyFile, NULL) && cookerFactory::fileNeedsCook(normalizedPath, cookFile,_szPlatform,Versioning::animTrackVer()) || _forceCook;

    AnimTrack                * trk      = NULL;
    if (LOGICDATABASE && LOGICDATABASE->isBinarisationEnable() && !needRecook)
    {
        trk = newAlloc(mId_Animation, AnimTrack);
        needRecook = !trk->openBinary(cookFile, bfalse);
        if (!needRecook)
            needRecook = trk->needRecook();
        SF_DEL(trk);
    }

    if (needRecook)
    {
        // first update dependencies
        {
            String        skeletonPath      = dep->getSkeletonPath(mapDep->getPathMap());
            AnimSkeleton *skeleton          = NULL;
            bbool         needSkeletonDelete= bfalse;

            if (!skeletonPath.isEmpty())
            {
                if (RESOURCE_MANAGER)
                {
                    // use cook factory resource group, if present
                    ResourceGroup* grp = (ResourceGroup*)ResourceManager::m_cookingGroupID.getResource();
                    if (grp)
                    {
                        ResourceID resID = RESOURCE_MANAGER->newResourceIDFromFile(Resource::ResourceType_AnimSkeleton, skeletonPath);
                        if (isMainThread)
                        {
                            if (!grp->addResource(resID.getResource(), bfalse))
                            {
                                RESOURCE_MANAGER->queryRemoveUserToResourceLogicalData(resID);
                            }
                        }
                        else
                        {
                            RESOURCE_MANAGER->queryAddResourceToGroup( grp->getID(), resID );
                        }
                        skeleton            = (static_cast<AnimSkeletonResource*>(resID.getResource()))->getOwnerData();
                        //ITF_ASSERT(skeleton);
                    }
                }

                if (!skeleton)
                {
                    // load skeleton
                    skeleton = newAlloc(mId_Animation, AnimSkeleton);
                    if (skeleton->loadFile(skeletonPath,_szPlatform, btrue, btrue))
                    {
                        needSkeletonDelete = btrue;
                    } else
                    {
                        SF_DEL(skeleton);
                    }
                }

                trk = AnimationLoadTrack::readAnimTrack(skeleton, _szFilename,_szPlatform,_bRaiseError, btrue);

                if (needSkeletonDelete)
                {
                    SF_DEL(skeleton);
                }
            }
        }

        if (!trk)
        {
            result+= " FAILED ";
            result+= _szFilename;
            ITF_ERROR_SHOW(_bRaiseError, StringConverter(result).getChar());
			bResult = bfalse;
        }
        else
        {
            result += " COOKED";
            FILEMANAGER->flushTimeWriteAccess(cookFile, FILEMANAGER->getLastTimeWriteAccess(normalizedPath));
        }
        SF_DEL(trk);

        LOG_COOKER("[ANIMATION_COOKER] %ls",result.cStr());
        return bResult;
    }

    return bResult;
}

static void resolvePatchDep( const DepCollection& _vPatchDep, DepCollection& dependencies )
{
    typedef ITF_MAP< String, DepCollection::Collection > RedirectCollection;

    if ( !_vPatchDep.size() )
        return;

    DepCollection::Collection::const_iterator   patchEnd = _vPatchDep.end();
    RedirectCollection                          redirects;

    for ( DepCollection::const_iterator patchIt = _vPatchDep.begin(); patchIt != patchEnd; ++patchIt )
        redirects[FilePath::getDirectory( patchIt->path )].insert( *patchIt );

    RedirectCollection::iterator    redEnd = redirects.end();
    PbkRedirect                     pbkRedirect;
    String                          redirectPatchBank;

    for ( RedirectCollection::iterator redIt = redirects.begin(); redIt != redEnd; ++redIt )
    {
        pbkRedirect.clear();
        redirectPatchBank = redIt->first + AnimTrack::s_redirectPatchBank;
        patchEnd = redIt->second.end();
        if ( !pbkRedirect.openBinary( redirectPatchBank, bfalse ) )
        {
            for ( DepCollection::iterator patchIt = redIt->second.begin(); patchIt != patchEnd; ++patchIt )
            {
                dependencies.add( patchIt->path );
                dependencies.add( FilePath::getDirectory( patchIt->path ) + FilePath::getFilenameWithoutExtension( patchIt->path ) + AnimationDependencies::PBK_EXT );
            }
            continue;
        }
        for ( DepCollection::iterator patchIt = redIt->second.begin(); patchIt != patchEnd; ++patchIt )
        {
            if ( !pbkRedirect.findPatchBank( patchIt->path, redirectPatchBank ) )
            {
                dependencies.add( patchIt->path );
                dependencies.add( FilePath::getDirectory( patchIt->path ) + FilePath::getFilenameWithoutExtension( patchIt->path ) + AnimationDependencies::PBK_EXT );
                continue;
            }
            dependencies.add( redirectPatchBank );
            dependencies.add( FilePath::getDirectory( redirectPatchBank ) + FilePath::getFilenameWithoutExtension( redirectPatchBank ) + AnimationDependencies::PBK_EXT );
        }
    }
}

bbool animTrackCooker::dependencyFiles( const String& cacheKey, const String& _vPlatform,const String& _szFilename,DepCollection& _vDependencies )
{
    if ( _vDependencies.load( cacheKey, _szFilename, _vPlatform ) )
    {
        _vDependencies.filterExt();
        return true;
    }

    DepCollection   patchDep;
    PugiXMLWrap     XML;

    if ( !XML.openFile( _szFilename, bfalse, PugiXMLWrap::DoNothing ) )
        return bfalse;


    String skeleton =  XML.readAttribute("Squeleton/value", "");
    _vDependencies.add( skeleton );

    if ( XML.enterElement( "PatchBankList" ) && XML.enterFirstElement() )
        do
        {
            patchDep.add( XML.readAttribute( "value", "" ) );
        } while ( XML.nextElement() );

    resolvePatchDep( patchDep, _vDependencies );

    String animDep( COOKED_DIRNAME );

    animDep += _vPlatform;
    animDep += '/';
    animDep += FilePath::getDirectory( _szFilename );
    animDep += AnimationDependenciesMap::ANIMATION_DEPENDENCIES;
    _vDependencies.add( animDep );
    _vDependencies.save( cacheKey, _szFilename, _vPlatform );
    _vDependencies.filterExt();
    return btrue;
}

};
