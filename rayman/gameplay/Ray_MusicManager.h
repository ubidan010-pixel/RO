#ifndef _ITF_RAY_MUSICMANAGER_H_
#define _ITF_RAY_MUSICMANAGER_H_

#ifndef _ITF_MUSICMANAGER_H_
#include "engine/gameplay/musicmanager.h"
#endif //_ITF_MUSICMANAGER_H_


namespace ITF
{

    //////////////////////////////////////////////////////////////////////////
    class Ray_MusicManager_Template : public MusicManager_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_MusicManager_Template, MusicManager_Template,3881661542);
        DECLARE_SERIALIZE()

    public:
        Ray_MusicManager_Template()
            : Super()
        {}
        Ray_MusicManager_Template(const Path& _path)
            : Super(_path)
        {}

        const Path &getLumMusicManager() const {return m_lumMusicManager;}

    private:
		Path m_lumMusicManager;
    };


    //////////////////////////////////////////////////////////////////////////
    class Ray_MusicManager 
        : public MusicManager
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_MusicManager,MusicManager,2851409933);
    public:

        Ray_MusicManager() {}
        virtual ~Ray_MusicManager() {}

        const Path &getLumMusicManagerPath() const {return m_lumMusicManagerPath;}

    private:
        virtual void addMusicFromConfigFile(const Path& _configFile);
        Path m_lumMusicManagerPath;


    };

}

#endif // _ITF_MUSICMANAGER_H_

