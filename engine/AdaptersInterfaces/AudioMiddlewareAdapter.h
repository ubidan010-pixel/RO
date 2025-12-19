#ifndef __AUDIOMIDDLEWAREADAPTER_H__
#define __AUDIOMIDDLEWAREADAPTER_H__

#ifndef _ITF_TEMPLATESINGLETON_H_
#include "core/templateSingleton.h"
#endif // ITF_TEMPLATESINGLETON_H_
#if defined(USE_PAD_HAPTICS)
#if defined(ITF_WINDOWS)
#include <mmdeviceapi.h>
#endif
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif
#if defined(USE_PAD_HAPTICS) && defined(ITF_XBOX_SERIES)
#include <GameInput.h>
#endif
#include "engine/AdaptersInterfaces/AudioMiddlewareAdapter_Types.h"
#include "core/serializer/ObjectFactory.h"
#include "engine/events/IEventListener.h"
#include "core/AdaptersInterfaces/SystemAdapter.h"


#if defined ITF_X360 || defined ITF_DURANGO
struct IXAudio2;
#endif

namespace	ITF
{
	class	Vec3d;
	class	SoundComponent_Template;
    class   SoundConfig_Template;
    class   WwiseEnvironmentComponent;
    class   AudioCapture;
	class WWiseSoundBank;
    class SoundInstance;

	//$1
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//

	class Adapter_AudioMiddleware :
        public TemplateSingleton<Adapter_AudioMiddleware>, public IEventListener
	{

	public:
			Adapter_AudioMiddleware();
			virtual					~Adapter_AudioMiddleware();

#ifdef ITF_SUPPORT_PLUGIN
            virtual void            registerPlugins() const = 0;
#endif
#ifdef ITF_SUPPORT_EDITOR
        //    virtual void            registerEditorPlugins() const = 0;
       //     virtual void            visitAllPlayingInstances( void (*_visitor)(void*, const AudioPlayRequest &), void*_userParam ) const = 0;
			virtual void			debugEffects() = 0;
#endif
#ifdef ITF_DEBUG_STRINGID
            virtual const char*     getWwiseNameFromGUID(const StringID &_guid) const = 0;
            virtual const char*     getWwiseNameFromID(SoundEventID _id) const = 0;
#endif
            virtual SoundEventID    getEventIDFromPlayingID(ITF::SoundHandle _playingID) const = 0;
            virtual bbool           getPlayRequestFromPlayingID(SoundHandle _playingID, AudioPlayRequest& request) const = 0;


			virtual bbool			init(bbool  _enableGameOutputCapture=false) = 0;
			virtual void			update(f32 _dt = 0) = 0;
			virtual void			destroy() = 0;
			virtual void			fastDestroy() = 0;
			virtual bbool			isRunning() const = 0;

			virtual SoundEventID	getIDFromGUID(const StringID &_guid) const = 0;
            virtual f32             getDurationFromGUID(const StringID& _guid) const = 0;
			//virtual AudioItemType	getAudioTypeFromGUID(const StringID &_guid) const = 0;

            virtual f32             getLocalizationVolumeFromLocId( u32 _locId) const =0;
            virtual SoundEventID        getLocalizationWWiseIdFromLocId( u32 _locId) const = 0;
            virtual StringID           getLocalizationWWiseGUIdFromLocId( u32 _locId) const = 0;

			virtual void			pauseImmediate(AudioPauseID _pauselevel) = 0;
			virtual void			muteDevice(AudioDeviceID _device) = 0;
			virtual void			unmuteDevice(AudioDeviceID _device) = 0;
            virtual void			pause(AudioPauseID _pauselevel) = 0;
            virtual void			resume(AudioPauseID _pauselevel) = 0;

			virtual void			mute(bbool _isMute) = 0;

            virtual void			loadBank(const char *_path, const SoundComponent_Template *_tpl) = 0;
            virtual void			unloadBank(const char *_path, const SoundComponent_Template *_tpl) = 0;
            virtual void			loadBank(const char *_path, const SoundConfig_Template *_tpl) = 0;
            virtual void			unloadBank(const char *_path, const SoundConfig_Template *_tpl) = 0;
            virtual void            prepareBank(const char* _path) = 0;
            virtual void            unprepareBank(const char* _path) = 0;
			virtual void			unregisterObjectRef(ObjectRef _objRef) = 0;

			virtual void			loadGlobalBank() = 0;
			virtual void			unloadGlobalBank() = 0;
			virtual bbool			isGlobalBankLoaded()= 0;

			virtual void			loadPackage(u32 _episode) = 0;
			virtual void			unloadPackage(u32 _episode) = 0;

			virtual void			signal(AudioSignal _signal) = 0;

			virtual void			setBusEffect(SoundBusID _busID, AudioBusSlotID _slotID, SoundEffectID _effectID) = 0;
			virtual void			resetBusEffect(SoundBusID _busID, AudioBusSlotID _slotID) = 0;

			virtual void			setListenerPosition(const Vec3d &_position, int _listenerIndex = 0) = 0;
			virtual void			setListenerOrientation(const Vec3d &_front, const Vec3d &_top, int _listenerIndex = 0) = 0;
            virtual void			getListenerPosition(Vec3d *_position, int _listenerIndex = 0) const =0;
            virtual void			getListenerOrientation(Vec3d *_front, Vec3d *_top, int _listenerIndex = 0) const=0;
            virtual void            setActorPosition(ObjectRef _objectRef, const ITF_VECTOR<Vec3d>&_positions, AudioMultiPositionMode _mode) = 0;

			virtual SoundHandle	    play(const AudioPlayRequest& _playRequest) = 0;
            virtual bbool           prepareEvent(const  SoundEventID _soundEventID) = 0;
            virtual bbool           unPrepareEvent(const  SoundEventID _soundEventID)= 0;
            virtual void			stop(SoundHandle _SoundHandle) = 0;
			virtual bbool			isPlaying(SoundHandle _SoundHandle) const = 0;
            virtual void            seek(SoundHandle _SoundHandle, f32 _seekMsec) = 0;
            virtual void            pause(SoundHandle _SoundHandle) = 0;
            virtual void            resume(SoundHandle _SoundHandle) = 0;
			virtual bbool			playInstanceAfterDestroy(SoundHandle _SoundHandle) = 0;

			virtual	i32				getSourcePlayPosition(SoundHandle _SoundHandle) = 0;

            virtual void            setVolume(const ObjectRef _objectRef, const f32 _volume) = 0;
            virtual void            setPan(const ObjectRef _objectRef, const f32 _pan) = 0;
            virtual void            setPitch(const ObjectRef _objectRef, f32 _pitch) = 0;
            virtual ObjectRef       createWwiseActor(const char* _name = NULL) = 0;


			virtual void			setRtpc(SoundRtpcID _inputName, f32 _inputValue, ObjectRef _objectRef) = 0;
			virtual void			setRtpc(SoundRtpcID _inputName, SoundRtpcID _inputValue, ObjectRef _objectRef) = 0;
            virtual bool			getRtpc(SoundRtpcID _inputName, f32 &_inputValue, ObjectRef _objectRef) = 0;
            virtual bool            getState(SoundRtpcID _inputName, SoundRtpcID &_value) = 0;
            virtual void            setState(SoundRtpcID _inputName, SoundRtpcID _value) = 0;

            virtual SourceHandle            createSource(SourceInfo* _params, SoundInstance* _inst) = 0;
            virtual  void                   getPauseSensitivePlayingVoices(SafeArray<SourceHandle>& _playingVoices) = 0;
            virtual InstanceManager* getInstanceManager() = 0;
            virtual const SoundConfig_Template* getConfig() const = 0;
            virtual void setBusVolume(StringID _bus, Volume _volume, f32 _time) =0;
            virtual void setMasterVolume(Volume _volume) = 0;
            virtual void setMenuMusicVolume(Volume _volume) = 0;
            virtual void setMenuSFXVolume(Volume _volume) = 0;

            virtual SoftwareBus* getBus(const StringID _name) = 0;

#ifdef USE_PAD_HAPTICS
	   virtual bbool registerHaptics(u32 _pad,u32 _deviceId,u32 _deviceOutputId,InputAdapter::PadType _padType){return bfalse;};
	   virtual bbool unregisterHaptics(u32 _pad){return bfalse;};
	   virtual bbool registerControllerSpeaker(u32 _pad,u32 _deviceId,u32 _deviceOutputId,InputAdapter::PadType _padType){return bfalse;};
	   virtual bbool unregisterControllerSpeaker(u32 _pad){return bfalse;};
	   virtual void  setMotionVolume(Volume _volume) {};
#if defined(ITF_WINDOWS)
	    virtual u32 getDeviceId(IMMDevice* _imDevice) {return 0;};
#elif defined(ITF_XBOX_SERIES)
	    virtual u32 getDeviceId(IGameInputDevice*_device) {return 0;};
	    virtual u32   getDeviceIdFromName(String& _device) {return 0;};
#endif
#endif


#if defined ITF_X360 || defined ITF_DURANGO
            virtual ::IXAudio2*     getXAudio2Interface() = 0;
#endif


            //////////////////////////////////////////////////////////////////////////
            // from IEventListener
            virtual void			onEvent(Event *_event) = 0;

            //////////////////////////////////////////////////////////////////////////

			virtual bool					isInDebugMode() = 0;
			virtual void					setInDebugMode(bool _inDebugMode)= 0;


            //////////////////////////////////////////////////////////////////////////
            // helper
            void helper_setSwitch(const StringID& _switchGroupGUID, const StringID& _switchGUID, ObjectRef _objectRef)
            {
                 if(_switchGroupGUID.isValid() && _switchGUID.isValid())
                 {
                     const SoundRtpcID switchGroupID = getIDFromGUID(_switchGroupGUID);
                     const SoundRtpcID switchID = getIDFromGUID(_switchGUID);
                     setRtpc(switchGroupID, switchID, _objectRef);
                 }
            }

            void helper_setRtpc(const StringID& _rtpcGUID, f32 _value, ObjectRef _objectRef)
            {
                if(_rtpcGUID.isValid())
                {
                    const SoundRtpcID rtpcGUID = getIDFromGUID(_rtpcGUID);
                    setRtpc(rtpcGUID, _value, _objectRef);
                }
            }

            SoundHandle helper_play(const StringID& _eventGUID, ObjectRef _objectRef)
            {
                if(_eventGUID.isValid())
                {
                    SoundEventID soundEventID = getIDFromGUID(_eventGUID);
                    if(soundEventID != ITF_INVALID_SOUND_EVENT_ID)
                    {
                        AudioPlayRequest audioPlayRequest(soundEventID, _objectRef);
                        return play(audioPlayRequest);
                    }
                    else
                    {
                        ITF_ASSERT_MSG(0, "helper_play invalid wwiseid  <%s>", _eventGUID.getDebugString());
                    }
                }

                return ITF_INVALID_SOUND_PLAYING_ID;
            }

            void helper_resetBusEffect(const StringID& _busGUID, AudioBusSlotID _slotID)
            {
                if(_busGUID.isValid())
                {
                    const SoundRtpcID busID= getIDFromGUID(_busGUID);
                    resetBusEffect(busID, _slotID);
                }
            }
    };

#define AUDIO_ADAPTER	Adapter_AudioMiddleware::getptr()
}	// namespace ITF
#endif // __AUDIOMIDDLEWAREADAPTER_H__
