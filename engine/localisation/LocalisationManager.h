#ifndef _ITF_LOCALISATIONMANAGER_H_
#define _ITF_LOCALISATIONMANAGER_H_

#ifndef _ITF_RESOURCE_H_   
#include "engine/resources/Resource.h"
#endif //_ITF_RESOURCE_H_

#ifndef _ITF_ALIASMANAGER_H_
#include "engine/aliasmanager/aliasmanager.h"
#endif //_ITF_ALIASMANAGER_H_

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

#ifndef _LOCALISATION_ID_H_
#include "core/LocalisationId.h"
#endif // _LOCALISATION_ID_H_

namespace ITF
{
#define DEFAULT_LANGUAGE_FILE "localisation\\default.lng"
#define LOCALISATION_FILE "localisation\\localisation.loc"

    class DefaultLanguage
    {
    public:
        DefaultLanguage(): m_language(ITF_LANGUAGE_ENGLISH) {  }
        void serialize(ArchiveMemory& _archive) { i32 i = (i32)m_language; _archive.serialize(i); m_language = (ITF_LANGUAGE) i; }

        ITF_LANGUAGE m_language;
    };

    //////////////////////////////////////////////////////////////////////////
    // Localisation data
    //////////////////////////////////////////////////////////////////////////
    class LocText
    {
    public:
        LocText() { }
        LocText(const String _text) { m_text = _text; }
        void serialize(ArchiveMemory& _archive);

        const String getText() const { return m_text; }        

    private:
        String m_text;
    };
    
    class LocAudio
    {
    public:
        LocAudio(): m_pathID(0), m_volume(0.0f) { }
        LocAudio(const u32 _pathID, const String _filename, const f32 _volume) { m_pathID = _pathID; m_filename = _filename; m_volume = _volume; }
        void serialize(ArchiveMemory& _archive);

        const u32   getPath()       const { return m_pathID; }
        const String& getFilename() const { return m_filename; }
        const f32 getVolume()       const { return m_volume; }

    private:
        u32    m_pathID;
        String m_filename;
        f32    m_volume;
    };



    //////////////////////////////////////////////////////////////////////////
    // Localisation_template
    //////////////////////////////////////////////////////////////////////////
    class Localisation_Template
    {
    public:
        typedef  ITF_MAP<LocalisationId, LocAudio> MapAudioData;  // key: lineID
        typedef  ITF_VECTOR<String> VecPathAudio;
        typedef  ITF_MAP<LocalisationId, LocText>  MapLineIdData; // key: lineID
        typedef  ITF_MAP<i32, MapLineIdData>  MapDataByLanguage;  // key: Language

        #if defined (ITF_WINDOWS) && !defined (ITF_FINAL) /* USE ONLY SERIALIZE LOCALISATION FILE */
        void addTextData(i32 _language, LocalisationId _keyLineID, String _text);
        void addAudioData(LocalisationId _keyLineID, String _path, String _filename, f32 _volume);
        #endif
        
        void serialize(ArchiveMemory& _archive);
        String getText(const LocalisationId& _lineID, const ITF_LANGUAGE _language) const;
        Path getAudioPath(const LocalisationId& _lineID) const;
        f32 getAudioVolume(const LocalisationId& _lineID) const;

        void getSupportedLanguages(ITF_LANGUAGE _supportedLanguages[], u32& _size) const;
        void clearMapExceptLanguage(ITF_LANGUAGE _language);
        bbool isLanguageDataLoaded(ITF_LANGUAGE _language) const;

        void clear() { m_mapDataTextByLanguages.clear(); }

        #if defined (ITF_WINDOWS)
        void getDependencies(const LocalisationId _lineID, ITF_VECTOR<String>& _resourcePathList);
        #endif

    private:
        MapDataByLanguage m_mapDataTextByLanguages;
        MapAudioData m_mapDataAudio;
        VecPathAudio m_vecPathAudio;
    };


    //////////////////////////////////////////////////////////////////////////
    // LocalisationManager
    //////////////////////////////////////////////////////////////////////////
    class LocalisationManager
    {

    public:
        LocalisationManager();
        ~LocalisationManager();

        // init can be called later to reload if the system language change.
        // If ITF_LANGUAGE_UNKNOWN is given as the first parameter,
        // The language is taken from the system adapter.
        void init(ITF_LANGUAGE _forcedLanguage = ITF_LANGUAGE_UNKNOWN, bbool _clearUnusedLanguages = btrue);
        bbool needReloadFont(ITF_LANGUAGE _newLanguage, ITF_LANGUAGE _currentLanguage);
        bbool changeLanguage(ITF_LANGUAGE _newLanguage);
       
        String getText(const LocalisationId& _lineID) const;
        Path getAudioPath(const LocalisationId& _lineID) const { return m_template.getAudioPath(_lineID); }
        f32 getAudioVolume(const LocalisationId& _lineID) const { return m_template.getAudioVolume(_lineID); }

        #if defined (ITF_WINDOWS) && !defined (ITF_FINAL) /* USE ONLY SERIALIZE LOCALISATION FILE */
        void addTextData(i32 _language, LocalisationId _keyLineID, String _text) { m_template.addTextData(_language, _keyLineID, _text); }
        void addAudioData(LocalisationId _keyLineID, String _path, String _filename, f32 _volume) { m_template.addAudioData(_keyLineID, _path, _filename, _volume); }
        void save(const String& _filename);
        #endif

        #if defined (ITF_WINDOWS)
        void getDependencies(const LocalisationId _lineID, ITF_VECTOR<String>& _resourcePathList) { m_template.getDependencies(_lineID, _resourcePathList); }
        #endif

        ITF_LANGUAGE getCurrentLanguage() const { return m_currentLanguage; }
        void getSupportedLanguages(ITF_LANGUAGE _supportedLanguages[ITF_LANGUAGE_COUNT], u32& _size) const { return m_template.getSupportedLanguages(_supportedLanguages, _size); }

        const String& getLocalisationPath() const { return m_localisationFile; }

        ITF_LANGUAGE getDefaultLanguage();
        void saveDefaultLanguage(ITF_LANGUAGE _defaultLanguage, String _path);

    private:
        void load(ITF_LANGUAGE _forcedLanguage);

        ITF_LANGUAGE m_currentLanguage;
        Localisation_Template m_template;
        String m_localisationFile;
    };


} // namespace ITF

#endif // _ITF_LOCALISATIONMANAGER_H_
