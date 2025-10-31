#include "precompiled_engine.h"

#ifndef _ITF_SOUND_H_
#include "engine/sound/Sound.h"
#endif //_ITF_SOUND_H_

#ifndef _ITF_SOUND_ADAPTER_
#include "engine/AdaptersInterfaces/SoundAdapter.h"
#endif //_ITF_SOUND_ADAPTER_

#ifndef _ITF_FILEMANAGER_ITF_H_
#include "adapters/FileManager_ITF/FileManager_ITF.h"
#endif //_ITF_FILEMANAGER_ITF_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_CONFIG_H_
#include "core/Config.h"
#endif //_ITF_CONFIG_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Sound)
	void    Sound::flushPhysicalData()
	{
        if (CONFIG && !CONFIG->m_doFreesounds)
            return;
        ITF_ASSERT(isPhysicallyLoaded());
#ifdef ITF_SUPPORT_RAKI
		SOUND_ADAPTER->unloadSound(this);
#endif
        m_adapterImplementationData = NULL;
	}

	bbool    Sound::tryLoadFile ()
	{
#if defined( ITF_CTR)
        RESOURCE_MANAGER->addResourceAsLoaded(this);
        return btrue;
#endif
        ITF_ASSERT_MSG(!isPhysicallyLoaded(), "duplicate resource loading can lead to crashes");
#if defined(ITF_SUPPORT_RAKI)
        if (!SOUND_ADAPTER)
            return bfalse;
        else
        {
            PathString_t pathName;
            getPath().getString(pathName);
            String filenameCooked = FILESERVER->getCookedName(pathName);
            StringConverter p(filenameCooked);
            return SOUND_ADAPTER->loadSound(this, p.getChar());
        }
#else
        RESOURCE_MANAGER->addResourceAsLoaded(this);
        return btrue;
#endif
	}


} // namespace ITF

