#ifndef _ITF_MUSICSYNCPLAY_H_
#define _ITF_MUSICSYNCPLAY_H_


#ifndef _ITF_RAKIINCLUDES_
#include "engine/sound/raki/RakiIncludes.h"
#endif // _ITF_RAKIINCLUDES_

#ifdef ITF_SUPPORT_RAKI
#ifndef CIRCULARSTATELIST_H
#include "RakiEngine/System/Containers/CircularCommandList.h"
#endif // CIRCULARSTATELIST_H
#else
#include "engine/sound/common/SoundCircularCommandList.h"
#endif




#define ITF_MUSICSYNCPLAY_MAXREGISTEREDINSTANCES 96



namespace raki
{
    class VoiceBase;
}



namespace ITF
{

    class MusicPart;
    class SoundInstance;




 
    class MusicSyncPlay
    {
    public:

        MusicSyncPlay();

        ~MusicSyncPlay();

        bbool registerForPlayAtNextDivision( const u32 _handle, const u32 _playOnNextSubdivision, const f32 _volume );
#ifdef ITF_SUPPORT_RAKI
        bbool registerForPlayAtNextDivision( raki::VoiceBase * _voice, const u32 _playOnNextSubdivision, const f32 _volume );
#else
        bbool registerForPlayAtNextDivision(SoundInstance* _inst, const u32 _playOnNextSubdivision, const f32 _volume);
#endif

        void resetMusicAndForceSyncPlays();

        void audioCallback( MusicPart * _musicPart, const u32 _samplePosition );

    private:

        const bool isVoiceToPlayArrayCoherent();

        struct VoiceInfo
        {
#ifdef ITF_SUPPORT_RAKI
            raki::VoiceBase * m_voiceBase;
#else
            SoundInstance* m_voiceBase;
#endif
            u32 m_subdivision;
        };

        
#ifdef ITF_SUPPORT_RAKI
        raki::CircularCommandList<VoiceInfo> m_voicesToAdd;
#else
        sound::CircularCommandList<VoiceInfo> m_voicesToAdd;
#endif

        VoiceInfo m_voiceToPlayArray[ ITF_MUSICSYNCPLAY_MAXREGISTEREDINSTANCES ];
        u32 m_nbElementsToPlay;
        MusicPart * m_currentMusicPart;
        f32 m_subdivisionSampleDuration;
        u32 m_lastCallbackSubdivision;
    };


}

#endif //_ITF_MUSICSYNCPLAY_H_
