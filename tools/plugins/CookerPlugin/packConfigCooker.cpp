#include "core/enginecommon.h"

#ifndef _ITF_PACKCONFIGCOOKER_H_
#include "tools/plugins/CookerPlugin/packConfigCooker.h"
#endif

#ifndef _ITF_PUGIXMLWRAP_H_
#include "core/XML/PugiXMLWrap.h"
#endif //_ITF_PUGIXMLWRAP_H_

#ifndef _ITF_ANIMPACKERCONFIGURATION_H_
#include "tools/plugins/CookerPlugin/AnimTexPacker/AnimPackerConfiguration.h"
#endif

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_FILEMANAGER_H_
#include "core/AdaptersInterfaces/FileManager.h"
#endif //_ITF_FILEMANAGER_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_ANIMATIONLOADER_H_
#include "tools/plugins/CookerPlugin/AnimTexPacker/AnimationLoader.h"
#endif

#ifndef _ITF_ANIMATIONLOADCOMMON_H_
#include "tools/plugins/CookerPlugin/AnimLoader/AnimationLoadCommon.h"
#endif //_ITF_ANIMATIONLOADCOMMON_H_

#ifndef _ITF_ANIMATIONTRACK_H_
#include "engine/animation/AnimationTrack.h"
#endif //_ITF_ANIMATIONTRACK_H_

#ifndef TINYXML_INCLUDED
#include "tinyxml/tinyxml.h"
#endif //TINYXML_INCLUDED

namespace ITF
{
    struct AscFile
    {
        String              m_squeleton;
        ITF_VECTOR<String>  m_anmList;
        ITF_VECTOR<String>  m_patchBankName;
        ITF_VECTOR<String>  m_patchBankPath;
        
        f32                 m_scale;
        f32                 m_sceneVersion;
        bbool               m_useDataFolder;
        bbool               m_useRelative;
    };
    
    ///////////////////////////////////////////////////////////////////////////////////////////
    static bbool readASC(const String &_ascFilePath, AscFile & ascFile)
    {
        PugiXMLWrap xml;

        ascFile.m_anmList.clear();
        ascFile.m_anmList.reserve(64);

        if (!xml.openFile(String8(_ascFilePath.cStr()).cStr(),bfalse, PugiXMLWrap::DoNothing))
            return bfalse;
        
        ascFile.m_squeleton = xml.readAttribute("Squeleton/value", "");

        if (xml.enterElement("AnimsList"))
        {
            if (xml.enterElement("Element"))
            {
                do 
                {
                    ascFile.m_anmList.push_back(xml.readAttribute("value", "(incorrect animation in animation list)"));
                } while (xml.nextSameElement());
            }
            xml.exitElement();
        }

        ascFile.m_patchBankName.clear();
        ascFile.m_patchBankPath.clear();
        if (xml.enterElement("PatchBankList"))
        {
            if (xml.enterFirstElement())
            do 
            {
                ascFile.m_patchBankName.push_back(xml.readElementName());
                ascFile.m_patchBankPath.push_back(xml.readAttribute("value", ""));
            } while (xml.nextElement());

            xml.exitElement();
        }

        ascFile.m_scale         = xml.readFloatAttribute("Scale/value", 1.f);
        ascFile.m_sceneVersion  = xml.readFloatAttribute("SceneVersion/value", 1.f);
        ascFile.m_useDataFolder = !strcmp(xml.readAttribute("UseDataFolder/value", ""), "true");
        ascFile.m_useRelative   = !strcmp(xml.readAttribute("UseRelative/value", ""), "true");

        return btrue;
    }

#if !defined(ITF_FINAL) && defined(ITF_WINDOWS) && defined(ITF_DEBUG_STRINGID)
    static void writeASC(const String &_ascFilePath, AscFile & ascFile)
    {
        pugi::xml_document doc;
        pugi::xml_node rootNode = doc.append_child("root");


        AnimationLoadCommon::xmlWriteString(rootNode, "Squeleton", StringConverter(ascFile.m_squeleton).getChar());
        
        pugi::xml_node animListNode = rootNode.append_child("AnimsList");
        {
            for (u32 animIndex=0; animIndex<ascFile.m_anmList.size(); animIndex++)
            {
                pugi::xml_node e = AnimationLoadCommon::xmlWriteString(animListNode, "Element",
                                        StringConverter(ascFile.m_anmList[animIndex]).getChar());
                e.append_attribute("index").set_value(animIndex+1);
            }
        }

        pugi::xml_node patchBankListNode = rootNode.append_child("PatchBankList");
        {
            for (u32 patchBankIndex=0; patchBankIndex<ascFile.m_patchBankName.size(); patchBankIndex++)
            {
                AnimationLoadCommon::xmlWriteString(patchBankListNode, 
                    StringConverter(ascFile.m_patchBankName[patchBankIndex]).getChar(),
                    StringConverter(ascFile.m_patchBankPath[patchBankIndex]).getChar());
            }
        }

        AnimationLoadCommon::xmlWriteFloat(rootNode, "Scale", ascFile.m_scale);
        AnimationLoadCommon::xmlWriteFloat(rootNode, "SceneVersion", ascFile.m_sceneVersion);
        AnimationLoadCommon::xmlWriteBool(rootNode, "UseDataFolder", ascFile.m_useDataFolder);
        AnimationLoadCommon::xmlWriteBool(rootNode, "UseRelative", ascFile.m_useRelative);

        String fullPath;
        if (FILEMANAGER->TranslatePath(fullPath, _ascFilePath, 0))
            doc.save_file(StringConverter(fullPath).getChar());
    }
#endif


    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool packConfig_Cooker::cook(const String& _szPlatform,const String& _szFilename)
    {
        AnimTexPacker::AnimPackerConfiguration config;

        String configPath = FilePath::normalizePath(_szFilename);
        FilePath::transformToRelative(configPath);
        AscFile ascFile;

        String ASCExtension = ".ASC";
        String ascPath = FilePath::getDirectory(configPath)+FilePath::getFilenameWithoutExtension(configPath)+ASCExtension;
        ITF_VECTOR<String> animList;

        if (!FILEMANAGER->fileExists(configPath))
            return bfalse;

        if (!config.openConfigurationFile(_szPlatform, configPath))
        {
            return bfalse;
        }

        if (!readASC(ascPath, ascFile))
        {
            ITF_ERROR("Error reading %ls", ascPath.cStr());
            return bfalse;
        }

        // remove existing redirect file before doing compute
         FILEMANAGER->deleteFile(config.m_mainPath + AnimTrack::s_redirectPatchBank);

        AnimTexPacker::AnimationLoader loader;
        ResourceGroup *res = loader.loadAnimations(ascFile.m_anmList);
        if (!res)
        {
            ITF_ERROR("Could not process animations");
            return bfalse;
        }
        if (config.m_useSimpleMerge)
        {
            loader.buildBasicPack(config, _szPlatform);
        } else
        {
            loader.buildPackedMap(config, _szPlatform);
        }

#if !defined(ITF_FINAL) && defined(ITF_WINDOWS) && defined(ITF_DEBUG_STRINGID)
        if (config.m_mustExportGenAnim)
        {
            loader.modifyAnimations(config);
            loader.exportAnimations(String8(config.m_outputPath.cStr()));
            
            for (u32 i = 0; i < ascFile.m_anmList.size(); i++)
                ascFile.m_anmList[i] = config.m_outputPath + FilePath::getFilename(ascFile.m_anmList[i]);

            ascFile.m_patchBankName.resize(loader.getMergeListCount());
            ascFile.m_patchBankPath.resize(loader.getMergeListCount());
            for (u32 i = 0; i < loader.getMergeListCount(); i++)
            {
                ascFile.m_patchBankName[i].setTextFormat("pbk%03d", i+1);
                ascFile.m_patchBankPath[i].setTextFormat("%Spbk%03d.tga", config.m_mainPath.cStr(), i+1);
            }
            writeASC(config.m_outputPath + FilePath::getFilename(ascPath), ascFile);
        }
#endif
        return btrue;
    }
}
