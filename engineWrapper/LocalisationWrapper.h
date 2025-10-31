#pragma once

using namespace System;
using namespace System::Text;
using namespace System::Runtime::InteropServices;
using namespace System::Collections::Generic;
using namespace System::Collections;

#include <windows.h>
#include <vcclr.h>
#include "engine/localisation/LocalisationManager.h"

public ref class LocalisationWrapper
{
public:
    LocalisationWrapper();
    ~LocalisationWrapper();
    void save(String^ _filename);
    void addTextData(int _language, int _keyLineID, String^ _text);
    void addAudioData(int _keyLineID, String^ _path, String^ _filename, float _volume);
    
    String^ getLanguageToStr(int _laguage);
    int     getLanguageCount() { return ITF::SystemAdapter::getLanguageCount(); }

    void saveDefaultLanguage(int _language, String^ _path);

protected:
    ITF::LocalisationManager* m_localisationManager;
};