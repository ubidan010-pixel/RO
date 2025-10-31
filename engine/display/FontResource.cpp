#include "precompiled_engine.h"

#ifndef _ITF_FONTRESOURCE_H_
#include "engine/display/FontResource.h"
#endif //_ITF_FONTRESOURCE_H_

#ifndef _ITF_FONT_H_
#include "engine/display/font.h"
#endif //_ITF_FONT_H_

#ifndef _ITF_FILEMANAGER_ITF_H_
#include "adapters/FileManager_ITF/FileManager_ITF.h"
#endif //_ITF_FILEMANAGER_ITF_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(FontResource)

    FontResource::FontResource( const Path &path ) 
    : Resource(path)
    , m_pFont(NULL)
    {
    }

	void    FontResource::flushPhysicalData()
	{
        ITF_ASSERT(isPhysicallyLoaded());

        SF_DEL(m_pFont);
	}

	bbool    FontResource::tryLoadFile ()
	{
        ITF_ASSERT_MSG(!isPhysicallyLoaded(), "duplicate resource loading can lead to crashes");
	    Font* pFont = newAlloc(mId_Resource,Font());
        PathString8_t pathFilename;

	    getPath().getString(pathFilename);

#ifdef ASSERT_ENABLED
        i32 r = 
#endif
        pFont->init(pathFilename.cStr());

	    ITF_ASSERT(r);
       
        m_pFont = pFont;
        RESOURCE_MANAGER->addResourceAsLoaded(this);

        return btrue;
	}

} // namespace ITF

