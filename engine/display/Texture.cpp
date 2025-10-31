#include "precompiled_engine.h"

#ifndef _ITF_TEXTURE_H_
#include "engine/display/Texture.h"
#endif //_ITF_TEXTURE_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_UVATLAS_H_
#include "engine/display/UVAtlas.h"
#endif //_ITF_UVATLAS_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

namespace ITF
{
IMPLEMENT_OBJECT_RTTI(Texture)

    bool PrefetchTexture( const Path& path, PrefetchFactory::Params& params )
    {
        return ResourceManager::PrefetchResource( path, params, Resource::ResourceType_Texture );
    }

    Texture::Texture( const Path &path ) 
    : Resource(path)
#ifndef ITF_WII
    , m_alpha(0)
#endif    
    , m_adapterimplementationData(NULL)
    , m_sizeX(0)
    , m_sizeY(0)
    , m_datasizeX(0)
    , m_datasizeY(0)
    , m_uvAtlas(0)
    , m_ratio(1.0f)
#ifndef ITF_WII    
    , m_fileContent(NULL)
    , m_fileContentSize(0)
#endif    
    , m_createdGFXObject(bfalse)
    {

#ifdef  MODE_HRESTEXTURE_SUPPORTED
        if (FILESERVER->hasHResEnable())
        {
            String pathStr = path.getString();
            const String& hdFileName = FILESERVER->getHResVersionName(pathStr);
            m_HResVersion = FILEMANAGER->fileExists(hdFileName);//override the texture path name

            if (m_HResVersion)
            {
                bbool res = RESOURCE_MANAGER->cookHResNeeded(pathStr);
                if (res)
                    RESOURCE_MANAGER->cookHRes(pathStr);
            }
        }
        else
        {
            m_HResVersion = bfalse;
        }
#endif //MODE_HRESTEXTURE_SUPPORTED
#ifndef ITF_FINAL
        m_dbgNfo[0]=0;
#endif // ITF_FINAL
    }

    Texture::~Texture()
    {
    }

  

    void    Texture::postCreate()
    {
         createAtlasIfExist();
    };

    void Texture::createAtlasIfExist()
    {
        PathString_t atlasFilename;

        getPath().getString(atlasFilename);

        m_uvAtlas = UVATLASMANAGER->getAtlas(atlasFilename);
    }

    void Texture::onResourceProcessingFromMainThread()
    {
#if defined(ITF_CTR)
        if (m_fileContent && !m_createdGFXObject)
        {
            GFX_ADAPTER->loadFromFileInMemory(this, m_fileContent, m_fileContentSize);
			SF_DEL_ARRAY(m_fileContent);
			m_fileContentSize = 0;
            m_createdGFXObject = btrue;
        }
#endif
    }


    void Texture::flushPhysicalData()
    {
#ifdef ITF_WII
        if(m_adapterimplementationData)
            GFX_ADAPTER->syncGPU();
#endif
        GFX_ADAPTER->cleanupTexture(this);
        m_adapterimplementationData = NULL;
#ifndef ITF_WII
		SF_DEL_ARRAY(m_fileContent);
#endif		
        m_createdGFXObject = bfalse;
    }

    bbool Texture::tryLoadFile()
    {
        ITF_ASSERT_MSG(!isPhysicallyLoaded(), "duplicate resource loading can lead to crashes");
        if (!isPhysicallyLoaded())
        {

#if defined(ITF_CTR)
            bbool res = bfalse;
            if (m_createdGFXObject || m_fileContent != NULL)
            {
                res = btrue;
            }
            else
            {
				File *textureFile = NULL;
				String filename = FILESERVER->getCookedName(getPath().getString());

				textureFile = FILEMANAGER->openFile(filename, ITF_FILE_ATTR_READ);
				ITF_ASSERT(textureFile);

				u32 sizeFile = (u32) textureFile->getLength();
				u8 *rawBuf = newAlloc(mId_Temporary, u8[sizeFile]);
				ITF_ASSERT_MSG(rawBuf, "LOAD Texture Failed out of memory %s", filename);

				if (!rawBuf)
				{
					return bfalse;
				}

				u32 bytesRead = 0;
				bbool isRead = textureFile->read(rawBuf, sizeFile, &bytesRead);
				if (isRead)
				{
					m_fileContent = rawBuf;
					m_fileContentSize = bytesRead;
					RESOURCE_MANAGER->addResourceToProcessFromMainThread(this);
					res = btrue;
				}
				else
				{
					SF_DEL_ARRAY(rawBuf);
					RESOURCE_MANAGER->addResourceAsLoaded(this);
				}

				textureFile = FILEMANAGER->closeFile(textureFile);
            }
#else
            PathString8_t p;
            getPath().getString(p);
            bbool res = GFX_ADAPTER->loadTexture(this, p.cStr()
            #ifndef ITF_WII
                , m_alpha
            #endif
                );
#endif
            ITF_WARNING_CATEGORY(Graph,NULL, res, "Could not load texture : %s", getPath().getString8().cStr());

            return res;
        }

        return btrue;
    }

#ifdef ITF_SUPPORT_COOKING

    void Texture::cancelDefault()              
    {
        hijackPhysicalLoaded(bfalse); // pretend unload() was called without changing physical users
        tryLoadFile();
        setAsShared(bfalse);
    }

    void Texture::setUseDefault(Resource* _pDefaultResource)
    {
        setPhysicalLoaded(btrue);
        
        if (_pDefaultResource)
        {
            setAsShared(btrue);
            Texture* defaultTexture = static_cast<Texture*>(_pDefaultResource);

#ifndef ITF_WII
            m_alpha                     = defaultTexture->m_alpha;
#endif            
            m_adapterimplementationData = defaultTexture->m_adapterimplementationData;
            m_sizeX                     = defaultTexture->m_sizeX;
            m_sizeY                     = defaultTexture->m_sizeY;
            m_datasizeX                 = defaultTexture->m_datasizeX;
            m_datasizeY                 = defaultTexture->m_datasizeY;
        }
    }

#endif //ITF_SUPPORT_COOKING

#ifndef ITF_FINAL
    String Texture::getCustomInfo()
    {   
        String info;
        info.setTextFormat("%dx%d %s", m_sizeX ,m_sizeY, m_dbgNfo);
        return info;
    }
#endif //ITF_FINAL

} // namespace ITF

