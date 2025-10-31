#ifndef _ENGINECOMMON_H_
#include "core/enginecommon.h"
#endif // _ENGINECOMMON_H_

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif // _ITF_SYSTEMADAPTER_

#ifndef _LOCALISATION_ID_H_
#include "core/LocalisationId.h"
#endif //_LOCALISATION_ID_H_

#include "LocalisationWrapper.h"
#include "utils.h"


LocalisationWrapper::LocalisationWrapper()
{
    m_localisationManager = new ITF::LocalisationManager();
}

LocalisationWrapper::~LocalisationWrapper()
{
    delete m_localisationManager;
    m_localisationManager = NULL;
}

void LocalisationWrapper::save(String^ _filename)
{
    ITF::String str_filename     = utils::getITFString(_filename);
    m_localisationManager->save(str_filename);
}

void LocalisationWrapper::addTextData(int _language, int _keyLineID, String^ _text)
{
    ITF::String str_text = utils::getITFString(_text);
    ITF::LocalisationId id;
    id = _keyLineID;
    m_localisationManager->addTextData(_language, id, str_text);
}

void LocalisationWrapper::addAudioData(int _keyLineID, String^ _path, String^ _filename, float _volume)
{
    ITF::String str_path = utils::getITFString(_path);
    ITF::String str_filename = utils::getITFString(_filename);
    ITF::LocalisationId id;
    id = _keyLineID;
    m_localisationManager->addAudioData(id, str_path, str_filename, _volume);
}

String^ LocalisationWrapper::getLanguageToStr(int _language)
{
    ITF::String lang;
    ITF::SystemAdapter::getLanguageToStr((const ITF::ITF_LANGUAGE)_language, lang);
    return gcnew String((wchar_t*)lang.cStr());
}

void LocalisationWrapper::saveDefaultLanguage(int _language, String^ _path)
{
    ITF::String str_path = utils::getITFString(_path);
    m_localisationManager->saveDefaultLanguage((ITF::ITF_LANGUAGE)_language, str_path);
}