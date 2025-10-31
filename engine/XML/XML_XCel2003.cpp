#include "precompiled_engine.h"

#ifdef ITF_SUPPORT_XML

#ifndef _ITF_XMLALL_H_
#include "engine/XML/XML_All.h"
#endif //_ITF_XMLALL_H_

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_OUTPUTDEBUGGER_H_
#include "core/OutputDebugger.h"
#endif //_ITF_OUTPUTDEBUGGER_H_

#ifndef _ITF_UITEXTMANAGER_H_
#include "engine/actors/managers/UITextManager.h"
#endif //_ITF_UITEXTMANAGER_H_

namespace ITF
{

struct XCelStyle
{
    XCelStyle() {m_color = COLOR_WHITE; m_size = 17.f;}

    StringID    m_id;
    u32         m_color;
    f32         m_size;
};
SafeArray<XCelStyle> m_XCelStyleTab;

i32  getNumberFromString(char _nb)
{
    if (_nb >= '0' && _nb <= '9')
        return _nb-'0';

    if (_nb >= 'a' && _nb <= 'f')
        return _nb-'a' + 10;

    if (_nb >= 'A' && _nb <= 'F')
        return _nb-'A' + 10;

    ITF_ASSERT(0);
    return 0;
}

u32 getColorFromString (const char * _color)
{
    _color++;  //   skip #
    i32 red = getNumberFromString(*_color++);
    red <<= 4;
    red |= getNumberFromString(*_color++);

    i32 green = getNumberFromString(*_color++);
    green <<= 4;
    green |= getNumberFromString(*_color++);

    i32 blue = getNumberFromString(*_color++);
    blue <<= 4;
    blue |= getNumberFromString(*_color++);

    return ITFCOLOR_TO_U32(255,red,green,blue);
}
 
static void decodeTag(const pugi::xml_node _node, i32 _iRowIndex, i32 _iColIndex, i32 &_langColIndex, String& _currentText, String &_friendly, const String & _XMLfileName)
{
    String text = _node.value();
    if (text == "Data")
    {
    }
    else if (text == "ss:Data")
    {
    }
    else if (text == "Font")
    {
        const char* col = _node.attribute("html:Color").value();
        if(col && *col == '#')
            UI_TEXTMANAGER->setCurrentColor(getColorFromString(col));


        const char* sizeAttr = _node.attribute("html:Size").value();
        if(sizeAttr)
        {
            UI_TEXTMANAGER->setCurrentSize((f32)atof(sizeAttr));
        }

        const char* fontAttr = _node.attribute("html:Face").value();
        if(fontAttr)
        {
            String font = fontAttr; // TODO : AJOUTER CETTE FONT
        }
        
    }
    else if (text == "B")
    {
    }
    else 
    {
        // Find the col of the default language
        if (_langColIndex == 0 && _iRowIndex == 0 && _iColIndex != 0)
        {
            if (StringID(text) == UI_TEXTMANAGER->getTextLanguage())
                _langColIndex = _iColIndex;
        }

        if (_langColIndex != 0)
        {
            if (_iRowIndex != 0 && _iColIndex == 0)
                _friendly = _XMLfileName + "." + text;
            else if (_iRowIndex != 0 && _iColIndex == _langColIndex)
                _currentText = text;
        }

        // Add the text
        if (_friendly != String::emptyString && _currentText != String::emptyString)
        {
            UI_TEXTMANAGER->addText(StringID(_friendly), _currentText.cStr());
            _currentText = String::emptyString;

            f32 defaultSize = UI_TEXTMANAGER->getDefaultSize();
            if (UI_TEXTMANAGER->getCurrentSize() != defaultSize)
            {
                // Reinit the size with the default value
                UI_TEXTMANAGER->setCurrentSize(defaultSize);
            }
        }
    }

    pugi::xml_node child = _node.first_child();
    while (!child.empty())
    {
        decodeTag(child, _iRowIndex, _iColIndex, _langColIndex, _currentText, _friendly, _XMLfileName);
        child = child.next_sibling();
    }
}

void XMLAll::readTextDB(const String& _szXMLFile)
{
    DECLARE_OUTPUTDBG;

    PugiXMLWrap XML;
    String friendly = String::emptyString;
    String currentText = String::emptyString;
    const String XMLfileName = FilePath::getFilenameWithoutExtension(_szXMLFile);
    i32 langColIndex = 0;

    // With this example: <Font>Ex </Font><Font>1</Font>
    // If white space is condensed: Ex1
    // If white space isn't condensed: Ex 1

    // TODO ?
    //bool isWhiteSpaceCondensed = TiXmlBase::IsWhiteSpaceCondensed();
    //TiXmlBase::SetCondenseWhiteSpace(false);
    
    m_XCelStyleTab.clear();

    if (!XML.openFile(_szXMLFile, bfalse, PugiXMLWrap::RaiseError, bfalse))
        return;

    if (XML.enterElement("Workbook"))
    {
        if (XML.enterElement("Styles"))
        {
            if (XML.enterElement("Style"))
            {
                do 
                {
                    XCelStyle xCelStyle;
                    const pugi::xml_node node = XML.getCurrentNode();
                    xCelStyle.m_id = StringID(node.attribute("ss:ID").value());

                    if (XML.enterElement("Font"))
                    {
                        const pugi::xml_node nodeFont = XML.getCurrentNode();
                        const char* col = nodeFont.attribute("ss:Color").value();
                        if(col && *col == '#')
                        {
                            xCelStyle.m_color = getColorFromString(col);
                        }

                        const char* sizeAttr = nodeFont.attribute("ss:Size").value();
                        if(sizeAttr)
                        {
                            xCelStyle.m_size = (f32)atoi(sizeAttr);
                        }

                        m_XCelStyleTab.push_back(xCelStyle);
                        XML.exitElement();
                    }
                } while (XML.nextSameElement());
            }
        }
        XML.exitElement();

        // Set default values
        StringID defaultID = StringID("Default");
        bbool isInitialized = bfalse;
        for (u32 i = 0; i < m_XCelStyleTab.size(); i++)
        {
            XCelStyle xCelStyle = m_XCelStyleTab[i];
            if (xCelStyle.m_id == defaultID)
            {
                UI_TEXTMANAGER->setCurrentColor(xCelStyle.m_color);
                UI_TEXTMANAGER->setDefaultSize(xCelStyle.m_size);
                UI_TEXTMANAGER->setCurrentSize(xCelStyle.m_size);
                isInitialized = btrue;
                break;
            }
        }
        if (!isInitialized)
        {
            UI_TEXTMANAGER->setCurrentColor(COLOR_WHITE);
            UI_TEXTMANAGER->setDefaultSize(17.f);
            UI_TEXTMANAGER->setCurrentSize(17.f);
        }

        if (XML.enterElement("Worksheet"))
        {    
            do
            {
                if (XML.enterElement("Table"))
                {    
                    do
                    {
                        u32 iRowIndex = 0;
                        if (XML.enterElement("Row"))
                        {    
                            do
                            {
                                u32 iColIndex = 0;
                                if (XML.enterElement("Cell"))
                                {    
                                    do
                                    {
                                        // Looking for style index
                                        const pugi::xml_node nodeCell = XML.getCurrentNode();
                                        StringID currentID = StringID(nodeCell.attribute("ss:StyleID").value());
                                        for (u32 i = 0; i < m_XCelStyleTab.size(); i++)
                                        {
                                            XCelStyle xCelStyle = m_XCelStyleTab[i];
                                            if (xCelStyle.m_id == currentID)
                                            {
                                                UI_TEXTMANAGER->setCurrentColor(xCelStyle.m_color);
                                                UI_TEXTMANAGER->setCurrentSize(xCelStyle.m_size);
                                                break;
                                            }
                                        }

                                        if (XML.enterElement("Data") || XML.enterElement("ss:Data"))
                                        {
                                            decodeTag(XML.getCurrentNode(), iRowIndex, iColIndex, langColIndex, currentText, friendly, XMLfileName);
                                            XML.exitElement();
                                        }
                                        iColIndex++;
                                    } while (XML.nextSameElement());
                                }
                                iRowIndex++;
                            } while (XML.nextSameElement());
                        }
                    } while (XML.nextSameElement());
                }
            } while (XML.nextSameElement());
        }
    }

    // Reset to default
    // TODO ?
    //TiXmlBase::SetCondenseWhiteSpace(isWhiteSpaceCondensed);
}

} // namespace ITF

#endif ///ITF_SUPPORT_XML