#include "precompiled_engine.h"

#include "engine/common/CommandArgs.h"

#define isEspace 32

namespace ITF
{
 
void CommandArgs::parseValueKey(const char* arg)
{
    if (strlen(arg) <=1)
        return;

    coupleKeyValue cKV;

    char *keyValue = newAlloc(mId_Temporary, char[1024]);
    *keyValue = 0;

    const char* pVal = arg;
    const char* sinceLastWord = pVal;


    while (*pVal)
    {
        while (*pVal == isEspace) pVal++;
        if (!*pVal) break;


        if (*pVal == '=')
        {
            iPtr len =  pVal-sinceLastWord;
            ITF_Memcpy(keyValue, sinceLastWord, len);
            keyValue[len] = 0;

            sinceLastWord = pVal;
            sinceLastWord++;

        }

        pVal++;

    }

    if (*keyValue!=0)
    {
        cKV.m_szCommand = keyValue;
        cKV.m_szValue   = sinceLastWord;
    }
    else
    {
        cKV.m_szCommand = m_szKeyDefault;
        cKV.m_szValue   = sinceLastWord;
    }

    cKV.m_szCommand.toLower();

    m_vKeyValue.push_back(cKV);
    SF_DEL_ARRAY(keyValue);
}

u32 CommandArgs::find(const String& _szKey) const
{
    String key = _szKey;
    key.toLower();

    for (u32 i = 0; i < m_vKeyValue.size(); ++i)
        if ( m_vKeyValue[i].m_szCommand == key)
            return i;
    return U32_INVALID;
}

bbool CommandArgs::find(const String& _szKey,String& _szResult) const
{
    u32 idx = find(_szKey);

    if (idx == static_cast< u32 >(-1))
        return false;
    _szResult = m_vKeyValue[idx].m_szValue;
    return true;
}

i32     CommandArgs::getValueI32(const String& _szKey,i32 _default) const
{
    u32 idx = find(_szKey);

    if (idx == static_cast< u32 >(-1))
        return _default;
    return m_vKeyValue[idx].m_szValue.atoi32();
}

bbool   CommandArgs::isSet(const String& _szKey) const
{
    u32 idx = find(_szKey);

    if (idx == static_cast< u32 >(-1))
        return false;
    return !(m_vKeyValue[idx].m_szValue == "0" || m_vKeyValue[idx].m_szValue == "");
}

void CommandArgs::process()
{
    StringConverter src(m_szCommandLine);
    char *storedValue = newAlloc(mId_Temporary, char[1024]);
    char *keyValue = storedValue;
    const char* pVal = src.getChar();
    const char* sinceLastWord = pVal;


    while (*pVal)
    {
        while (*pVal == isEspace) pVal++;
        if (!*pVal) break;


        if (*pVal == ';')
        {
            //keyword found
            iPtr len =  pVal-sinceLastWord;
            ITF_Memcpy(keyValue, sinceLastWord, len);
            keyValue[len] = 0;

            parseValueKey(keyValue);
            sinceLastWord = pVal;
            sinceLastWord++;

        }

        pVal++;

    }

    parseValueKey(sinceLastWord);
    SF_DEL_ARRAY(storedValue);


}

}
