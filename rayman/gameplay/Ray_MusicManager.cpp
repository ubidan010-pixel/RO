#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_MUSICMANAGER_H_
#include "rayman/gameplay/Ray_MusicManager.h"
#endif //_ITF_RAY_MUSICMANAGER_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_PREFETCH_FCT_H_
#include "engine/boot/PrefetchFct.h"
#endif //_ITF_PREFETCH_FCT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_MusicManager);
    IMPLEMENT_OBJECT_RTTI(Ray_MusicManager_Template)

    BEGIN_SERIALIZATION_CHILD(Ray_MusicManager_Template)
    SERIALIZE_MEMBER("lumMusicManager",m_lumMusicManager);
    END_SERIALIZATION()

    bool PrefetchImt( const Path& path, PrefetchFactory::Params& params )
    {
        return PrefetchTemplate< Ray_MusicManager_Template >( path, params );
    }

    void Ray_MusicManager::addMusicFromConfigFile(const Path& _configFile)
    {
#ifdef MUSICMANAGER_MUSICTEST
        m_templateClientHandler.addUsedTemplate(_configFile.getStringID());
        const MusicManager_Template* config = TEMPLATEDATABASE->getTemplate<Ray_MusicManager_Template>(&m_templateClientHandler, _configFile);
        
        if (!config)
        {
            m_lumMusicManagerPath = RAY_GAMEMANAGER->getDefaultLumMusicManagerPath();
            config = TEMPLATEDATABASE->getTemplate<MusicManager_Template>(&m_templateClientHandler, _configFile);

            if (!config)
            {
                ITF_FATAL_ERROR("Couldn't load music config: %s", _configFile.getString8().cStr());
            }
        }
        else
        {
            m_lumMusicManagerPath = ((Ray_MusicManager_Template*)config)->getLumMusicManager();
        }

        addMusic( config );
#endif // MUSICMANAGER_MUSICTEST
    }

}

