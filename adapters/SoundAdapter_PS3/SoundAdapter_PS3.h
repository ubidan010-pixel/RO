#ifndef _ITF_SOUNDADAPTER_H_PS3_
#define _ITF_SOUNDADAPTER_H_PS3_



#include <cell/mstream.h>
#include <cell/spurs/control.h>





#ifndef _ITF_SOUND_ADAPTER_
#include "engine/AdaptersInterfaces/SoundAdapter.h"
#endif //_ITF_SOUND_ADAPTER_




namespace raki
{
    class MultiStreamUpdateThread;
}


namespace ITF
{
	class SoundAdapter_PS3 : public SoundAdapter
	{
	public:
        SoundAdapter_PS3      (   ) ;
		virtual ~SoundAdapter_PS3     (   );
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn void init()
        /// @brief  initialises audio. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bbool init(bbool _audioCaptureAllowed);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn void init()
        /// @brief  returns true if engine creation went well 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bbool isRunning() const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn void shutdown()
        /// @brief  shuts down audio. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void shutdown();

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn void loadSound(Sound * _sound, const char * _path)
        /// @brief loads a Sound resource 
        /// @param [in,out] _sound   [in] _path 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bbool loadSound(Sound * _sound, const char * _path);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn void unloadSound(Sound * _sound)
        /// @brief unloads a Sound resource 
        /// @param [in,out] _sound 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void unloadSound(Sound * _sound);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn SourceVoice * createSource(const Sound * _sound)
        /// @brief creates a source voice from a Sound resource 
        /// @param [in] _sound   
        /// @return pointer to the newly created voice, or NULL if unable to create voice
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual SourceHandle createSource(SourceInfo * _params);
        virtual void toggleDebug() {};
        virtual bbool isDebug() {return bfalse;}

        virtual f32 getSoundDuration(const Sound * _sound);

#ifndef ITF_FINAL
        void getPerformanceDebugInfo(String &debugInfo, char paramDelimiter = '\n', char valueDelimiter = ':');
#endif

        CellSpurs *                         getSpurs();

        CellAudioPortConfig *               getPortConfig();

        CellAudioPortParam *                getAudioPortParam();

	private:


        CellSpurs                          *m_spurs;
        CellAudioPortConfig                 m_portConfig;
        CellAudioPortParam                  m_audioPortParam;

        void *                              m_multistreamMemory;
        void *                              m_mp3Memory;
        u32                                 m_audioPortNumber;

        raki::MultiStreamUpdateThread *     m_multiStreamUpdateThread;

	};

} // namespace ITF



#endif //_ITF_SOUNDADAPTER_H_PS3_