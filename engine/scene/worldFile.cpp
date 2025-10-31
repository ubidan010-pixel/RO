#include "precompiled_engine.h"

#ifndef _ITF_WORLDFILE_H_
#include "engine/scene/worldFile.h"
#endif //_ITF_WORLD_H_


#ifndef SERIALIZEROBJECTPARSER_H
#include "core/serializer/ZSerializerObjectParser.h"
#endif // SERIALIZEROBJECTPARSER_H

#ifndef _ITF_DEBUGINFO_H_
#include "engine/debug/debugInfo.h"
#endif //_ITF_DEBUGINFO_H_



namespace ITF
{

    // Init WorldFile's static members
    ObjectFactory WorldFile::m_projectDataFactory;

    BEGIN_SERIALIZATION(WorldFile)
        SERIALIZE_MEMBER("scenePath", m_scenePath);
        SERIALIZE_MEMBER("description", m_description);
        SERIALIZE_OBJECT_WITH_FACTORY("projectSpecific", m_projectData, &m_projectDataFactory);
    END_SERIALIZATION()

    WorldFile::WorldFile()
        : m_projectData(NULL)
    {
    }

    WorldFile::~WorldFile()
    {
        SF_DEL(m_projectData);
    }
    
    bbool WorldFile::loadFromFile( const Path& _relativePath )
    {
#ifdef ITF_SUPPORT_XML
        PugiXMLWrap XML;
        //set default value from isd filename
        m_scenePath.clear();
        if (!_relativePath.isEmpty())
        {
            String basenameWE;
            _relativePath.getBasenameWithoutExtension(basenameWE);
            m_scenePath = String(_relativePath.getDirectory())+basenameWE+".isc";
        }

        PathString_t pathName;
        
        _relativePath.getString(pathName);

        if(!XML.openFile(pathName, bfalse, PugiXMLWrap::LogWarning))
            return bfalse;

        CSerializerObjectParser parser(&XML);

        this->Serialize(&parser, ESerialize_Data_Load);
        return btrue;
#else
        ITF_ASSERT_CRASH(0,"OLIVIER must implement");
        return bfalse;
#endif //ITF_SUPPORT_XML

    }
    
#if defined(ITF_SUPPORT_XML)
    bbool WorldFile::saveToFile( const String& _absolutePath )
    {
        pugi::xml_document doc;

        pugi::xml_node decl = doc.prepend_child(pugi::node_declaration);
        decl.append_attribute("version").set_value("1.0");
        decl.append_attribute("encoding").set_value("ISO-8859-1");

        pugi::xml_node root = doc.append_child( "root" );  
        
        CSerializerObjectParser parserSave(root);

        this->Serialize(&parserSave, ESerialize_Data_Save);

        return doc.save_file( StringConverter(_absolutePath).getChar() );
    }
#endif // ITF_SUPPORT_XML

    void WorldFile::applySettings()
    {
        if(m_projectData)
            m_projectData->applySettings();
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    IMPLEMENT_OBJECT_RTTI(Interface_WorldFile_ProjectData)

    BEGIN_SERIALIZATION_ABSTRACT(Interface_WorldFile_ProjectData)
    END_SERIALIZATION()

} //namespace ITF
