#include "precompiled_engine.h"

#ifndef _ITF_UVATLAS_H_
#include "engine/display/UVAtlas.h"
#endif //_ITF_UVATLAS_H_

#ifndef _ITF_LUAHANDLER_H_
#include "core/script/LUAHandler.h"
#endif //_ITF_LUAHANDLER_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_FILEPACKFORMAT_H_
#include "engine/file/FilePackFormat.h"
#endif //_ITF_FILEPACKFORMAT_H_

#ifndef _ITF_VERSIONING_H_
#include "core/versioning.h"
#endif //_ITF_VERSIONING_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef ITF_FINAL
# ifndef _ITF_FILE_MANAGER_LOGGER_ITF_H_
#  include "adapters/FileManager_ITF/FileManagerLogger_ITF.h"
# endif //_ITF_FILE_MANAGER_LOGGER_ITF_H_
#endif

namespace ITF
{

    const UVdata& UVAtlas::getUVDatabyIndex(i32 _index) const
    {
        ITF_MAP<i32, UVdata>::const_iterator iter = m_uvMap.find(_index);
        ITF_ASSERT(iter != m_uvMap.end());
        return iter->second;
    }

    const UVdata& UVAtlas::getUVDataAt(u32 _n) const
    {
        return m_uvMap.find(_n)->second;
    }

    /// 0 -- 3
    /// |    |
    /// 1 -- 2
    void UVAtlas::get4UVbyIndex(i32 _index, Vec2d *m_UV) const
    {
        UVdata data;
        ITF_MAP<i32, UVdata>::const_iterator iter = m_uvMap.find(_index);
        if (iter != m_uvMap.end())
        {
            data = iter->second;
        }

        m_UV[0] = data.getUV0();
        m_UV[1] = Vec2d(data.getUV0().m_x, data.getUV1().m_y);
        m_UV[2] = data.getUV1();
        m_UV[3] = Vec2d(data.getUV1().m_x, data.getUV0().m_y);
    }

    /// 0 -- 3
    /// |    |
    /// 1 -- 2
    void UVAtlas::get4UVAt(i32 _n, Vec2d *m_UV) const
    {
        const UVdata& data = m_uvMap.find(_n)->second;
        m_UV[0] = data.getUV0();
        m_UV[1] = Vec2d(data.getUV0().m_x, data.getUV1().m_y);
        m_UV[2] = data.getUV1();
        m_UV[3] = Vec2d(data.getUV1().m_x, data.getUV0().m_y);
    }
   
    void UVAtlas::addUVdata(i32 _index, UVdata _data)
    {
        m_uvMap[_index] = _data;
    }

    void UVAtlas::removeUVdataAt(i32 _n)
    {
        ITF_MAP<i32, UVdata>::iterator iter = m_uvMap.find(_n);
        if (iter != m_uvMap.end())
        {
            m_uvMap.erase(iter);
        }
    }

    void UVAtlas::removeUVdataByIndex(i32 _index)
    {
        ITF_MAP<i32, UVdata>::iterator itmap = m_uvMap.find(_index);
        ITF_ASSERT(itmap != m_uvMap.end());
        m_uvMap.erase(itmap);
    }

    void UVAtlas::deleteAllUVdata()
    {
        m_uvMap.clear();
    }

	void UVAtlas::serialize(ArchiveMemory& _pArchive)
	{
		ITF_VERIFY(Versioning::serializeVersion(_pArchive,Versioning::atlasVer()));
		m_uvMap.serialize(_pArchive);
	}


	bbool UVAtlas::openFile(UVAtlas& _atlas,const String& _path)
	{
		File* atlasFile = FILEMANAGER->openFile(_path,ITF_FILE_ATTR_READ);

		if (atlasFile)
		{
			u32 fileSize = (u32)atlasFile->getLength();
			u8 * readBuffer = newAlloc(mId_Temporary,u8[fileSize+1]);
			u32 sizeRead = 0;
			bbool rdValid = atlasFile->read(readBuffer,fileSize,&sizeRead);
			
            if (rdValid)
            {
                ITF_ASSERT(fileSize == sizeRead);
			    readBuffer[fileSize] = '\0';
			    FILEMANAGER->closeFile(atlasFile);
			
			    ArchiveMemory rd(readBuffer, fileSize);
			    _atlas.serialize(rd);
            }
            else
            {
                SF_DEL_ARRAY(readBuffer);
                return 0;
            }

			SF_DEL_ARRAY(readBuffer);
		}
		else
		{
			return 0;
		}

		
		_atlas.setName(FilePath::getFilenameWithoutExtension(_path));
		
		return 1;
	}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void UVAtlasManager::getCookedNamePlatform(const String& _src, const String& _platform,String& _atlName,String& _cookedname)
    {
        String pathCopy = FilePath::getDirectory(_src);
        String name = FilePath::getFilenameWithoutExtension(_src);

        name += "_ATL.atl";
        pathCopy += name;

        _atlName = pathCopy;
        _cookedname = FILESERVER->getCookedNamePlatform(pathCopy,_platform,btrue);
    }

//------------------------------------------------------------------------------------------------------------------------------------------------

    void UVAtlasManager::loadDefault()
    {
#ifdef ITF_CONSOLE
        bbool bRemote = FILEMANAGER->isRemote();

        m_bContainerAccess = !bRemote;
#ifndef ITF_FINAL
        if ( bRemote )
        {
            // even if the atlascontainer is not used in remote, do a dummy opened log to have a reference
            String CookedName( FILESERVER->getCookedName("atlascontainer"));

            FILEMANAGER->getLogger().LogOpenedFile(CookedName);
        }
#endif //ITF_FINAL
#else
        m_bContainerAccess = bfalse;
#endif //ITF_CONSOLE

        if (m_bContainerAccess)
        {
            String CookedName(FILESERVER->getCookedName("atlascontainer"));
            load(CookedName);
        }
      
    }

//------------------------------------------------------------------------------------------------------------------------------------------------

    const UVAtlas* UVAtlasManager::getAtlas(const String& filename)
    {
   
        if (m_bContainerAccess)
        {
            UVAtlasKey::iterator iter =  m_atlasKey.find(filename);
            if (iter!=m_atlasKey.end())
                return  &(*iter).second;

            return NULL;

        }
        else
        {
            String atlName;
            String cookedFile;

            getCookedNamePlatform(filename,FILESERVER->getPlatform(),atlName,cookedFile);

            if (FILEMANAGER->fileExists(atlName))
            {
                bbool cookNeeded = RESOURCE_MANAGER->cookNeeded(atlName);

                if (cookNeeded)
                {
                    RESOURCE_MANAGER->requestCooking(atlName);
                }
            }

            bbool exists = FILEMANAGER->fileExists(cookedFile);

            if (exists)
            {
                UVAtlas atlas;
                if (UVAtlas::openFile(atlas,cookedFile))
                {
                    UVAtlasKey::iterator iter =  m_atlasKey.insert(m_atlasKey.begin(),std::pair<StringID,UVAtlas>(filename,atlas));
                    return &(*iter).second;

                }

                return NULL;
            }
        }
  

        return NULL;
    }

//------------------------------------------------------------------------------------------------------------------------------------------------

    void UVAtlasManager::serialize(ArchiveMemory& _archive,UVAtlasKey& _atlasMap)
    { 
        u32 version = 0;

        _archive.serialize(version);
        _atlasMap.serialize(_archive);
    }

//------------------------------------------------------------------------------------------------------------------------------------------------

    void UVAtlasManager::load(const String& filename)
    {
        File* pFile = FILEMANAGER->openFile(filename,ITF_FILE_ATTR_READ);
        ITF_ASSERT_MSG(pFile,"the UVAtlasManager file is missing %s", StringToUTF8(filename).get());
        if (pFile)
        {
            u32 lenght = (u32) pFile->getLength();
            u8* pBuffer = newAlloc(mId_Temporary,u8[lenght]);

            pFile->read(pBuffer,lenght);
            FILEMANAGER->closeFile(pFile);
            pFile = NULL;

            ArchiveMemory rd(pBuffer,lenght);
            SF_DEL_ARRAY(pBuffer);

            serialize(rd,m_atlasKey);
        }

    }

//------------------------------------------------------------------------------------------------------------------------------------------------

#ifdef ITF_WINDOWS
    void UVAtlasManager::save(const String& _filenameCooked,UVAtlasKey& _atlasMap)
    {
        ArchiveMemory arch;

        serialize(arch,_atlasMap);

        File* writeFile = FILEMANAGER->openFile(_filenameCooked,ITF_FILE_ATTR_WRITE|ITF_FILE_ATTR_CREATE_NEW);

        if (writeFile)
        {
            writeFile->write(arch.getData(),arch.getSize());
            FILEMANAGER->closeFile(writeFile);
            writeFile = NULL;
        }

    }

//------------------------------------------------------------------------------------------------------------------------------------------------

    void UVAtlasManager::build(const String& _filename,const String& _platform,const ITF_VECTOR<String>& _filesTexture)
    {
        UVAtlasKey localAtlasKey;

        String atlName;
        String cookedFile;

        for (ITF_VECTOR<String> ::const_iterator iter= _filesTexture.begin();iter!=_filesTexture.end();iter++)
        {
            UVAtlas atlas;
        
            getCookedNamePlatform(*iter,_platform,atlName,cookedFile);
            if (UVAtlas::openFile(atlas,cookedFile))
            {
                //m_atlasKey.insert()
                localAtlasKey[*iter] = atlas;
            }

        }

        save(_filename,localAtlasKey);

    }

#endif //ITF_WINDOWS


//------------------------------------------------------------------------------------------------------------------------------------------------
    
 }
