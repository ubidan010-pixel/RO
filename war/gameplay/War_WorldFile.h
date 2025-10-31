#ifndef _ITF_WAR_WORLDFILE_H_
#define _ITF_WAR_WORLDFILE_H_

#ifndef _ITF_WORLDFILE_H_
#include "engine/scene/worldFile.h"
#endif //_ITF_WORLD_H_

namespace ITF
{

    class War_WorldFile : public Interface_WorldFile_ProjectData
    {
        DECLARE_OBJECT_CHILD_RTTI(War_WorldFile, Interface_WorldFile_ProjectData,760257924)

    public:
        DECLARE_SERIALIZE();

        War_WorldFile();
        virtual ~War_WorldFile();

        virtual void    applySettings();
    };
};

#endif //_ITF_WAR_WORLDFILE_H_