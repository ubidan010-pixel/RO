#include "precompiled_gameplay_war.h"

#ifndef _ITF_WAR_WORLDFILE_H_
#include "war/gameplay/War_WorldFile.h"
#endif //_ITF_WAR_WORLDFILE_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(War_WorldFile)

    BEGIN_SERIALIZATION(War_WorldFile)
    END_SERIALIZATION()

    War_WorldFile::War_WorldFile()
    {
    }

    War_WorldFile::~War_WorldFile()
    {
    }

    void War_WorldFile::applySettings()
    {
    }

};