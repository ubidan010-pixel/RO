#pragma once

using namespace System;
using namespace System::Text;
using namespace System::Runtime::InteropServices;
using namespace System::Collections::Generic;
using namespace System::Collections;

#include <windows.h>
#include <vcclr.h>
#include "engine/aliasmanager/aliasmanager.h"

public ref class aliasContainerWrapper
{
public:
    aliasContainerWrapper();
    ~aliasContainerWrapper();
    void save(String^ _filename);
    void load(String^ _filename);
    void addAlias(String^ _key,String^ _path);
    void getAllAlias(Hashtable^ _aliasMap);

protected:
    ITF::aliasContainer* m_aliasContainer;
};