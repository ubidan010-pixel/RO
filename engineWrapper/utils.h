#pragma once

using namespace System;

class utils
{
public:

    static char* utils::getCharCopy(String^ stringToCopy)
    {
        IntPtr tmp = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(stringToCopy);
        char* tmpChar=(char*)tmp.ToPointer();
        size_t len = strlen(tmpChar);
        char* data = new char[len+1];
        strcpy(data,tmpChar);

        System::Runtime::InteropServices::Marshal::FreeHGlobal(tmp);
        return data;
    }

    static ITF::String utils::getITFString(String^ stringToCopy)
    {
        IntPtr tmp = System::Runtime::InteropServices::Marshal::StringToHGlobalUni(stringToCopy);
        ITF::u16* tmpChar=(ITF::u16*)tmp.ToPointer();
        ITF::String tmp16(tmpChar);
        System::Runtime::InteropServices::Marshal::FreeHGlobal(tmp);

        return tmp16;
}
};
