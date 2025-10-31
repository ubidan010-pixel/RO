#include "precompiled_engine.h"


#ifndef _ITF_LOCALISATIONMANAGER_H_
#include "engine/localisation/LocalisationManager.h"
#endif //_ITF_LOCALISATIONMANAGER_H_

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

#ifndef _ITF_CONFIG_H_
#include "core/Config.h"
#endif //_ITF_CONFIG_H_

#ifndef _ITF_XMLALL_H_
#include "engine/XML/XML_All.h"
#endif //_ITF_XMLALL_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_EVENTMANAGER_H_
#include "engine/events/EventManager.h"
#endif //_ITF_EVENTMANAGER_H_

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

namespace ITF
{

    void LocText::serialize(ArchiveMemory& _archive)
    {
        m_text.serialize(_archive);

//      m_text.replace('/', '\\');
         m_text.replace( '\\','/');
    }

#if defined (ITF_WINDOWS) && !defined (ITF_FINAL)
    void Localisation_Template::addTextData(i32 _language, LocalisationId _keyLineID, String _text)
    {
        MapDataByLanguage::iterator iter = m_mapDataTextByLanguages.find(_language);
        if (iter == m_mapDataTextByLanguages.end())
        {// add new language and add data
            MapLineIdData map;
            map[_keyLineID] = LocText(_text);

            m_mapDataTextByLanguages[_language] = map;
        }
        else
        {// add data
            MapLineIdData& map = m_mapDataTextByLanguages[_language];
            map[_keyLineID] = LocText(_text);
        }
    }
#endif

    void LocAudio::serialize(ArchiveMemory& _archive)
    {
        _archive.serialize(m_pathID);
        m_filename.serialize(_archive);
        _archive.serialize(m_volume);
    }

#if defined (ITF_WINDOWS) && !defined (ITF_FINAL)
    void Localisation_Template::addAudioData(LocalisationId _keyLineID, String _path, String _filename, f32 _volume)
    {
        i32 pathID = -1;

        for (u32 i = 0; i < m_vecPathAudio.size(); ++i)
        {
            if(m_vecPathAudio[i] == _path)
            {
                pathID = i;
                break;
            }
        }

        // path isn't in vector
        if(pathID == -1)
        {
            pathID = m_vecPathAudio.size();
            m_vecPathAudio.push_back(_path);
        }

        m_mapDataAudio[_keyLineID] = LocAudio(pathID, _filename, _volume);
    }
#endif

    LocalisationManager::LocalisationManager()
        : m_currentLanguage(ITF_LANGUAGE_UNKNOWN)
    {
    }

    LocalisationManager::~LocalisationManager()
    {
        m_template.clear();
    }

    String LocalisationManager::getText(const LocalisationId& _lineID) const
    {
        return m_template.getText(_lineID, m_currentLanguage);
    }

    String Localisation_Template::getText(const LocalisationId& _lineID, const ITF_LANGUAGE _language) const
    {
        const MapDataByLanguage::const_iterator language_it = m_mapDataTextByLanguages.find(_language);
        if (language_it != m_mapDataTextByLanguages.end())
        {
            const MapLineIdData& m_mapDataTextByLanguages = language_it->second;
            const MapLineIdData::const_iterator locText_it = m_mapDataTextByLanguages.find(_lineID);
            if (locText_it != m_mapDataTextByLanguages.end())
            {
                const LocText text = locText_it->second;

                // to spot empty texts
                if (text.getText().isEmpty())
                {

// Francis asked to let English on Console too
//#if defined(ITF_WINDOWS) && !defined(ITF_FINAL)
                    // on PC, allow displaying English when some lineId is missing (RO-10562)
                    if (_language != ITF_LANGUAGE_ENGLISH)
                    {
                        String languageStr;
                        SYSTEM_ADAPTER->getLanguageToStr(_language, languageStr);
                        ITF_WARNING_CATEGORY(L10n, NULL, bfalse, "LineId %d empty in %s, using English instead", _lineID.value, StringToUTF8(languageStr).get());
                        return getText(_lineID, ITF_LANGUAGE_ENGLISH);
                    }
//#endif

                    char buffer[64];
                    ITF_WARNING_CATEGORY(L10n, NULL, bfalse, "LineId %d empty", _lineID.value);
                    SPRINTF_S(buffer, 64, "<EMPTY:%d>", _lineID.value);
                    return String(buffer);
                }

                return text.getText();
            }
        }

        // to spot missing texts
        char buffer[64];
        ITF_WARNING_CATEGORY(L10n, NULL, bfalse, "LineId %d missing", _lineID.value);
        SPRINTF_S(buffer, 64, "<MISSING:%d>", _lineID.value);
        return String(buffer);
    }

    Path Localisation_Template::getAudioPath(const LocalisationId& _lineID) const
    {
        const MapAudioData::const_iterator locAudio_it = m_mapDataAudio.find(_lineID);
        if (locAudio_it != m_mapDataAudio.end())
        {
            const LocAudio audio = locAudio_it->second;

            String fullPath_str = String::emptyString;
            if((u32)audio.getPath() < m_vecPathAudio.size())
            {
                fullPath_str = m_vecPathAudio[audio.getPath()] + "/" + audio.getFilename();

                Path fullPath(fullPath_str);
                return fullPath;
            }
        }
        return Path::EmptyPath;
    }

    f32 Localisation_Template::getAudioVolume(const LocalisationId& _lineID) const
    {
        const MapAudioData::const_iterator locAudio_it = m_mapDataAudio.find(_lineID);
        if (locAudio_it != m_mapDataAudio.end())
        {
            const LocAudio audio = locAudio_it->second;

            return audio.getVolume();
        }
        return 0;
    }

    void LocalisationManager::init(ITF_LANGUAGE _forcedLanguage /* = ITF_LANGUAGE_UNKNOWN */, bbool _clearUnusedLanguages /* = btrue */ )
    {
        load(_forcedLanguage);
        if (_clearUnusedLanguages)
            m_template.clearMapExceptLanguage(m_currentLanguage);
    }

    bbool LocalisationManager::needReloadFont(ITF_LANGUAGE _newLanguage,ITF_LANGUAGE _currentLanguage)
    {
        auto isSpecialLanguage = [](ITF_LANGUAGE _language)
        {
            switch (_language)
            {
            case ITF_LANGUAGE_JAPANESE:
            case ITF_LANGUAGE_SIMPLIFIEDCHINESE:
            case ITF_LANGUAGE_TRADITIONALCHINESE:
                return true;
            default:
                return false;
            }
        };
        return isSpecialLanguage(_newLanguage) || isSpecialLanguage(_currentLanguage);

    }

    bbool LocalisationManager::changeLanguage(ITF_LANGUAGE _newLanguage)
    {
        if (m_localisationFile.isEmpty())
        {
            LOG("LocalisationManager::changeLanguage() called before init()!");
            return bfalse;
        }
        if (m_currentLanguage == _newLanguage)
        {
            return btrue;
        }
        bbool reloadFont = needReloadFont(_newLanguage, m_currentLanguage);
        ITF_LANGUAGE oldLanguage = m_currentLanguage;
        bbool languageDataLoaded = m_template.isLanguageDataLoaded(_newLanguage);

        if (!languageDataLoaded)
        {
            LOG("[LocalisationManager] Language %d data not in memory, reloading localisation file...", _newLanguage);
            m_localisationFile = String::emptyString;
            load(_newLanguage);
            LOG("[LocalisationManager] Localisation file reloaded with all languages");
        }
        else
        {
            m_currentLanguage = _newLanguage;
            LOG("[LocalisationManager] Language changed from %d to %d (data already in memory)", oldLanguage, m_currentLanguage);
        }
        if (reloadFont)
        {
            GFX_ADAPTER->reloadFont();
        }
        if (EVENTMANAGER)
        {
            EventLanguageChanged languageChanged(oldLanguage, m_currentLanguage);
            EVENTMANAGER->broadcastEvent(&languageChanged);
        }

        return btrue;
    }

    void Localisation_Template::clearMapExceptLanguage(ITF_LANGUAGE _language)
    {
        MapDataByLanguage::iterator it = m_mapDataTextByLanguages.begin();
        MapDataByLanguage::iterator end = m_mapDataTextByLanguages.end();

        for (; it != end; ++it)
        {
            ITF_LANGUAGE lang = (ITF_LANGUAGE)it->first;   // i32 => ITF_LANGUAGE

#if defined(ITF_WINDOWS) && !defined(ITF_FINAL)
            // on PC, allow displaying English when some lineId is missing (RO-10562)
            if(lang == ITF_LANGUAGE_ENGLISH)
            {
                continue;
            }
#endif

            if(lang != _language)
            {
                m_mapDataTextByLanguages[lang].clear();
            }
        }

    }

    void Localisation_Template::getSupportedLanguages(ITF_LANGUAGE _supportedLanguages[], u32& _size) const
    {
        _size = 0;
        for (   MapDataByLanguage::const_iterator it = m_mapDataTextByLanguages.begin(), end = m_mapDataTextByLanguages.end();
                it != end;
                ++it)
        {
            _supportedLanguages[_size] = ITF_LANGUAGE(it->first);
            _size++;
        }
    }

    bbool Localisation_Template::isLanguageDataLoaded(ITF_LANGUAGE _language) const
    {
        MapDataByLanguage::const_iterator lang_it = m_mapDataTextByLanguages.find(_language);
        return (lang_it != m_mapDataTextByLanguages.end() && !lang_it->second.empty());
    }

    //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#if defined (ITF_WINDOWS) && !defined (ITF_FINAL)
    void LocalisationManager::save(const String& _filename)
    {
        ArchiveMemory arch;

        m_template.serialize(arch);

        File* writeFile = FILEMANAGER->openFile(_filename,ITF_FILE_ATTR_WRITE|ITF_FILE_ATTR_CREATE_NEW);
        if (writeFile)
        {
            writeFile->write(arch.getData(),arch.getSize());
            FILEMANAGER->closeFile(writeFile);
            writeFile = NULL;
        }
    }
#endif

    void LocalisationManager::load(ITF_LANGUAGE _forcedLanguage)
    {
        // We need to read localisation file first at init, so we must remove dependency on alias manager
        // Please be sure that the following path match the one present in common.alias
        ITF_ASSERT_MSG( m_localisationFile.isEmpty(), "Already loaded" );
        if ( !m_localisationFile.isEmpty() )
            return;
        m_localisationFile = LOCALISATION_FILE;

        //static const Path localisationFile(GETPATH_ALIAS("localisation"));

        ITF_LANGUAGE defaultLanguageFile = getDefaultLanguage();

        m_template.clear();

        File* pFile = FILEMANAGER->openFile(m_localisationFile, ITF_FILE_ATTR_READ);
        ITF_ASSERT_MSG(pFile,"the localisation file is missing %s", StringToUTF8(m_localisationFile).get());
        if (pFile)
        {
            u32 lenght = (u32) pFile->getLength();
            u8* pBuffer = newAlloc(mId_Temporary,u8[lenght]);

            bbool rdValid = pFile->read(pBuffer,lenght);
            FILEMANAGER->closeFile(pFile);
            pFile = NULL;

            if(rdValid)
            {
                ArchiveMemory rd(pBuffer,lenght);
                m_template.serialize(rd);
            }
            SF_DEL_ARRAY(pBuffer);
        }

        // select the good language according to the system and what we have
        u32 slSize = 0;
        ITF_LANGUAGE supportedLanguages[ITF_LANGUAGE_COUNT];
        getSupportedLanguages(supportedLanguages, slSize);

        if (_forcedLanguage==ITF_LANGUAGE_UNKNOWN)
        {
            m_currentLanguage = SYSTEM_ADAPTER->getSystemLanguage(supportedLanguages, slSize, defaultLanguageFile);
        }
        else
        {   // ignore what is in the system, just be sure it's a loaded language
            m_currentLanguage = SYSTEM_ADAPTER->filterSupportedLanguages(_forcedLanguage, supportedLanguages, slSize, defaultLanguageFile);
        }
    }

    void Localisation_Template::serialize(ArchiveMemory& _archive)
    {
        m_mapDataTextByLanguages.serialize(_archive);
        m_mapDataAudio.serialize(_archive);
        m_vecPathAudio.serialize(_archive);
    }

#if defined (ITF_WINDOWS)
    void Localisation_Template::getDependencies(const LocalisationId _lineID, ITF_VECTOR<String>& _resourcePathList)
    {
        MapDataByLanguage::iterator language_it = m_mapDataTextByLanguages.begin();
        MapDataByLanguage::iterator end = m_mapDataTextByLanguages.end();

        for (; language_it != end; ++language_it)
        {
            const MapLineIdData& m_mapDataTextByLanguages = language_it->second;
            const MapLineIdData::const_iterator locText_it = m_mapDataTextByLanguages.find(_lineID);
            if (locText_it != m_mapDataTextByLanguages.end())
            {
                const LocText locText = locText_it->second;
                const String& text = locText.getText();

                const String startStr = "[actor:";
                const String endStr = "]";

                i32 indexStart = 0;
                i32 indexFound;

                text.strstr(startStr.cStr(), bfalse, &indexFound, 0);
                while ( indexFound != -1 )
                {
                    indexStart = indexFound + startStr.getLen() + indexStart;
                    text.strstr(endStr.cStr(), bfalse, &indexFound, indexStart);

                    if ( indexFound != -1 )
                    {
                        String pathResource = text.substr(indexStart, indexFound);
                        _resourcePathList.push_back(pathResource);
                        indexStart = indexStart + indexFound + 1;
                        text.strstr(startStr.cStr(), bfalse, &indexFound, indexStart);
                    }
                }
            }
        }
    }
#endif

    ITF_LANGUAGE LocalisationManager::getDefaultLanguage()
    {
        DefaultLanguage defaultLanguage;
        if (FILEMANAGER->fileExists(DEFAULT_LANGUAGE_FILE))
        {
            File* pFile = FILEMANAGER->openFile(DEFAULT_LANGUAGE_FILE, ITF_FILE_ATTR_READ);

            u32 lenght = (u32) pFile->getLength();
            u8* pBuffer = newAlloc(mId_Temporary,u8[lenght]);

            bbool rdValid = pFile->read(pBuffer,lenght);
            FILEMANAGER->closeFile(pFile);
            pFile = NULL;

            if(rdValid)
            {
                ArchiveMemory rd(pBuffer,lenght);

                defaultLanguage.serialize(rd);
            }
            SF_DEL_ARRAY(pBuffer);
        }

        return defaultLanguage.m_language;
    }

    void LocalisationManager::saveDefaultLanguage(ITF_LANGUAGE _defaultLanguage, String _path)
    {
        ArchiveMemory arch;

        DefaultLanguage dLanguage;
        dLanguage.m_language = _defaultLanguage;
        dLanguage.serialize(arch);

        Path path(DEFAULT_LANGUAGE_FILE);
        _path += path.getBasename();
        File* writeFile = FILEMANAGER->openFile(_path,ITF_FILE_ATTR_WRITE|ITF_FILE_ATTR_CREATE_NEW);
        if (writeFile)
        {
            writeFile->write(arch.getData(), arch.getSize());
            FILEMANAGER->closeFile(writeFile);
            writeFile = NULL;
        }
    }

} // namespace ITF
