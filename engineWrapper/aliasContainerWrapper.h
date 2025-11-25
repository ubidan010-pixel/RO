#pragma once

using namespace System;
using namespace System::Text;
using namespace System::Runtime::InteropServices;
using namespace System::Collections::Generic;

#include <windows.h>
#include <vcclr.h>
#include "engine/aliasmanager/aliasmanager.h"


public ref class aliasSlotWrapper
{
public:
    String^ group;
    String^ key;
    String^ path;
    String^ wildcardPattern;
};

public ref class aliasContainerWrapper
{
public:
    aliasContainerWrapper();
    ~aliasContainerWrapper();

    void save(String^ _filename);
    void load(String^ _filename);
    void addAlias(String^ _group, String^ _key, String^ _path, String^ _wildcardPattern);

    void getAllAliases(ICollection<aliasSlotWrapper^> ^ _aliasMap);

protected:
    ITF::AliasContainer* m_aliasContainer;
};
