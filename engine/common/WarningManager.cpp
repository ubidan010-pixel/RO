#include "precompiled_engine.h"

#ifndef ITF_FINAL

#ifndef _ITF_WARNINGMANAGER_H_
#include "engine/common/WarningManager.h"
#endif // _ITF_WARNINGMANAGER_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_SYNCHRONIZE_H_
#include "engine/system/Synchronize.h"
#endif //_ITF_SYNCHRONIZE_H_

#ifndef _ITF_CONFIG_H_
#include "core/Config.h"
#endif //_ITF_CONFIG_H_

#ifndef _ITF_PLUGINGATEWAY_H_
#include "PluginGateway/PluginGateway.h"
#endif //_ITF_PLUGINGATEWAY_H_

#define ITF_DEFINE_WARNINGCATEGORIES_NAMES 1
#include "engine/common/WarningCategories.h"
#undef ITF_DEFINE_WARNINGCATEGORIES_NAMES
#define ITF_DEFINE_WARNINGCATEGORIES_COLORS 1
#include "engine/common/WarningCategories.h"
#undef ITF_DEFINE_WARNINGCATEGORIES_COLORS


namespace ITF
{
    ///A line, made of an optional owner with a message. The message may contain a separator for fields
    struct Line
    {
        static const char m_separator = '\x1D'; // Group Separator (GS)
        static const int m_messageMaxSize=200; //message includes param and separator

        ObjectRef m_owner;
        char m_message[m_messageMaxSize];
        u32 m_count;
        WarningCategoryIndex m_category;

        void fillLine(WarningCategoryIndex _category, const  BaseObject *_owner, const char *_param, const char *_message); //Fill line with given info
        void fetchString(const String &_prefix, String &_line); //get string representation of this line
    private:
        void addStringToLine(char *&_dest, int &_remaining, const char *_src); //add string _str to _dest, which contains _remaining bytes. Doesnt terminating 0. Updates _dest and _remaining
    };

    //Fill in _ownerName from _owner
    static void fetchObjectName(const BaseObject *_owner, String &_ownerName); 

    //static variables. We dont want to interfere with Macros.h, so we put them here
    static const unsigned int m_maxLineCount = 20;
    static unsigned int m_lineCount=0;
    static unsigned int m_firstLineIndex=0;
    static ObjectRef m_filterRef[m_maxLineCount];
    static unsigned int m_filterCount=0;
    static Line m_lines[m_maxLineCount];

    static ITF_THREAD_CRITICAL_SECTION  s_csWarningBuffer;

    ///////////////////////////////////////////////////////////////////////////////////////////
    static void deleteLine(u32 _line)
    {
        ITF_ASSERT_CRASH(_line<m_lineCount, "incorrect line");

        m_lineCount--; 

        for (unsigned int line=_line; line<m_lineCount; line++ )
        {
            unsigned int lineIndex1 = (line+m_firstLineIndex)%m_maxLineCount;
            unsigned int lineIndex2 = (line+1+m_firstLineIndex)%m_maxLineCount;
            Line &l1 = m_lines[lineIndex1];
            const Line &l2 = m_lines[lineIndex2];
            ITF_MEMCOPY(&l1, &l2, sizeof(Line));
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    static const char *findCategoryNameById(WarningCategoryIndex _category)
    {
        //default category
        i32 id = (int)_category;
        if (id<=0 || id>=WarningCategory_END)
        {
            return "";
        }
        //
        return WarningCategory_Names[id];
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    static Color findCategoryColorById(WarningCategoryIndex _category)
    {
        //default category
        i32 id = (int)_category;
        if (id<0 || id>=WarningCategory_END)
        {
            return Color::black();
        }
        //
        return WarningCategory_Colors[id];
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Line::addStringToLine(char *&_dest, int &_remaining, const char *_src)
    {
        while (_remaining>0)
        {
            char c = *_src;
            _src++;

            if (c==0)
                return;
            if (c==m_separator)
                continue;
            *_dest = c;
            _dest++;
            _remaining--;
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Line::fillLine(WarningCategoryIndex _category, const BaseObject *_owner, const char *_param, const char *_message)
    {
        if (_owner)
            m_owner = _owner->getRef();
        else
            m_owner = ITF_INVALID_OBJREF;

        m_count = 1;
        m_category = _category;
        int remaining = m_messageMaxSize-2; //we substract room for a terminating 0 and a separator
        char *dest = m_message;


        if (_param)
        {
            addStringToLine(dest, remaining, _param);
        }

        *(dest++)=m_separator;
        addStringToLine(dest, remaining, _message);

        ITF_ASSERT_MSG(dest<(m_message+m_messageMaxSize), "bug in WarningManager : not enough room");
        *dest = 0;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Line::fetchString(const String &_prefix, String &_line)
    {
        String ownerName;
        BaseObject* pOwner = GETOBJECT(m_owner);
        if (pOwner)
            fetchObjectName(pOwner, ownerName);
        String msg = m_message;
        msg.replace(m_separator, ':');
        const char *categoryName = findCategoryNameById(m_category);
        if (ownerName.isEmpty())
            _line = _prefix+categoryName+msg;
        else
            _line = _prefix+categoryName+ownerName+":"+msg;
        if (m_count>1)
        {
            String tmpTxt;
            tmpTxt.setTextFormat("%s(x %d)", StringToUTF8(_line).get(), m_count);

            _line = tmpTxt;
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void WarningManager::initialize()
    {
        Synchronize::createCriticalSection(&s_csWarningBuffer);
        m_filterCount = 0;
        m_lineCount = 0;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void WarningManager::terminate()
    {
        Synchronize::destroyCriticalSection(&s_csWarningBuffer);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void WarningManager::clearWarningsForObject(const BaseObject *_owner)
    {
        if (!_owner)
            return;
        ObjectRef ownerRef = _owner->getRef();
        for (unsigned int line=0; line<m_lineCount; line++ )
        {
            unsigned int lineIndex = (line+m_firstLineIndex)%m_maxLineCount;
            const Line &l = m_lines[lineIndex];
            if (ownerRef==l.m_owner)
            {
                deleteLine(line);
                line--;
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    bool WarningManager::hasWarningForObject(const BaseObject *_owner)
    {
        ObjectRef filter = ITF_INVALID_OBJREF;
        if (_owner)
        {
            filter = _owner->getRef();
        }

        for (unsigned int line=0; line<m_lineCount; line++ )
        {
            unsigned int lineIndex = (line+m_firstLineIndex)%m_maxLineCount;
            const Line &l = m_lines[lineIndex];
            if (!_owner || filter==l.m_owner)
            {
                return true;
            }
        }

        return false;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void WarningManager::filterDisplayForObjects(BaseObject *_owner[], unsigned int _itemCount)
    {
        csAutoLock cs(s_csWarningBuffer);
        m_filterCount = std::min(_itemCount, m_maxLineCount);
        for (u32 i=0; i<m_filterCount; i++)
        {
            if (_owner[i])
            {
                m_filterRef[i] = _owner[i]->getRef();
            }
            else
            {
                m_filterRef[i] = ITF_INVALID_OBJREF;
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void WarningManager::display()
    {
        csAutoLock cs(s_csWarningBuffer);

        if (GFX_ADAPTER)
        {
            String warningPrefix("");

            unsigned int lineY = 0;
            unsigned int interLine = 12;
            for (unsigned int line=0; line<m_lineCount; line++ )
            {
                unsigned int lineIndex = (line+m_firstLineIndex)%m_maxLineCount;
                String desc;
                Line &l = m_lines[lineIndex];

                //Check filter
                bbool isEnabled = btrue;
                if (m_filterCount)
                {
                    isEnabled = bfalse;
                    for (u32 i=0; i<m_filterCount; i++)
                    {
                        if (m_filterRef[i]==l.m_owner)
                        {
                            isEnabled = btrue;
                            break;
                        }
                    }
                }
                //display
                if (isEnabled)
                {
                    l.fetchString(warningPrefix, desc);
                    Color c = findCategoryColorById(l.m_category);

                    GFX_ADAPTER->drawDBGText(desc, 0, (f32)lineY, c.getRed(), c.getGreen(), c.getBlue(), bfalse, FONT_ALIGN_RIGHT);
                    lineY+=interLine;
                }
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    static void fetchObjectName(const BaseObject *_owner, String &_ownerName)
    {
        if (_owner)
        {
            switch (_owner->getObjectType())
            {
            case BaseObject::eActor:
            case BaseObject::eFrise:
                _ownerName = ((const Pickable*)_owner)->getUserFriendly().cStr();
                break;
            case BaseObject::eRessource:
                _ownerName = ((const Resource*)_owner)->getPath().getString();
                break;
            default:
                _ownerName.clear();
                break;
            }
        }
        else
            _ownerName.clear();

    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void WarningManager::clear()
    {
        csAutoLock cs(s_csWarningBuffer);
        m_firstLineIndex = 0;
        m_lineCount = 0;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    bool WarningManager::isEmpty()
    {
        return m_lineCount==0;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void WarningManager::addWarning(WarningCategoryIndex _category, const BaseObject *_owner, const char *_param, const char *_messageFormat,...)
    {
        if (!CONFIG || CONFIG->m_noWarnings)
            return;
        csAutoLock cs(s_csWarningBuffer);

        const int bufferSize=1024;
        char message[bufferSize];
        va_list arglist;
        va_start(arglist, _messageFormat);
        vsnprintf(message, bufferSize, _messageFormat, arglist);
        message[bufferSize-1]=0;
        va_end( arglist );   
        
        //Log to file if possible
        bbool needFullInfo = ITF::ERROR_HANDLER != NULL;
    #ifdef ITF_SUPPORT_PLUGIN
        needFullInfo = needFullInfo || (SYSTEM_ADAPTER && PLUGINGATEWAY && SYSTEM_ADAPTER->IsSilent());
    #endif
        if(needFullInfo)
        {
            String ownerName;
            fetchObjectName(_owner, ownerName); 
            if (ITF::ERROR_HANDLER)
            {
                LOG("WARNING:%s %s %s", StringToUTF8(ownerName).get(), (_param ? _param : ""), message);
            }
        #ifdef ITF_SUPPORT_PLUGIN
            if(SYSTEM_ADAPTER->IsSilent())
            {
                String strMsg = ownerName.cStr();
                strMsg += " ";
                strMsg += (_param?_param:"");
                strMsg += " ";
                strMsg += message;
                if (PLUGINGATEWAY)
                    PLUGINGATEWAY->OnWarning(StringConverter(strMsg).getChar());
            }
        #endif // ITF_SUPPORT_PLUGIN
        }

        //Try to add to displayed lines
        if (m_lineCount>=m_maxLineCount)
        {
            return;
        }

        Line &line = m_lines[(m_lineCount+m_firstLineIndex)%m_maxLineCount];
        line.fillLine(_category, _owner, _param, message);

        for (u32 i=0; i<m_lineCount; i++)
        {
            Line &existingLine = m_lines[i];
            if ((existingLine.m_owner==line.m_owner) && 
                (strcmp(existingLine.m_message, line.m_message)==0) )
            {
                existingLine.m_count++;
                return;
            }
        }
        //We check if this line is already present. If so, we dont increment the count, so the warning will be ignored
        m_lineCount++;
    }

    void WarningManager::getWarningsList(ITF_VECTOR<String>& _warnings)
    {
        String warningPrefix("");
        String desc;
        _warnings.reserve(m_lineCount);
        for (u32 index = 0; index < m_lineCount; index++)
        {
            m_lines[index].fetchString(warningPrefix, desc);
            _warnings.push_back(desc);
        }        
    }

}

#endif // ITF_FINAL
