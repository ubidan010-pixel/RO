#include "precompiled_engine.h"

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_XMLALL_H_
#include "engine/XML/XML_All.h"
#endif //_ITF_XMLALL_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_CONFIG_H_
#include "core/Config.h"
#endif //_ITF_CONFIG_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef SERIALIZEROBJECTBINARY_H 
#include "core/serializer/ZSerializerObjectBinary.h"
#endif //#ifndef SERIALIZEROBJECTBINARY_H

#ifndef _ITF_GAMEINTERFACE_H_
#include "engine/gameplay/GameInterface.h"
#endif //_ITF_GAMEINTERFACE_H_

#ifndef SERIALIZEROBJECTPARSER_H
#include "core/serializer/ZSerializerObjectParser.h"
#endif // SERIALIZEROBJECTPARSER_H

#ifndef _ITF_DEBUGINFO_H_
#include "engine/debug/debugInfo.h"
#endif //_ITF_DEBUGINFO_H_

namespace ITF
{

#ifdef ITF_SUPPORT_HOTRELOAD
    XMLAllHotReloadHandler XMLAll::m_hotReloader;
 

    void XMLAllHotReloadHandler::onNewFile(const String& _absoluteFilename)
    {
        onModifiedFile(_absoluteFilename);
    }

    void XMLAllHotReloadHandler::onModifiedFile(const String& _absoluteFilename)
    {
        String relativePath = _absoluteFilename;
        FilePath::transformToRelative(relativePath);

        String extension = FilePath::getExtension(_absoluteFilename);

        if (extension == "gmt")
        {
            GAMEINTERFACE->reloadGameMaterial(relativePath);
        }
        else
        if (extension == "fcg")
        {
            ResourceID resID;

            if ( RESOURCE_MANAGER->getResourceIdFromFile(resID, relativePath, Resource::ResourceType_FriezeConfig) )
            {
                Frise::loadFriezeConfig(relativePath, btrue);
            }
        }
        else
        if (extension == "atl")
        {
            String pathCopy = FilePath::getDirectory(relativePath);
            String name = FilePath::getFilenameWithoutExtension(relativePath);
            // remove underscore.
            name.replace("_atl","");
            
            // try to find tga ressource.
            String nameExt;
            nameExt = pathCopy + (name + ".tga");
            
            Resource* r = RESOURCE_MANAGER->getResourceByPath_slow(nameExt);
            if (!r)
            {
                nameExt = pathCopy + (name + ".png");
                r = RESOURCE_MANAGER->getResourceByPath_slow(nameExt);
            }
            if (r)
            {
                Texture* t = (Texture*)r;
                t->createAtlasIfExist();
            }

        }
        else
        {
            Resource* r = RESOURCE_MANAGER->getResourceByPath_slow(relativePath);
            if (r)
                r->reload();
        }
    }

#endif //ITF_SUPPORT_HOTRELOAD

#ifdef ITF_SUPPORT_XML

    bbool XMLAll::readScene(Scene* _pScene, const Path& _relativeFile, CSerializerObjectParser* serializer /* = NULL */)
    {
        bbool result = bfalse;

        if(_pScene)
        {
            // time to load 
#ifdef ITF_SUPPORT_DEBUGFEATURE
            const f32 rs = (f32)(SYSTEM_ADAPTER->getTime());
#endif //ITF_SUPPORT_DEBUGFEATURE

            PugiXMLWrap XML;
            PathString_t pathName;
            //XML.m_logEnable = btrue;
            _relativeFile.getString(pathName);

            if (XML.openFile(pathName, bfalse, PugiXMLWrap::LogWarning))
            {
                CSerializerObjectParser parser;

                if ( serializer == NULL )
                    serializer = &parser;
                serializer->Init( &XML );
                _pScene->Serialize(serializer,ESerialize_Data_Load);
                result = btrue;
            }

            // time to load
#ifdef ITF_SUPPORT_DEBUGFEATURE
            if (DEBUGINFO)
            {
                const f32 timetoload = (f32)(SYSTEM_ADAPTER->getTime()-rs);
                String displayMsg;
                displayMsg.setTextFormat("Loading \"%s\" took %0.03f seconds", StringToUTF8(pathName).get(), timetoload);
                //DEBUGINFO->addDebugInfoDisplay(displayMsg,10.0f);
                LOG("%s", StringToUTF8(displayMsg).get());
            }
#endif // ITF_SUPPORT_DEBUGFEATURE
        }

        return result;
    }

#ifndef ITF_CONSOLE
    // XML attributes which should be serialized with a Path type
    static const char* s_iscTags[] =
    {
        "ConfigName",
        "LUA",
        "RELATIVEPATH",
        "texture",
        "animInstance",
        NULL
    };


    bbool XMLAll::saveScene(const String& _filenameAbsolute, Scene* _pScene, bbool _binary/* = bfalse*/)
    {
        bbool result = bfalse;

        if(_pScene)
        {
            if (!_binary)
            {
                pugi::xml_document doc;

                pugi::xml_node decl = doc.prepend_child(pugi::node_declaration);
                decl.append_attribute("version").set_value("1.0");
                decl.append_attribute("encoding").set_value("ISO-8859-1");

                pugi::xml_node root = doc.append_child( "root" );

                CSerializerObjectParser parser(root);

                _pScene->Serialize(&parser,ESerialize_Data_Save);

                result = doc.save_file( StringConverter(_filenameAbsolute).getChar() );
            }
            else 
            {
                ArchiveMemory am;
                CSerializerObjectBinary serializeBin(&am);

                _pScene->Serialize(&serializeBin,ESerialize_Data_Save);

                //save to a file;
                File* writeFile = FILEMANAGER->openFile(_filenameAbsolute, ITF_FILE_ATTR_WRITE | ITF_FILE_ATTR_CREATE_NEW);
                bbool res = writeFile != NULL;

                if (writeFile)
                {
                    res &= writeFile->write(am.getData(), am.getSize());
                    FILEMANAGER->closeFile(writeFile);
                    writeFile = NULL;
                }

                result = res;
            }
        }
        
        return result;
    }
#endif //ITF_CONSOLE

#endif // ITF_SUPPORT_XML

} // namespace ITF

