#include "precompiled_engine.h"

#ifndef _ITF_ANIMATIONDEPENDENCIES_H_
#include "engine/animation/AnimationDependencies.h"
#endif //_ITF_ANIMATIONDEPENDENCIES_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif // _ITF_FILESERVER_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_VERSIONING_H_
#include "core/versioning.h"
#endif  //_ITF_VERSIONING_H_

namespace ITF
{



    void IdPathFilename::serialize( ArchiveMemory& _pArchive )
    {
        m_idPath.serialize(_pArchive);
        m_filename.serialize(_pArchive);
    }

    ITF::String IdPathFilename::getFullPath( MapPath * _pathMap )
    {
        String result;
        getFullPath(result, _pathMap);
        return result;
    }

    void  IdPathFilename::getFullPath(ITF::String & _strResult, MapPath * _pathMap)
    {
        MapPath::iterator pathMapIter = _pathMap->find(m_idPath);
        if (pathMapIter == _pathMap->end())
        {
            _strResult = String::emptyString;
        }
        else
        {
            _strResult = pathMapIter->second;
            _strResult += m_filename;
        }
    }

    void IdPathFilename::setFullPath( MapPath * _pathMap, const String & _path )
    {
        m_idPath    = StringID(FilePath::getDirectory(_path));
        m_filename  = FilePath::getFilename(_path);

        MapPath::iterator pathMapIter = _pathMap->find(m_idPath);
        if (pathMapIter == _pathMap->end())
        {
            (*_pathMap)[m_idPath] = FilePath::getDirectory(_path);
        }
    }


    void AnimationDependencies::serialize( ArchiveMemory& _pArchive )
    {
        _pArchive.serialize(m_fileTime);
        m_skeleton.serialize(_pArchive);
        m_textureList.serialize(_pArchive);
        m_packBankList.serialize(_pArchive);
        ITF_ASSERT(m_textureList.size() == m_packBankList.size());
        m_bounding.serialize(_pArchive);
        _pArchive.serialize(m_boundingFound);
    }

    const char * AnimationDependencies::PBK_EXT = ".pbk";

    void AnimationDependencies::processPatchBankData()
    {
        m_packBankList.resize(m_textureList.size());
        for (u32 i=0; i<m_textureList.size(); i++)
        {
            m_packBankList[i] = IdPathFilename(m_textureList[i].getIdPath(),
                FilePath::getFilenameWithoutExtension(m_textureList[i].getFilename()) + PBK_EXT);
        }
    }

    const String  AnimationDependenciesMap::ANIMATION_DEPENDENCIES("animdependencies.dep");

    Path AnimationDependenciesMap::getDependenciesFile( const String & _path, const String & _platform )
    {
        return Path(FilePath::getDirectory(FILESERVER->getCookedNamePlatform(_path,_platform)) + ANIMATION_DEPENDENCIES);
    }

    void AnimationDependenciesMap::serialize( ArchiveMemory& _pArchive , bbool )
    {
        if (_pArchive.isReading())
        {
            _pArchive.serialize(m_animVersion);

            if (m_animVersion == getVersion())
            {
                u32 nbAnims = 0;
                _pArchive.serialize(nbAnims);
                for (u32 i=0; i<nbAnims; i++)
                {
                    StringID animStringId;
                    AnimationDependencies animDependencies;

                    animStringId.serialize(_pArchive);
                    animDependencies.serialize(_pArchive);

                    m_dependencyMap[animStringId] = animDependencies;
                }

                u32 nbPath = 0;
                _pArchive.serialize(nbPath);
                for (u32 i=0; i<nbPath; i++)
                {
                    StringID    pathId;
                    String      pathStr;

                    pathId.serialize(_pArchive);
                    pathStr.serialize(_pArchive);

                    m_pathMap[pathId] = pathStr;
                }

            }
        } else
        {
            m_animVersion = getVersion();
            _pArchive.serialize(m_animVersion);

            u32 nbAnims = m_dependencyMap.size();
            _pArchive.serialize(nbAnims);
            for (MapDependencies::iterator mapIdx = m_dependencyMap.begin();
                mapIdx != m_dependencyMap.end(); mapIdx++)
            {
                StringID    pathId = mapIdx->first;
                pathId.serialize(_pArchive);
                mapIdx->second.serialize(_pArchive);
            }

            u32 nbPath = m_pathMap.size();
            _pArchive.serialize(nbPath);
            for (MapPath::iterator mapIdx = m_pathMap.begin();
                mapIdx != m_pathMap.end(); mapIdx++)
            {
                StringID    pathId = mapIdx->first;
                pathId.serialize(_pArchive);
                mapIdx->second.serialize(_pArchive);
            }
        }
    }

    computeDependencies AnimationDependenciesMap::s_computeDependenciesCallback = NULL;

    u32 AnimationDependenciesMap::getVersion()
    {
        return Versioning::animTrackVer() + Versioning::animDependeniesVer();
    }
};
