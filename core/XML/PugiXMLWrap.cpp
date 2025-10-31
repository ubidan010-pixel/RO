#include "precompiled_core.h"

#ifdef ITF_SUPPORT_XML

#ifndef _ITF_PUGIXMLWRAP_H_
#include "core/XML/PugiXMLWrap.h"
#endif //_ITF_PUGIXMLWRAP_H_

#ifndef HEADER_PUGIXML_HPP
#include "pugixml/src/pugixml.hpp"
#endif //HEADER_PUGIXML_HPP

#ifndef _ITF_FILEMANAGER_H_
#include "core/AdaptersInterfaces/FileManager.h"
#endif //_ITF_FILEMANAGER_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_WARNINGMANAGER_H_
#include "engine/common/WarningManager.h"
#endif // _ITF_WARNINGMANAGER_H_

namespace ITF
{


PugiXMLWrap::PugiXMLWrap()
: m_logEnable(bfalse)
{

}

PugiXMLWrap::~PugiXMLWrap()
{
}

char* PugiXMLWrap::getFullName()
{
    String s;
    pugi::xml_node cur = m_currentNode;
    bbool first = btrue;
    while(cur != m_doc)
    {
        String ss;
        ss += cur.name();
        if (first)
            first = bfalse;
        else
            ss += " --> ";

        ss += s;
        s = ss;
        cur = cur.parent();
    }
    return s.getCharCopy();
}

bbool PugiXMLWrap::openString(const char* _documentContent, ErrorBehavior _err, bbool _enterRoot)
{
    pugi::xml_parse_result resultLoad = m_doc.load(_documentContent);

    if ( resultLoad.status != pugi::status_ok) 
    {
        if (_err == LogWarning)
        {
            LOG("Could not open XML string :\n\t");
        }
        else if (_err == RaiseError)
        {
            ITF_FATAL_ERROR("Could not open XML string :\n\t");
        }
        return bfalse;
    }

    m_currentNode = m_doc;

    bbool result = btrue;
    if (_enterRoot)
    {
        result = enterElement("root");
        if (!result)
        {
            LOG("Failed to enter element \"root\"");
        }
    }

    return result;
}

bbool PugiXMLWrap::openFile( const String& _szXMLFile,bbool _bUseBinary , ErrorBehavior _err /*= DoNothing*/, bbool _enterRoot /*= btrue */ )
{
    m_szXMLFileDebug = _szXMLFile;
    File* fileXML = FILEMANAGER->openFile(_szXMLFile,ITF_FILE_ATTR_READ);
    if (!fileXML)
        return bfalse;
    
    u32 sizeText = (u32) fileXML->getLength();
    u32 bytesRead = 0;
    char* pBuffer = newAlloc(mId_Temporary, char[sizeText+1]);

    if (!fileXML->read(pBuffer,sizeText,&bytesRead))
    {
        SF_DEL_ARRAY(pBuffer);
        FILEMANAGER->closeFile(fileXML);

        if (_err == RaiseError)
        {
            ITF_FATAL_ERROR("Could not read XML file %s", StringToUTF8(_szXMLFile).get());
        }
        return bfalse;
    }
    pBuffer[sizeText] = 0;

    bbool result = openString(pBuffer,_err,_enterRoot);

    SF_DEL_ARRAY(pBuffer);
    FILEMANAGER->closeFile(fileXML);

    return result;
}

bbool PugiXMLWrap::openString(const String& _documentContent, ErrorBehavior _err, bbool _enterRoot)
{
    return openString(StringConverter(_documentContent).getChar(), _err, _enterRoot);
}

ITF::bbool PugiXMLWrap::enterFirstElement( ErrorBehavior _err /*= DoNothing */ )
{
    pugi::xml_node newElement = m_currentNode.first_child();
    if (!newElement.root())
    {
        char* name = getFullName();
        if (_err == RaiseError)
        {
            ITF_FATAL_ERROR("\"%s\" has no children", name);
        }
        else if (_err == LogWarning)
        {
            LOG("\"%s\" has no children", name);
        }
        SF_DEL_ARRAY(name);

        return bfalse;
    }
    m_currentNode = newElement;
    if (m_logEnable) 
    {
        char* name = getFullName();
        LOG("[LOG Pugi XML] [%s] enterFirstElement: %s", StringToUTF8(m_szXMLFileDebug).get(), name);
        SF_DEL_ARRAY(name);
    }
    
    return btrue;
}

ITF::bbool PugiXMLWrap::enterElement( const char* _elemName, size_t _index, ErrorBehavior _err /*= DoNothing */ )
{
    pugi::xml_node newElement;
    if (_index != 0)
    {
        pugi::xml_node node =  m_currentNode.child(_elemName);
        for (u32 i = 0; i < _index; i++)
        {
            node = node.next_sibling(_elemName);
        }
        newElement = node;
    }
    else
    {
        newElement =  m_currentNode.child(_elemName);
    }

    if (!newElement)
    {
        char* name = getFullName();
        if (_err == RaiseError)
        {
            ITF_FATAL_ERROR("[PugiXMLWrap] Could not find \"%s\" element in \"%s\"", _elemName, name);
        }
        else if (_err == LogWarning)
        {
            LOG("[PugiXMLWrap] Could not find \"%s\" element in \"%s\"", _elemName, name);
        }

        SF_DEL_ARRAY(name);
        return bfalse;
    }

    m_currentNode = newElement;
    if(m_logEnable)
    {
        char* name = getFullName();
        LOG("[LOG Pugi XML] [%s] enterElement: %s", StringToUTF8(m_szXMLFileDebug).get(), name);
        SF_DEL_ARRAY(name);
    }
    return btrue;
}

ITF::bbool PugiXMLWrap::enterElement( const char* _elemName, ErrorBehavior _err /*= DoNothing */ )
{
    return enterElement( _elemName, 0, _err);
}


void PugiXMLWrap::exitElement()
{
    m_currentNode = m_currentNode.parent();

    if (m_logEnable) 
    {
        char* name = getFullName();
        LOG("[LOG Pugi XML] [%s] exitElement: %s", StringToUTF8(m_szXMLFileDebug).get(), name);
        SF_DEL_ARRAY(name);
    }
    ITF_ASSERT(!m_currentNode.empty());
}

ITF::bbool PugiXMLWrap::nextSameElement( bbool _exitAtLast /*= btrue */ )
{
    pugi::xml_node next = m_currentNode.next_sibling(m_currentNode.name());
    if (next)
    {
        m_currentNode = next;
        if(m_logEnable) 
        {
            char* name = getFullName();
            LOG("[LOG Pugi XML] [%s] nextSameElement: %s", StringToUTF8(m_szXMLFileDebug).get(), name);
            SF_DEL_ARRAY(name);
        }
        return btrue;
    }
    if (_exitAtLast)
    {
        exitElement();
    }
    return bfalse;
}

u32	PugiXMLWrap::getNumChildren(const char* _elemName)
{
	u32 result = 0;
	if (enterElement(_elemName))
	{
		do 
		{
			result++;
		}
		while (nextSameElement());
	}
	return result;
}

u32 PugiXMLWrap::getNumChildren()
{
	u32 result = 0;
	if (enterFirstElement())
	{
		do 
		{
			result++;
		}
		while (nextElement());
	}
	return result;
}


ITF::bbool PugiXMLWrap::nextElement( bbool _exitAtLast /*= btrue */ )
{
    pugi::xml_node next = m_currentNode.next_sibling();
    if (next)
    {
        m_currentNode = next;
        if(m_logEnable)
        {
            char* name = getFullName();
            LOG("[LOG Pugi XML] [%s] nextSameElement: %s", StringToUTF8(m_szXMLFileDebug).get(), name);
            SF_DEL_ARRAY(name);
        }
        return btrue;
    }
    if (_exitAtLast)
    {
        exitElement();
    }
    return bfalse;
}

const char* PugiXMLWrap::readAttribute( const char* _attrName, const char* _default /*= NULL*/, ErrorBehavior _err /*= DoNothing */ )
{
    const char * nextName = strchr(_attrName, '/');
    const char*  result = NULL;
    if (nextName)
    {
        char buffer[256];
        size_t len = nextName - _attrName;
        strncpy(buffer, _attrName, len);
        buffer[len] = 0;

        if (enterElement(buffer))
        {
            result = readAttribute( nextName+1, _default, _err );
            exitElement();
        }
    } 
    else
    {
        pugi::xml_attribute att = m_currentNode.attribute(_attrName);
        if(!att.empty())
            result = att.value();
        else
            result = NULL;
    }

    if (!result)
    {
        char* pname = getFullName();
        if (_err == RaiseError)
        {
            ITF_FATAL_ERROR("[PugiXMLWrap] Could not find attribute \"%s\"  in element \"%s\"", _attrName, pname);
        }
        else if (_err == LogWarning)
        {
            LOG("[PugiXMLWrap] Could not find attribute \"%s\"  in element \"%s\"", _attrName, pname);
        }
        SF_DEL_ARRAY(pname);

        result = _default;
    }
    if(m_logEnable) 
    {
        char* name = getFullName();
        LOG("[LOG Pugi XML] [%s] readAttribute: %s", StringToUTF8(m_szXMLFileDebug).get(), result);
        SF_DEL_ARRAY(name);
    }
    return result;
}

int PugiXMLWrap::readIntAttribute( const char* _attrName, int _default /*= 0*/, ErrorBehavior _err /*= DoNothing */ )
{
    const char* attr = readAttribute(_attrName, NULL, _err);

    if (attr)
    {
        i32 val = atoi(attr);
        return val;
    }
    return _default;
}

float PugiXMLWrap::readFloatAttribute( const char* _attrName, float _default /*= 0.0f*/, ErrorBehavior _err /*= DoNothing */ )
{
    const char* attr = readAttribute(_attrName, NULL, _err);
    if (attr)
    {
        float val = (float)atof(attr);
        return val;
    }

    return _default;
}
bbool PugiXMLWrap::readBoolAttribute( const char* _attrName, bbool _default /*= false*/, ErrorBehavior _err /*= DoNothing */ )
{
    const char* attr = readAttribute(_attrName, NULL, _err);

    if (attr)
    {
        if ( _strcmpi(attr, "true") == 0 )
            return btrue;
        i32 val = atoi(attr);
        return val? btrue : bfalse;
    }
    return _default;
}

ITF::Vec2d PugiXMLWrap::readVec2dAttribute( const char* _attrName, const Vec2d& _default /*= Vec2d::Zero*/, ErrorBehavior _err /*= DoNothing */ )
{
    const char* str = readAttribute(_attrName, NULL, _err);
    if (!str)
        return _default;

    uSize len = uSize(strlen(str));
    String8 dest[2];
    ux destIndex = 0;
    for (ux index = 0; index < len; index++)
    {
        switch (str[index])
        {
        case 0:
            break;
        case ' ':
        case '\t':
        case '\r':
        case '\n':
            break;            
        case ',':
        case ';':
            destIndex++;
            if (destIndex > 1)
            {
                ITF_ERROR("error reading Vec2d in XML file");
                return _default;
            }
            break;            
        default:
            dest[destIndex] += str[index];
            break;
        }  
    }

    Vec2d ret((f32)atof(dest[0].cStr()), (f32)atof(dest[1].cStr()));
    return ret;
}

const char * PugiXMLWrap::readElementName()
{
    if(m_logEnable)
    {
        char* name = getFullName();
        LOG("[LOG Pugi XML] [%s] readElementName: %s \t name: %s", StringToUTF8(m_szXMLFileDebug).get(), getFullName(), m_currentNode.name());
        SF_DEL_ARRAY(name);
    }
    return m_currentNode.name();
}


const char * PugiXMLWrap::readElementValue()
{
    if(m_logEnable)
    {
        char* name = getFullName();
        LOG("[LOG Pugi XML] [%s] readElementValue: %s \t value: %s", StringToUTF8(m_szXMLFileDebug).get(), getFullName(), m_currentNode.value());
        SF_DEL_ARRAY(name);
    }
    return m_currentNode.value();
}

} // namespace ITF

#endif //ITF_SUPPORT_XML
