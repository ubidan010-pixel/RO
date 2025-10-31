#include "core/enginecommon.h"

#ifdef ITF_WINDOWS

#ifndef _ITF_ANIMPACKERCONFIGURATION_H_
#include "tools/plugins/CookerPlugin/AnimTexPacker/AnimPackerConfiguration.h"
#endif

#ifndef _ITF_PUGIXMLWRAP_H_
#include "core/XML/PugiXMLWrap.h"
#endif //_ITF_PUGIXMLWRAP_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_ASSERTMANAGER_H_
#include "core/AssertManager.h"
#endif //_ITF_ASSERTMANAGER_H_

#ifndef _ITF_DIRECTORY_H_
#include "core/file/Directory.h"
#endif //_ITF_DIRECTORY_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_FILEMANAGER_H_
#include "core/AdaptersInterfaces/FileManager.h"
#endif //_ITF_FILEMANAGER_H_

///////////////////////////////////////////////////////////////////////////////////////////
///help parsing an item list. Always processes 1 item.
#define PARSE_STRING_LIST_START(stringList, separatorChar)\
    {\
    i32 separatorPos=-1;\
    bbool isFinished = bfalse;\
    do\
        {\
        stringList.strstr(separatorChar, bfalse, &separatorPos);\
        String8 item;\
        isFinished = separatorPos<0;\
        if (isFinished)\
            {\
            item = stringList;\
            item.trim();\
            }\
            else\
            {\
            item = stringList.substr(0, separatorPos).trim();\
            stringList = stringList.substr(separatorPos+1, stringList.getLen()-separatorPos-1);\
            }

#define PARSE_STRING_LIST_END    } while(!isFinished); }

///////////////////////////////////////////////////////////////////////////////////////////
namespace ITF
{
    namespace AnimTexPacker
    {
        ///////////////////////////////////////////////////////////////////////////////////////////    
        void AnimPackerConfiguration::PbkConcatDesc::makeFromString(String8 _desc)
        {
            m_pbkFilter.clear();
            PARSE_STRING_LIST_START(_desc, '+')
                if (item.getLen()==0)
                    item="*";
            m_pbkFilter.push_back(item);
            PARSE_STRING_LIST_END

                if (m_pbkFilter.size()==0)
                    m_pbkFilter.push_back("*");
        }

        ///////////////////////////////////////////////////////////////////////////////////////////    
        bbool AnimPackerConfiguration::PbkConcatDesc::checkFileMatch(const String8 &_pbkBaseName /*WithoutExtension*/) const
        {
            for (u32 i=0; i<m_pbkFilter.size(); i++)
            {
                const String8 &filter=m_pbkFilter[i];
                if (filter=="*")
                    return btrue;
                 
                String8 packBaseIntern(_pbkBaseName);
                String8 filterIntern(filter);
                packBaseIntern.toLower();
                filterIntern.toLower();
                if (packBaseIntern.strstr(filterIntern.cStr()))
                    return btrue;
            }
            return bfalse;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        bbool AnimPackerConfiguration::openConfigurationFile(const String &_platform_w, const String &_path)
        {
            PugiXMLWrap xml;
            const String8 platform(_platform_w.cStr());
            String absDir;

            String fullPath;
            m_mainPath              = FilePath::getDirectory(_path);

            if (!FILEMANAGER->TranslatePath(fullPath, _path) ||
                !xml.openFile(StringConverter(fullPath).getChar(),bfalse, PugiXMLWrap::DoNothing))
            {
                // if the file is EMPTY -> use default params
                m_maxElementSize        = 0;
                m_maxTextureSize        = 4096;
                m_mustExportGenAnim     = bfalse;
                m_useSimpleMerge        = bfalse;

                parsePbkConcatList("*");
                m_importantBones.clear();

                return btrue;
            }

            if (xml.enterElement("AnimationPackerConfiguration"))
            {
                do
                {
                    String8 targetName = xml.readAttribute("TargetName","");
                    if (targetName=="" || targetName==platform)
                    {
                        String8 importantBoneList   = xml.readAttribute("ImportantBones", "");
                        m_maxElementSize            = xml.readIntAttribute("ElementSizeLimit", 0);
                        m_maxTextureSize            = xml.readIntAttribute("MaxTextureSize", 2048);
                        String genAnimExportPath    = xml.readAttribute("GenAnimExportPath", "");
                        m_processBankChange         = xml.readIntAttribute("ProcessBankChange", 0);
                        m_useSimpleMerge            = xml.readIntAttribute("UseSimpleMerge", 0);

                        m_mustExportGenAnim         = genAnimExportPath.getLen() != 0;
                        if (m_mustExportGenAnim)
                        {
                            m_outputPath = genAnimExportPath;
                            if(FILEMANAGER->TranslatePath(absDir, m_outputPath))
                                Directory::create(absDir);
                        }

                        parsePbkConcatList(xml.readAttribute("Merge", ""));
                        if (!parseImportantBoneList(importantBoneList))
                        {
                            incorrectConfigurationFile();
                            return bfalse;
                        }
                        return btrue;
                    }

                } while (xml.nextSameElement());
                xml.exitElement();
                incorrectConfigurationFile();
                return bfalse;
            }
            else
            {
                incorrectConfigurationFile();
                return bfalse;
            }
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        void AnimPackerConfiguration::incorrectConfigurationFile()
        {
            ITF_ERROR("Incorrect animation packer configuration file.\n"
                "Syntax is : <AnimationPackerConfiguration \n"
                "    [ElementSizeLimit=<size limit, in pixels>]\n"
                "    [Merge=\"<mypatchbank1+bank2,...\"]\n"
                "    TargetName=\"[platform]\"\n"
                "    ImportantBones=\"<BoneName>=<size in pixels for template>,...\" />\n"
                "NB : bones are specified in descending priority.");
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        void AnimPackerConfiguration::parsePbkConcatList(String8 _desc)
        {
            m_concatenatedPbkList.clear();

            PARSE_STRING_LIST_START(_desc, ',')
                PbkConcatDesc concatDesc;
            concatDesc.makeFromString(item);
            m_concatenatedPbkList.push_back(concatDesc);
            PARSE_STRING_LIST_END
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        bbool AnimPackerConfiguration::parseImportantBone(const String8 &_importantBoneDecl)
        {
            i32 equalPos=-1;
            _importantBoneDecl.strstr('=', bfalse, &equalPos);
            if (equalPos<0)
                return bfalse;
            String8 name=_importantBoneDecl.substr(0, equalPos);
            name.trim();
            int size = atoi(_importantBoneDecl.cStr()+equalPos+1);
            if (size<8) //Size less that 8 doesn't make sens
                return bfalse;

            ImportantBone ib;
            ib.m_boneName = name;
            ib.m_sizeInPixel = size;
            m_importantBones.push_back(ib);
            return btrue;
        }
        ///////////////////////////////////////////////////////////////////////////////////////////
        bbool AnimPackerConfiguration::parseImportantBoneList(String8 _importantBoneList)
        {
            m_importantBones.clear();
            PARSE_STRING_LIST_START(_importantBoneList, ',')
                if (!parseImportantBone(item))
                    return bfalse;
            PARSE_STRING_LIST_END
                return btrue;
        }
    }
}
#endif
