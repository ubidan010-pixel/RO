#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_WORLDFILE_H_
#include "rayman/gameplay/Ray_WorldFile.h"
#endif //_ITF_RAY_WORLDFILE_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_DEBUGINFO_H_
#include "engine/debug/debugInfo.h"
#endif //_ITF_DEBUGINFO_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_WorldFile)

    BEGIN_SERIALIZATION(Ray_WorldFile)
    END_SERIALIZATION()

    Ray_WorldFile::Ray_WorldFile()
    {
    }

    Ray_WorldFile::~Ray_WorldFile()
    {
    }

    void Ray_WorldFile::applySettings()
    {
    }

};