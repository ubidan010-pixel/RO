#include "precompiled_engine.h"

#ifndef _ITF_XMLALL_H_
#include "engine/XML/XML_All.h"
#endif //_ITF_XMLALL_H_

namespace ITF
{

#ifdef ITF_SUPPORT_XML
u32 XMLAll::saveFields(const ITF_MAP<String, String>& _FieldMap, const String& _filepathAbsolute)
{
    pugi::xml_document doc;
    pugi::xml_node root = doc.append_child( "root" );  

    ITF_MAP<String, String>::const_iterator cit = _FieldMap.begin();


    while(cit != _FieldMap.end())
    {
        StringConverter sfFirst(cit->first);
        StringConverter sfSecond(cit->second);

        pugi::xml_node field = root.append_child( sfFirst.getChar() );
        field.append_attribute("value").set_value(sfSecond.getChar());

        ++cit;
    }

    if(!doc.save_file( StringConverter(_filepathAbsolute).getChar() ))
    {
        ITF_FATAL_ERROR("ITF_FATAL_ERROR: Could not save XML file %s", StringToUTF8(_filepathAbsolute).get());
        return 0;
    }

    return 1;
}

u32 XMLAll::readFields(ITF_MAP<String, String>& _FieldMap, const String& _szXMLFile )
{
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(StringConverter(_szXMLFile).getChar());
    if ( result.status != pugi::status_ok ) 
    {
        ITF_FATAL_ERROR("ITF_FATAL_ERROR: Could not open XML file %s", StringToUTF8(_szXMLFile).get());
        return 0;
    }
    pugi::xml_node root = doc.first_element_by_path("root");
    if (root.empty()) 
    {
        ITF_FATAL_ERROR("ITF_FATAL_ERROR: Could not find \"root\" element in XML config file %s", StringToUTF8(_szXMLFile).get());
        return 0;
    }

    pugi::xml_node elem = root.first_child();
    ITF_ASSERT(!elem.empty());
    while(!elem.empty())
    {
        pugi::xml_attribute attrib = elem.first_attribute();
        ITF_ASSERT(!attrib.empty());
        const char * szName = elem.value();
        const char * szValue = attrib.value();
        _FieldMap[szName] = szValue;

        elem = elem.next_sibling();
    }

    return 1;
}

String XMLAll::readField(const String& _Field, const String& _szXMLFile )
{
    ITF_MAP<ITF::String, ITF::String> fields;
    ITF::XMLAll::readFields(fields, _szXMLFile);

    ITF_ASSERT(fields.find(_Field) != fields.end());

    return fields[_Field];
}

#endif //ITF_SUPPORT_XML
} // namespace ITF

