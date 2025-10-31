#ifndef _ITF_ANIMATIONDEPENDENCIES_H_
#define _ITF_ANIMATIONDEPENDENCIES_H_

#ifndef _ITF_KEYARRAY_H_
#include "core/container/KeyArray.h"
#endif //_ITF_KEYARRAY_H_

#ifndef _ITF_ANIMATION_SERIALIZE_H_
#include "engine/animation/AnimationSerialize.h"
#endif //_ITF_ANIMATION_SERIALIZE_H_

namespace ITF
{

    typedef ITF_MAP<StringID, String> MapPath;

    class IdPathFilename 
    {
    public:
        IdPathFilename()
        {}


        IdPathFilename(StringID _idPath, const String & _filename)
            : m_idPath(_idPath), m_filename(_filename)
        { }



        void            serialize(ArchiveMemory& _pArchive);

        String          getFullPath(MapPath * _pathMap);
        void            getFullPath(String & _str, MapPath * _pathMap);
        void            setFullPath(MapPath * _pathMap, const String & _path);

        StringID        getIdPath()     { return m_idPath; }
        String          getFilename()   { return m_filename; }

    private:
        StringID        m_idPath;
        String          m_filename;
    };


    class AnimationDependencies 
    {
    public:
        AnimationDependencies()
            : m_fileTime(0)
            , m_boundingFound(bfalse)
        { m_bounding.invalidate(); }

        void                    serialize(ArchiveMemory& _pArchive);
        void                    processPatchBankData();

        u64                     getFileTime() { return m_fileTime; }
        void                    setFileTime(u64 _fileTime) { m_fileTime = _fileTime; }

        String                  getSkeletonPath(MapPath * _pathMap)                         { return m_skeleton.getFullPath(_pathMap); }
        void                    getSkeletonPath(String & _strSkel, MapPath * _pathMap)      { m_skeleton.getFullPath(_strSkel, _pathMap); }
        void                    setSkeletonPath(MapPath * _pathMap, const String & _path)   { m_skeleton.setFullPath(_pathMap, _path); }

        const AABB &            getBoundingBox() { return m_bounding; }
        void                    setBoundingBox(const AABB & _aabb) { m_bounding = _aabb; }

        u32                     getTextureCount() { return m_textureList.size(); }

        String                  getTextureAt(MapPath * _pathMap, u32 _index)                { return m_textureList[_index].getFullPath(_pathMap); }
        void                    getTextureAt(String & _strTexture, MapPath * _pathMap, u32 _index) { m_textureList[_index].getFullPath(_strTexture, _pathMap); }
        void                    pushTexture (MapPath * _pathMap, const String & _path)      { IdPathFilename idPath; idPath.setFullPath(_pathMap, _path);  m_textureList.push_back(idPath); }

        String                  getPatchBankAt(MapPath * _pathMap, u32 _index)              { return m_packBankList[_index].getFullPath(_pathMap); }
        void                    getPatchBankAt(String & _strPatchBank, MapPath * _pathMap, u32 _index) { m_packBankList[_index].getFullPath(_strPatchBank, _pathMap); }
        void                    pushPatchBank (MapPath * _pathMap, const String & _path)    { IdPathFilename idPath; idPath.setFullPath(_pathMap, _path);  m_packBankList.push_back(idPath); }

        bbool                   isBoundingFound() const { return m_boundingFound; }
        void                    setBoundingFound(bbool _boundingFound) { m_boundingFound = _boundingFound; }

        static const char * PBK_EXT;

    private:
        // stored - version number is stocked for ALL tracks
        u64                         m_fileTime;
        IdPathFilename              m_skeleton;
        ITF_VECTOR<IdPathFilename>  m_textureList;

        // computed
        ITF_VECTOR<IdPathFilename>  m_packBankList;

        // AABB
        bbool                       m_boundingFound;
        AABB                        m_bounding;
    };


    typedef ITF_MAP<StringID, AnimationDependencies> MapDependencies;
    typedef bbool (*computeDependencies)(class AnimationDependenciesMap * & /*_mapDep*/,
    class AnimationDependencies * & /*_dep*/, 
        const String & /*_anim*/, const Path & /*_dependencyFile*/, ResourceGroup * /*_group*/);


    class AnimationDependenciesMap : public AnimationSerialize
    {
    public:
        static Path                         getDependenciesFile( const String & _path, const String & _platform);
        static u32                          getVersion();

        void                                serialize(ArchiveMemory& _pArchive, bbool _compressedData=bfalse);
        virtual void                        clear() {};

        u32                                 getAnimVersion() { return m_animVersion; }
        MapPath *                           getPathMap() { return &m_pathMap; }
        MapDependencies &                   getMap() { return m_dependencyMap; }

        AnimationDependenciesMap()
            : m_animVersion(0)
        {}

        static computeDependencies           s_computeDependenciesCallback;
        static const String                  ANIMATION_DEPENDENCIES;

    private:
        u32                     m_animVersion;
        MapPath                 m_pathMap;
        MapDependencies         m_dependencyMap;
    };


};

#endif //_ITF_ANIMATIONDEPENDENCIES_H_