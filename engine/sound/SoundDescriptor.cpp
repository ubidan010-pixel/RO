#include "precompiled_engine.h"


#ifndef _ITF_SOUNDDESCRIPTOR_H_
#include "engine/sound/SoundDescriptor.h"
#endif //_ITF_SOUNDDESCRIPTOR_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_SOUNDINSTANCE_H_
#include "engine/sound/SoundInstance.h"
#endif //_ITF_SOUNDINSTANCE_H_

#ifndef _ITF_SOUNDMANAGER_H_
#include "engine/sound/SoundManager.h"
#endif //_ITF_SOUNDMANAGER_H_

#ifndef _ITF_SOUNDCOMPONENT_H_
#include "engine/actors/components/SoundComponent.h"
#endif //_ITF_SOUNDCOMPONENT_H_

#ifndef _ITF_MUSICMANAGER_H_
#include "engine/gameplay/musicmanager.h"
#endif //_ITF_MUSICMANAGER_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_


#include "engine/AdaptersInterfaces/AudioMiddlewareAdapter.h"




namespace ITF
{

    template<class T> void shuffleArray(SafeArray<T> & _array)
    {
        for (u32 i = _array.size()-1; i > 0; i--)
        {
            u32 j = Seeder::GetUnseededU32(0,i);
            T temp = _array[i];
            _array[i] = _array[j];
            _array[j] = temp;
            //_array[j] = _array[i];
        }
    }


    BEGIN_SERIALIZATION(SoundDescriptor_Template)
        SERIALIZE_MEMBER("name",m_name);
        SERIALIZE_MEMBER("volume",m_volume);
        SERIALIZE_MEMBER("category",m_category);
        SERIALIZE_MEMBER("limitCategory",m_limitCategory);
        SERIALIZE_MEMBER("limitMode",m_limitMode);
        SERIALIZE_MEMBER("maxInstances",m_maxInstances);
        SERIALIZE_MEMBER("isStream",m_isStream);
        SERIALIZE_CONTAINER("files",m_files);
        SERIALIZE_OBJECT("params",m_params);
        SERIALIZE_MEMBER("WwiseEventGuid", m_wwiseEventGuid);
    END_SERIALIZATION()

	SoundDescriptor_Template::~SoundDescriptor_Template()
	{
        for ( u32 i = 0 ; i < m_resourceList.size() ; i++ )
        {
            RESOURCE_MANAGER->queryRemoveUserToResourceLogicalData(m_resourceList[i]);
        }
        m_resourceList.clear();
	}

    void SoundDescriptor_Template::onTemplateLoaded()
    {
        if ( isMusic() )
        {
            u32 fileCount = getFileCount();
            ITF_WARNING_CATEGORY(Sound,NULL,fileCount == 1,"Sound Descriptor %s contains more than 1 music, which is not supported by music streamer",GetName().getDebugString());
            if ( fileCount == 1 )
            {
                ResourceID resourceID;
                resourceID = RESOURCE_MANAGER->newResourceIDFromFile(Resource::ResourceType_MusicPart,getFiles()[0]);

                m_resourceList.push_back(resourceID);
            }
        }
        else
        {
            m_resourceList.reserve(m_files.size());
            for (ITF_VECTOR<Path>::const_iterator it = m_files.begin(); it != m_files.end(); ++it)
            {
                ResourceID resourceID;
                resourceID = RESOURCE_MANAGER->newResourceIDFromFile(Resource::ResourceType_Sound,*it);

                m_resourceList.push_back(resourceID);
            }
        }
    }

	void SoundDescriptor_Template::addToResourceGroup(ResourceGroup* _resourceGroup)
	{
		ITF_ASSERT_MSG(_resourceGroup, "SoundDescriptor needs a valid resource group !");
        if ( _resourceGroup )
        {
            for (ITF_VECTOR<ResourceID>::const_iterator it = m_resourceList.begin(); it != m_resourceList.end(); ++it)
            {
                _resourceGroup->addResource(it->getResource());
            }
        }
#if defined(ITF_SUPPORT_WWISE)
        m_soundEventID = AUDIO_ADAPTER->getIDFromGUID(m_wwiseEventGuid);
#if defined(ITF_SUPPORT_WWISE_DYNAMICLOADING)
        AUDIO_ADAPTER->prepareEvent(soundEventID);
#endif
#endif
	}

    void SoundDescriptor_Template::removeFromResourceGroup(ResourceGroup* _resourceGroup)
    {
        ITF_ASSERT_MSG(_resourceGroup, "SoundDescriptor needs a valid resource group !");
        if ( isMusic() && MUSICMANAGER != NULL )
        {
            u32 fileCount = getFileCount();
            if ( fileCount == 1 )
            {
                MUSICMANAGER->removeMusic(GetName());
                _resourceGroup->removeResource(m_resourceList[0]);
            }
        }
        else
        {
            for ( u32 i = 0 ; i < m_resourceList.size() ; ++i )
            {
                if ( m_resourceList[i].isValidResourceId() )
                {
                    _resourceGroup->removeResource(m_resourceList[i]);
                }
            }
#if defined(ITF_SUPPORT_WWISE_DYNAMICLOADING)
            AUDIO_ADAPTER->unPrepareEvent(m_soundEventID);
#endif
        }
    }

    void SoundDescriptor_Template::setFullParams( const SoundParams & _params )
    {
        BinaryClone((SoundParams *)&_params, (SoundParams *)&m_params);
    }


    BEGIN_VALIDATE_OBJECT(SoundDescriptor)
    VALIDATE_PARAM_CATEGORY(Sound,"name",m_template->GetName().isValid(),"Sound name is not valid");
    VALIDATE_PARAM_CATEGORY(Sound,"category",(m_template->getCategory().isValid() /* && SOUND_ADAPTER->isValidBus(m_template->getCategory())*/ ), "Category defined for sound \"%s\" but no bus with the name \"%s\" defined", m_template->GetName().getDebugString(), m_template->getCategory().getDebugString());
    VALIDATE_PARAM_CATEGORY(Sound,"limitCategory",(m_template->getLimitCategory().isValid()/*&& SOUND_ADAPTER->isValidLimitCategory(m_template->getLimitCategory())*/) || !m_template->getLimitCategory().isValid(), "Limit Category defined for sound %s but no limiter with that name %s defined", m_template->GetName().getDebugString(), m_template->getLimitCategory().getDebugString());

    VALIDATE_PARAM_CATEGORY(Sound,"playMode",m_template->getPlayMode() < PlayMode_Count,"Invalid play mode for sound \"%s\"",m_template->GetName().getDebugString());
    VALIDATE_PARAM_CATEGORY(Sound,"randomVolMin",m_template->getRandomVolMin() <= m_template->getRandomVolMax(),"random volume min must be smaller than random volume max for sound \"%s\"",m_template->GetName().getDebugString());
    VALIDATE_PARAM_CATEGORY(Sound,"randomVolMin",(m_template->getRandomVolMin() * m_template->getVolume()) >= 0.0f && (m_template->getRandomVolMin() * m_template->getVolume() ) <= 1.0f,"random volume min + volume is not within -96dB and 0dB range for sound \"%s\"",m_template->GetName().getDebugString());
    VALIDATE_PARAM_CATEGORY(Sound,"randomVolMax",(m_template->getRandomVolMax() * m_template->getVolume()) >= 0.0f && (m_template->getRandomVolMax() * m_template->getVolume()) <= 1.0f,"random volume max must within -96dB and 0dB range for sound \"%s\"",m_template->GetName().getDebugString());
    VALIDATE_PARAM_CATEGORY(Sound,"randomPitchMin",m_template->getRandomPitchMin() <= m_template->getRandomPitchMax(),"random pitch min must be smaller than random volume max for sound \"%s\"",m_template->GetName().getDebugString());
    VALIDATE_PARAM_CATEGORY(Sound,"fadeInTime",m_template->getFadeInTime() >= 0.0f,"fade in time must be positive or null for sound \"%s\"",m_template->GetName().getDebugString());
    VALIDATE_PARAM_CATEGORY(Sound,"fadeOutTime",m_template->getFadeOutTime() >= 0.0f,"fade out time must be positive or null for sound \"%s\"",m_template->GetName().getDebugString());
    VALIDATE_PARAM_CATEGORY(Sound,"filterFrequency",m_template->getParams()->getFilterFrequency() >= 0.0f,"filter frequency must be positive or null for sound \"%s\"",m_template->GetName().getDebugString());
    VALIDATE_PARAM_CATEGORY(Sound,"filterQ",m_template->getParams()->getFilterQ() >= 0.0f,"filter Q must be positive or null for sound \"%s\"",m_template->GetName().getDebugString());
    for (u32 i = 0; i < m_template->getParams()->getModifiers().size(); ++i)
    {
        if (m_template->getParams()->getModifiers()[i])
            m_template->getParams()->getModifiers()[i]->validate(_isComponentValidated);
    }
    //m_template->getParams()->validate(_isComponentValidated);
    END_VALIDATE_OBJECT()

    BEGIN_SERIALIZATION(SoundParams)
        SERIALIZE_MEMBER("numChannels",m_numChannels);
        SERIALIZE_MEMBER("loop",m_loop);
        SERIALIZE_ENUM_BEGIN("playMode",m_playMode);
            SERIALIZE_ENUM_VAR(PlayMode_PlayFirst );
            SERIALIZE_ENUM_VAR(PlayMode_Random);
            SERIALIZE_ENUM_VAR(PlayMode_RandomRememberLast);
            SERIALIZE_ENUM_VAR(PlayMode_RandomSequence);
            SERIALIZE_ENUM_VAR(PlayMode_Sequence);
            SERIALIZE_ENUM_VAR(PlayMode_Input);
        SERIALIZE_ENUM_END();
        SERIALIZE_MEMBER("randomVolMin",m_randomVolMin);
        SERIALIZE_MEMBER("randomVolMax",m_randomVolMax);
        SERIALIZE_MEMBER("delay",m_delay);
        SERIALIZE_MEMBER("randomDelay",m_randomDelay);
        SERIALIZE_MEMBER("randomPitchMin",m_randomPitchMin);
        SERIALIZE_MEMBER("randomPitchMax",m_randomPitchMax);
        SERIALIZE_MEMBER("fadeInTime",m_fadeInTime);
        SERIALIZE_MEMBER("fadeOutTime",m_fadeOutTime);
        SERIALIZE_MEMBER("filterFrequency",m_filterFrequency);
        SERIALIZE_MEMBER("filterQ",m_filterQ);
        SERIALIZE_ENUM_BEGIN("filterType",m_filterType);
            SERIALIZE_ENUM_VAR(FilterType_LowPass);
            SERIALIZE_ENUM_VAR(FilterType_BandPass);
            SERIALIZE_ENUM_VAR(FilterType_HighPass);
            SERIALIZE_ENUM_VAR(FilterType_Notch);
            SERIALIZE_ENUM_VAR(FilterType_None);
        SERIALIZE_ENUM_END();
        SERIALIZE_MEMBER("metronomeType",m_metronomeType);
        SERIALIZE_MEMBER("playOnNext",m_playOnNext);
        SERIALIZE_CONTAINER_WITH_FACTORY("modifiers", m_modifiers, SOUND_MANAGER()->getModifierFactory());
    END_SERIALIZATION()

    //BEGIN_VALIDATE_OBJECT(SoundParams)
    //VALIDATE_PARAM("playMode",m_playMode < PlayMode_Count,"Invalid play mode");
    //VALIDATE_PARAM("randomVolMin",m_randomVolMin.ratio() <= m_randomVolMax.ratio(),"random volume min must be smaller than random volume max");
    //VALIDATE_PARAM("randomVolMin",m_randomVolMin.ratio() >= 0.0f && m_randomVolMin.ratio() <= 1.0f,"random volume min must within -96dB and 0dB range");
    //VALIDATE_PARAM("randomVolMax",m_randomVolMax.ratio() >= 0.0f && m_randomVolMax.ratio() <= 1.0f,"random volume max must within -96dB and 0dB range");
    //VALIDATE_PARAM("randomPitchMin",m_randomPitchMin <= m_randomPitchMax,"random pitch min must be smaller than random volume max");
    //VALIDATE_PARAM("fadeInTime",m_fadeInTime >= 0.0f,"fade in time must be positive or null");
    //VALIDATE_PARAM("fadeOutTime",m_fadeOutTime >= 0.0f,"fade out time must be positive or null");
    //VALIDATE_PARAM("filterFrequency",m_filterFrequency >= 0.0f,"filter frequency must be positive or null");
    //VALIDATE_PARAM("filterQ",m_filterQ >= 0.0f,"filter Q must be positive or null");
    //for (u32 i = 0; i < m_template.getModifiers().size(); ++i)
    //{
    //    if (m_template.getModifiers[i])
    //        m_template.getModifiers[i]->validate(_isComponentValidated);
    //    else 
    //        m_template.getModifiers.eraseKeepOrder(i);
    //}
    //END_VALIDATE_OBJECT()


    ///////////////////////////////////////////////////////////////////////////////////////////
    ///Prepare loading of resource files of this sound descriptor
    void SoundDescriptor::onActorLoaded( Actor* _actor )
    {}

    SoundDescriptor::~SoundDescriptor()
    {
        if (m_freeTemplateOnDelete)
        {
            SF_DEL(m_template);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    ///Remove resources from this sound descriptor from the associated actor
    void SoundDescriptor::clear( class Actor* _actor )
    {}

    ITF::u32 SoundDescriptor::getSoundEventID()
    {
        return  getTemplate()->getSoundEventId();

    }

    ITF::u32 SoundDescriptor::getSoundIndex()
    {
        const u32 fileCount = getTemplate()->getFileCount();

        if ( fileCount == 0)
            return U32_INVALID;
        else if (fileCount == 1)
            return 0;

        //Get index of sound to play
        switch (getTemplate()->getPlayMode())
        {
        case PlayMode_PlayFirst :
            //set sound.param with 1st sound
            return 0;
        case PlayMode_Random:
            //set sound with random sound
            return Seeder::GetUnseededU32(0,fileCount);
        case PlayMode_Input:
        {
            if (!getTemplate()->getPlayModeInput().isValid())
                return U32_INVALID;

            if ( m_playModeInputIndex == U32_INVALID )
            {
                ITF_ASSERT_MSG(0, "input not defined: %s", getTemplate()->getPlayModeInput().getDebugString());
                return U32_INVALID;
            }
            u32 value = 0;
            m_soundComponent->getInputs()[m_playModeInputIndex].getValue(value);
            return value%fileCount;
        }
        case PlayMode_RandomRememberLast:
            {
                u32 index = Seeder::GetUnseededU32(0,fileCount);
                while (index == m_lastIndexPlayed)
                    index = Seeder::GetUnseededU32(0,fileCount);
                m_lastIndexPlayed = index;
                return index;
            }
        case PlayMode_RandomSequence:
            {
                if (m_lastIndexPlayed >= m_indices.size())
                {
                    //randomize
                    shuffleArray(m_indices);
                    m_lastIndexPlayed = 0;
                }

                u32 index = m_lastIndexPlayed;
                ++m_lastIndexPlayed;
                return m_indices[index];
            }
        case PlayMode_Sequence:
            {
                ++m_lastIndexPlayed;
                m_lastIndexPlayed = m_lastIndexPlayed%m_indices.size();
                return m_indices[m_lastIndexPlayed];
            }
        default:
            return 0;
        }

    }

    void SoundDescriptor::init( const class SoundDescriptor_Template * _template,class SoundComponent * _component, bbool _staticTemplate )
    {
        ITF_ASSERT(_component);
        ITF_ASSERT(_template);
        m_template = _template;
        m_freeTemplateOnDelete = !_staticTemplate;
        m_soundComponent = _component;
        m_playModeInputIndex = U32_INVALID;

        if ( getTemplate()->getPlayModeInput().isValid() )
        {
            u32 numInputs = m_soundComponent->getInputs().size();

            for ( u32 i = 0; i < numInputs; i++ )
            {
                const Input& input = m_soundComponent->getInputs()[i];

                if ( input.getId() == getTemplate()->getPlayModeInput() )
                {
                    m_playModeInputIndex = i;
                    break;
                }
            }
        }

        m_indices.resize(getTemplate()->getFileCount());

        if (getTemplate()->getPlayMode() == PlayMode_Sequence || getTemplate()->getPlayMode() == PlayMode_RandomSequence)
        {
            for (u32 i = 0; i < m_indices.size(); ++i)
            {
                m_indices[i] = i;
            }
        }
    }

	///////////////////////////////////////////////////////////////////////////////////////////
	void SoundDescriptor::uninit()
	{
		m_template = NULL;
		m_soundComponent = NULL;

        m_customCategory = StringID::Invalid;

		m_indices.clear();
	}

    bbool SoundParams::useFilter() const
    {
        if (m_filterType != FilterType_None)
            return btrue;

        for (u32 i = 0; i < m_modifiers.size(); ++i)
        {
            if (m_modifiers[i]->useFilter())
                return btrue;
        }
        return bfalse;
    }
    bbool SoundParams::usePitch() const
    {
        if (m_randomPitchMax != 1.0f || m_randomPitchMin != 1.0f)
            return btrue;

        for (u32 i = 0; i < m_modifiers.size(); ++i)
        {
            if (m_modifiers[i]->usePitch())
                return btrue;
        }
        return bfalse;
    }
}


