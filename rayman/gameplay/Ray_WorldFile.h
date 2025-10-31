#ifndef _ITF_RAY_WORLDFILE_H_
#define _ITF_RAY_WORLDFILE_H_

#ifndef _ITF_WORLDFILE_H_
#include "engine/scene/worldFile.h"
#endif //_ITF_WORLD_H_

namespace ITF
{
    // Set player state, powerups, cheats etc, per-scene, for testing purposes.

    class Ray_WorldFile : public Interface_WorldFile_ProjectData
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_WorldFile, Interface_WorldFile_ProjectData,4153301875);

    public:
        DECLARE_SERIALIZE();

        Ray_WorldFile();
        virtual ~Ray_WorldFile();

        virtual void    applySettings();

    private:
    };
};

#endif //_ITF_RAY_WORLDFILE_H_