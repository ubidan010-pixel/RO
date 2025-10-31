#include "precompiled_SoundAdapter_CTR.h"

#ifndef _ITF_SOUNDADAPTER_H_CTR_
#include "adapters/SoundAdapter_CTR/SoundAdapter_CTR.h"
#endif //_ITF_SOUNDADAPTER_H_CTR_


#ifndef _ITF_FILEMANAGER_H_
#include "core/AdaptersInterfaces/FileManager.h"
#endif //_ITF_FILEMANAGER_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_SOUND_H_
#include "engine/sound/Sound.h"
#endif //_ITF_SOUND_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_SLOTALLOCATORMANAGER_H_
#include "core/memory/slotallocatorManager.h"
#endif //_ITF_SLOTALLOCATORMANAGER_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_


namespace ITF
{


    //************************************
    // @name	ITF::SoundAdapter_CTR::SoundAdapter_CTR
    // @public 
    // @return	
    //************************************
    SoundAdapter_CTR::SoundAdapter_CTR()
    {

        m_soundStreamer =  NULL;
    }


    //************************************
    // @name	ITF::SoundAdapter_CTR::~SoundAdapter_CTR
    // @public 
    // @return	
    //************************************
    SoundAdapter_CTR::~SoundAdapter_CTR()
    {
        if (isRunning())
            shutdown();
    }

	//************************************
	// @name	ITF::SoundAdapter_CTR::init
	// @public 
	// @return	ITF::bbool
	//************************************
	bbool SoundAdapter_CTR::init()
	{
        bbool res=SoundAdapter::init(false); //CASA::add false parametre _audioCaptureAllowed = false
        if (res)
        {
            res = initInternal();
        }
        return res;
	}


	//************************************
	// @name	ITF::SoundAdapter_CTR::isRunning
	// @public 
	// @return	ITF::bbool
	//************************************
	bbool SoundAdapter_CTR::isRunning() const
	{
        return btrue;
	}


	//************************************
	// @name	ITF::SoundAdapter_CTR::shutdown
	// @public 
	// @return	void
	//************************************
	void SoundAdapter_CTR::shutdown()
	{
        m_busses.clear();
	}


	//************************************
	// @name	ITF::SoundAdapter_CTR::loadSound
	// @public 
	// @param	Sound * _sound
	// @param	const char * _filename
	// @return	ITF::bbool
	//************************************
	bbool SoundAdapter_CTR::loadSound(Sound * _sound, const char * _filename)
	{
        //TODO
        void *resourceData =(void*)1;     
        u32 chunkSize = 128;

        _sound->m_adapterImplementationData = resourceData;
        _sound->setPhysicalLoaded(btrue);
#ifndef ITF_FINAL
        _sound->setMemoryUsage(chunkSize);
#endif
        return btrue;
	}

	//************************************
	// @name	ITF::SoundAdapter_CTR::unloadSound
	// @public 
	// @param	Sound * _sound
	// @return	void
	//************************************
	void SoundAdapter_CTR::unloadSound(Sound * _sound)
	{
        ITF_ASSERT(_sound);
        void * resourceData = (void*)_sound->m_adapterImplementationData;
        if (!resourceData)
            return;
        _sound->m_adapterImplementationData = NULL;
	}

	//************************************
	// @name	ITF::SoundAdapter_CTR::createSource
	// @virtual private 
	// @param	const SourceInfo * _params
	// @return	ITF::SourceHandle
	//************************************
	SourceHandle SoundAdapter_CTR::createSource(const SourceInfo * _params)
	{
        SourceHandle handle = acquireSource();

        return handle;

	}

	//************************************
	// @name	ITF::SoundAdapter_CTR::update
	// @public 
	// @param	f32 _dt
	// @return	void
	//************************************
	void SoundAdapter_CTR::update(f32 _dt)
	{

        //update bus fades
        for (ITF_MAP<StringID,Bus*>::iterator it = m_busses.begin(); it != m_busses.end(); ++it)
        {
            Bus * bus = it->second;
            if (bus->m_timeToFade != 0.0f)
            {
                f32 volume = bus->m_initialVolume + (bus->m_targetVolume-bus->m_initialVolume ) * (1.0f - bus->m_timer/bus->m_timeToFade);
                bus->m_timer = Max(0.0f,bus->m_timer - _dt);
                if (bus->m_timer == 0.0f)
                    bus->m_timeToFade = 0.0f;
            }
        }


    }

     //************************************
     // @name	ITF::SoundAdapter_CTR::setVolume
     // @public 
     // @param	const SourceHandle _source
     // @param	const f32 _volume
     // @return	void
     //************************************
     void SoundAdapter_CTR::setVolume(const SourceHandle _source, const f32 _volume)
    {
        ITF_ASSERT(_source != InvalidSourceHandle);        
    }
 
    //************************************
    // @name	ITF::SoundAdapter_CTR::getVolume
    // @public 
    // @param	const SourceHandle _source
    // @return	ITF::f32
    //************************************
    f32  SoundAdapter_CTR::getVolume(const SourceHandle _source) const 
    {
        ITF_ASSERT(_source != InvalidSourceHandle);
        f32 volume = 0.0f;
        return volume;
    }

    //************************************
    // @name	ITF::SoundAdapter_CTR::play
    // @public 
    // @param	const SourceHandle _source
    // @return	void
    //************************************
    void SoundAdapter_CTR::play(const SourceHandle _source)
    {
        ITF_ASSERT(_source != InvalidSourceHandle);
    }

    //************************************
    // @name	ITF::SoundAdapter_CTR::resume
    // @public 
    // @param	const SourceHandle _source
    // @return	void
    //************************************
    void SoundAdapter_CTR::resume(const SourceHandle _source)
    {
        ITF_ASSERT(_source != InvalidSourceHandle);
    }

    //************************************
    // @name	ITF::SoundAdapter_CTR::emptySource
    // @public 
    // @param	const SourceHandle _source
    // @return	void
    //************************************
    void SoundAdapter_CTR::emptySource(const SourceHandle _source)
    {
        ITF_ASSERT(_source != InvalidSourceHandle);

    }

    //************************************
    // @name	ITF::SoundAdapter_CTR::stop
    // @public 
    // @param	const SourceHandle _source
    // @return	void
    //************************************
    void SoundAdapter_CTR::stop(const SourceHandle _source)
    {
        ITF_ASSERT(_source != InvalidSourceHandle);
    }

    //************************************
    // @name	ITF::SoundAdapter_CTR::getPlayingVoices
    // @public 
    // @param	SafeArray<SourceHandle> & _playingVoices
    // @return	void
    //************************************
    void SoundAdapter_CTR::getPlayingVoices(SafeArray<SourceHandle> &_playingVoices)
    {
        _playingVoices.clear();
    }

    //************************************
    // @name	ITF::SoundAdapter_CTR::isPlaying
    // @public 
    // @param	const SourceHandle _source
    // @return	ITF::bbool
    //************************************
    bbool SoundAdapter_CTR::isPlaying(const SourceHandle _source)
    {
        if (_source == U32_INVALID)
            return false;
        return bfalse;
    }

    //************************************
    // @name	ITF::SoundAdapter_CTR::isStreamingSource
    // @public 
    // @param	const SourceHandle _source
    // @return	ITF::bbool
    //************************************
    bbool SoundAdapter_CTR::isStreamingSource(const SourceHandle _source)
    {
        if (_source == U32_INVALID)
            return false;
        return bfalse;
    }

    //************************************
    // @name	ITF::SoundAdapter_CTR::setOutputVolumes
    // @public 
    // @param	const SourceHandle _source
    // @param	f32 * _volumes
    // @param	u32 _sourceChannels
    // @param	u32 _destChannels
    // @return	void
    //************************************
    void SoundAdapter_CTR::setOutputVolumes(const SourceHandle _source, f32 * _volumes, u32 _sourceChannels, u32 _destChannels)
    {
        ITF_ASSERT(_source != InvalidSourceHandle);
    }

    //************************************
    // @name	ITF::SoundAdapter_CTR::setPitch
    // @public 
    // @param	const SourceHandle _source
    // @param	f32 _pitch
    // @return	void
    //************************************
    void SoundAdapter_CTR::setPitch(const SourceHandle _source, f32 _pitch)
    {
        ITF_ASSERT(_source != InvalidSourceHandle);
    }

    //************************************
    // @name	ITF::SoundAdapter_CTR::toggleDebug
    // @public 
    // @return	void
    //************************************
    void SoundAdapter_CTR::toggleDebug()
    {
    }
 
    //************************************
    // @name	ITF::SoundAdapter_CTR::isDebug
    // @public 
    // @return	ITF::bbool
    //************************************
    bbool SoundAdapter_CTR::isDebug()
    {
        return bfalse;
    }

    //************************************
    // @name	ITF::SoundAdapter_CTR::setMasterVolume
    // @public 
    // @param	f32 _volume
    // @return	void
    //************************************
    void SoundAdapter_CTR::setMasterVolume(f32 _volume)
    {
        
    }

    //************************************
    // @name	ITF::SoundAdapter_CTR::openStreamFile
    // @public 
    // @param	const String & _filename
    // @return	SoundStreamerDataSource *
    //************************************
    SoundStreamerDataSource *SoundAdapter_CTR::openStreamFile(const String &_filename)
    {
        return NULL;
    }

    //************************************
    // @name	ITF::SoundAdapter_CTR::cancelStream
    // @public 
    // @param	SoundStreamHandle _handle
    // @return	ITF::bbool
    //************************************
    bbool SoundAdapter_CTR::cancelStream(SoundStreamHandle _handle)
    {
        return bfalse;
    }

    //************************************
    // @name	ITF::SoundAdapter_CTR::rewindDataSource
    // @public 
    // @param	SoundStreamHandle _handle
    // @param	SoundStreamerDataSource * _dataSource
    // @return	void
    //************************************
    void SoundAdapter_CTR::rewindDataSource(SoundStreamHandle _handle, SoundStreamerDataSource *_dataSource)
    {
        
    }

    //************************************
    // @name	ITF::SoundAdapter_CTR::createStream
    // @public 
    // @param	SoundStreamerDataSource * _dataSource
    // @param	SourceHandle _voice
    // @return	ITF::SoundStreamHandle
    //************************************
    SoundStreamHandle SoundAdapter_CTR::createStream(SoundStreamerDataSource* _dataSource, SourceHandle _voice)
    {
        return InvalidSoundStreamHandle;
    }

    //************************************
    // @name	ITF::SoundAdapter_CTR::addDataSourceToStream
    // @public 
    // @param	SoundStreamHandle _streamhandle
    // @param	SoundStreamerDataSource * _dataSource
    // @param	bbool _isLooping
    // @param	SoundStreamExhaustedCallback _reachEndOfStreamCallback
    // @return	void
    //************************************
    void SoundAdapter_CTR::addDataSourceToStream(SoundStreamHandle _streamhandle, SoundStreamerDataSource* _dataSource, bbool _isLooping, SoundStreamExhaustedCallback _reachEndOfStreamCallback)
    {
    }

    //************************************
    // @name	ITF::SoundAdapter_CTR::createStreamingSourceHandle
    // @public 
    // @param	SoundStreamerDataSource * _dataSource
    // @return	ITF::SourceHandle
    //************************************
    SourceHandle SoundAdapter_CTR::createStreamingSourceHandle(SoundStreamerDataSource *_dataSource)
    {
        return InvalidSourceHandle;
    }

    //************************************
    // @name	ITF::SoundAdapter_CTR::acquireSource
    // @private 
    // @return	ITF::SourceHandle
    //************************************
    SourceHandle SoundAdapter_CTR::acquireSource()
    {
        static int handle = 1;
        return handle++;
    }

    //************************************
    // @name	ITF::SoundAdapter_CTR::destroySource
    // @public 
    // @param	const SourceHandle _source
    // @return	void
    //************************************
    void SoundAdapter_CTR::destroySource(const SourceHandle _source)
    {
        ITF_ASSERT(_source!= InvalidSourceHandle);
    }

    //************************************
    // @name	ITF::SoundAdapter_CTR::initInternal
    // @public 
    // @return	ITF::bbool
    //************************************
    ITF::bbool SoundAdapter_CTR::initInternal()
    {
        if (!isRunning())
            return bfalse;

        initReverbPresets();

        
#if 0
        SoundConfig & config = SOUND_ADAPTER->getConfig();
        u32 numBusses = config.m_busDefs.size();
        for (u32 i = 0; i < numBusses; ++i)
        {
            Bus * bus = newAlloc(mId_Audio,Bus(&config.m_busDefs[i]));
            m_busses[bus->m_busDef->m_name] = bus;
        }


        ITF_MAP<StringID,Bus*>::iterator it = m_busses.find(masterBus);
        ITF_ASSERT_CRASH(it != m_busses.end(),"You must define the master bus in your soundconfig.isg");

        //resolve inputs
        for (ITF_MAP<StringID,Bus*>::const_iterator it = m_busses.begin(); it != m_busses.end(); ++it)
        {
            Bus * bus = it->second;
            u32 numOutputs = bus->m_busDef->m_outputs.size();

            ITF_ASSERT_CRASH((numOutputs == 0 && bus->getName() == masterBus) || (numOutputs > 0),
                "Bus %s has no output defined",bus->m_busDef->m_name.c_str());

            for (u32 i = 0; i < numOutputs; ++i)
            {
                //tell its output it has it as an input
                if (bus->m_busDef->m_outputs[i].isValid())
                    m_busses[bus->m_busDef->m_outputs[i]]->m_inputs.push_back(it->first);
            }
        }

        // calculate processing stage
        for (ITF_MAP<StringID,Bus*>::const_iterator it = m_busses.begin(); it != m_busses.end(); ++it)
        {
            Bus * bus = it->second;
            bus->m_processingStage = getProcessingStage(bus,1000);;
        }

        //create master voice first
        createSubmixVoice(m_busses[masterBus]);

        // create voices
        for (ITF_MAP<StringID,Bus*>::const_iterator it = m_busses.begin(); it != m_busses.end(); ++it)
        {
            Bus * bus = it->second;
            if (bus->getName() != masterBus)
                createSubmixVoice(bus);
        }



        for (ITF_MAP<StringID,Bus*>::iterator it = m_busses.begin(); it != m_busses.end(); ++it)
        {
            Bus* bus = it->second;
            initBus(bus);
        }
#endif
        return btrue;
    }


    //************************************
    // @name	ITF::SoundAdapter_CTR::destroySubmixVoice
    // @private 
    // @param	Bus * _bus
    // @return	void
    //************************************
    void SoundAdapter_CTR::destroySubmixVoice(Bus * _bus)
    {
        for (ITF_VECTOR<StringID>::iterator it = _bus->m_inputs.begin(); it != _bus->m_inputs.end(); ++it)
        {
            destroySubmixVoice(m_busses[*it]);
        }
        _bus->m_inputs.clear();
        delete _bus;
    }

    //************************************
    // @name	ITF::SoundAdapter_CTR::createSubmixVoice
    // @private 
    // @param	Bus * _bus
    // @param	u32 _processingStage
    // @return	void
    //************************************
    void SoundAdapter_CTR::createSubmixVoice(Bus * _bus)
    {
#if 0
        if (_bus->m_busDef->m_name != masterBus)
        {

            u32 flags = _bus->m_busDef->m_filterType != FilterType_None?XAUDIO2_VOICE_USEFILTER:0;

            const XAUDIO2_EFFECT_CHAIN * effectChain = NULL;

            XAUDIO2_EFFECT_DESCRIPTOR descriptor[1];
            XAUDIO2_EFFECT_CHAIN chain;

        }
        else
        {
            IUnknown * pVolumeMeterAPO;
            XAudio2CreateVolumeMeter(&pVolumeMeterAPO);
            XAUDIO2_EFFECT_DESCRIPTOR descriptor[2];

            descriptor[1].InitialState = true;
            descriptor[1].OutputChannels = NUM_CHANNELS;
            descriptor[1].pEffect = pVolumeMeterAPO;

            IUnknown * pReverbAPO;
            XAudio2CreateReverb(&pReverbAPO);

            descriptor[0].InitialState = false;
            descriptor[0].OutputChannels = NUM_CHANNELS;
            descriptor[0].pEffect = pReverbAPO;

            XAUDIO2_EFFECT_CHAIN chain;
            chain.EffectCount = 2;
            chain.pEffectDescriptors = descriptor;

            //
            HRESULT hr;
            if ( FAILED(hr = m_xAudio2->CreateMasteringVoice( &m_masterVoice, NUM_CHANNELS,SAMPLING_RATE, 0, 0, &chain ) ) )
            {
                ITF_FATAL_ERROR("Unable to create sound master voice");
                m_xAudio2->Release();
                m_xAudio2 = NULL;
#ifndef ITF_X360
                CoUninitialize();
#endif //ITF_X360
                LOG("<< SoundAdapter >> failed to create master voice");
                return;
            }
            m_busses[masterBus]->m_voice = m_masterVoice;


        }
#endif
    }

    //************************************
    // @name	ITF::SoundAdapter_CTR::setFilter
    // @public 
    // @param	const SourceHandle _source
    // @param	FilterType _filterType
    // @param	f32 _cutoff
    // @param	f32 _oneOverQ
    // @return	void
    //************************************
    void SoundAdapter_CTR::setFilter(const SourceHandle _source,FilterType _filterType, f32 _cutoff, f32 _oneOverQ)
    {
        ITF_ASSERT(_source != InvalidSourceHandle);

    }

    

    //************************************
    // @name	ITF::SoundAdapter_CTR::setBusFilter
    // @public 
    // @param	StringID _bus
    // @param	FilterType _filterType
    // @param	f32 _cutoff
    // @param	f32 _oneOverQ
    // @return	void
    //************************************
    void SoundAdapter_CTR::setBusFilter(StringID _bus,FilterType _filterType, f32 _cutoff, f32 _oneOverQ)
    {
        return;
        Bus * bus = m_busses[_bus];

        ITF_ASSERT_MSG(!(_filterType != FilterType_None && bus->m_busDef->m_filterType == FilterType_None),"Trying to set a filter on a bus that was not initialized with a filter");
        bus->m_busDef->m_filterType = _filterType;
        bus->m_busDef->m_filterFrequency = _cutoff;
        bus->m_busDef->m_filterQ = _oneOverQ;        
    }

    //************************************
    // @name	ITF::SoundAdapter_CTR::setBusVolume
    // @public 
    // @param	StringID _bus
    // @param	Volume _volume
    // @return	void
    //************************************
    void SoundAdapter_CTR::setBusVolume(StringID _bus, Volume _volume, f32 _time)
    {
        return;
        ITF_ASSERT(_time >= 0.0f);
        Bus * bus = m_busses[_bus];
        //fade requested
        if (_time > 0.0f)
        {
            bus->m_timeToFade = _time;
            bus->m_targetVolume = _volume.ratio();
            bus->m_timer = _time;

        }
        //no fade
        else
        {
            bus->m_timeToFade = 0.0f;
            bus->m_targetVolume = 1.0f;
            bus->m_timer = 0.0f;
            bus->m_initialVolume = _volume.ratio();
        }
    }
    //************************************
    // @name	ITF::SoundAdapter::enableBusReverb
    // @virtual public 
    // @param	StringID _bus
    // @param	bbool _enable
    // @return	void
    //************************************
    void SoundAdapter_CTR::enableBusReverb(StringID _bus,bbool _enable)
    {
        return;
        Bus * bus = m_busses[_bus];
        ITF_ASSERT_MSG(bus->m_busDef->m_hasReverb,"Trying to enable/disable a reverb on a bus that was not created with a reverb");
        if (_enable)
        {
        }
        else
        {
        }
    }
    //************************************
    // @name	ITF::SoundAdapter::setBusReverbPreset
    // @public 
    // @param	StringID _bus
    // @param	ReverbPreset _preset
    // @return	void
    //************************************
    void SoundAdapter_CTR::setBusReverbPreset(StringID _bus,ReverbPreset _preset)
    {
        return;
        Bus * bus = m_busses[_bus];

        ITF_ASSERT_MSG(bus->m_busDef->m_hasReverb,"Trying to set a reverb preset on a bus that was not created with a reverb");

        if (bus->m_busDef->m_hasReverb)
        {            
        }
    }


    //************************************
    // @name	ITF::SoundAdapter_CTR::initReverbPresets
    // @public 
    // @return	void
    //************************************
    void SoundAdapter_CTR::initReverbPresets()
    {

    }

    //************************************
    // @name	ITF::SoundAdapter_CTR::initBus
    // @private 
    // @param	Bus * _bus
    // @return	void
    //************************************
    void SoundAdapter_CTR::initBus( Bus * _bus )
    {
#if 0
        if (_bus->m_busDef->m_name != masterBus)
        {
            u32 numSends = _bus->m_busDef->m_outputs.size();
            ITF_ASSERT(numSends > 0);
            XAUDIO2_SEND_DESCRIPTOR  * sends = newAlloc(mId_Audio,XAUDIO2_SEND_DESCRIPTOR[numSends]);

            for (u32 i = 0; i < numSends; ++i)
            {
                sends[i].Flags = 0; /*XAUDIO2_SEND_USEFILTER*/
                sends[i].pOutputVoice = m_busses[_bus->m_busDef->m_outputs[i]]->m_voice;
            }

            XAUDIO2_VOICE_SENDS sendList;
            sendList.SendCount = numSends;
            sendList.pSends = sends;

            _bus->m_voice->SetOutputVoices(&sendList);

            delete[] sends;

            //SET OUTPUT VOLUMES 
            f32 volumes[] = 
            {
                1.0f,0.0f,
                0.0f,1.0f,
            };
            for (u32 i = 0; i < numSends; ++i)
            {
                _bus->m_voice->SetOutputMatrix(m_busses[_bus->m_busDef->m_outputs[i]]->m_voice,NUM_CHANNELS,NUM_CHANNELS,volumes,m_operationSet);
            }
        }

        //SET REVERB
        if (_bus->m_busDef->m_hasReverb)
        {
            XAUDIO2FX_REVERB_PARAMETERS *reverbParams = getReverbParams(_bus->m_busDef->m_reverbPreset);
            if (reverbParams)
                _bus->m_voice->SetEffectParameters(0,reverbParams,sizeof(XAUDIO2FX_REVERB_PARAMETERS),m_operationSet);
        }

        //SET VOLUME
        _bus->m_voice->SetVolume(_bus->m_busDef->m_volume.ratio(),m_operationSet);

        //SET FILTER
        if (_bus->m_busDef->m_filterType != FilterType_None)
            setVoiceFilterParameters(_bus->m_voice, _bus->m_busDef->m_filterType, _bus->m_busDef->m_filterFrequency, _bus->m_busDef->m_filterQ);
#endif
    }
} // namespace ITF