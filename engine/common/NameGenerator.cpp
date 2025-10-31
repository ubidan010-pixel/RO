#include "precompiled_engine.h"

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_NAMEGENERATOR_H_
#include "engine/common/NameGenerator.h"
#endif //_ITF_NAMEGENERATOR_H_

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

namespace ITF
{

    String NameGenerator::getName( const String& _prefix /*= ""*/ )
    {
        String name = _prefix;
        name += "_";
        name += SYSTEM_ADAPTER->userName();
        name += "_";
        name += SYSTEM_ADAPTER->dateTime();
        return name;
    }

    String NameGenerator::getUniqueName( const String& _prefix /*= ""*/ )
    {
        static u32 counter = 0;

        String uniqueName;
        uniqueName.setStringFormat("%s_%u", getName(_prefix).cStr(), counter);

        counter++;

        return uniqueName;
    }

} // namespace ITF