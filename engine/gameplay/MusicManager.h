#ifndef _ITF_MUSICMANAGER_H_
#define _ITF_MUSICMANAGER_H_

#ifndef _ITF_TEMPLATESINGLETON_H_
#include "core/templateSingleton.h"
#endif //_ITF_TEMPLATESINGLETON_H_

#ifndef _ITF_SAFEARRAY_H_
#include "core/container/SafeArray.h"
#endif //_ITF_SAFEARRAY_H_

#ifndef _ITF_MUSICPART_H_
#include "engine/sound/Music/MusicPart.h"
#endif // _ITF_MUSICPART_H_

#ifndef _ITF_MUSICPARTSET_H_
#include "engine/sound/Music/MusicPartSet.h"
#endif // _ITF_MUSICPARTSET_H_

#ifndef _ITF_MUSICTREE_H_
#include "engine/blendTree/MusicTree/MusicTree.h"
#endif //_ITF_MUSICTREE_H_

#ifndef _ITF_MUSIC_H_
#include "engine/sound/Music/Music.h"
#endif // _ITF_MUSIC_H_

#ifndef ITF_TEMPLATECLIENTHANDLER_H
#include "engine/TemplateManager/TemplateClientHandler.h"
#endif // ITF_TEMPLATECLIENTHANDLER_H

#if defined (ITF_WINDOWS) || defined ( ITF_X360 ) || defined ( ITF_PS3 ) || defined ( ITF_WII ) || defined ( ITF_PS5 ) || defined ( ITF_NINTENDO ) || defined ( ITF_XBOX_SERIES )
#define MUSICMANAGER_MUSICTEST
#endif // ITF_WINDOWS


namespace raki
{
    class VoiceBase;
    class StreamingThread; 
}


namespace ITF
{

    class Music;
    class MusicInstanceContainer;
    class ITFStreamingThread;
    class SoundInstance;
    class MusicManager_Template : public TemplateObj
    {
        DECLARE_OBJECT_CHILD_RTTI(MusicManager_Template, TemplateObj,2068797502);
        DECLARE_SERIALIZE()

    public:
        MusicManager_Template()
            : Super()
        {}
        MusicManager_Template(const Path& _path)
            : Super(_path)
        {}

        virtual bbool onTemplateLoaded();

        const SafeArray <InputDesc>&    getInputList() const { return m_inputList; }

        const MusicTree_Template * getMusicTreeTemplate() const { return &m_musicTree; }
        const MusicPartSet_Template * getMusicPartSetTemplate() const { return &m_musicPartSet; }

    private:

        SafeArray <InputDesc>                   m_inputList;

        MusicPartSet_Template                   m_musicPartSet;
        MusicTree_Template                      m_musicTree;
    };


    class MusicManager 
        : public IRTTIObject
        , public IEventListener
        , public TemplateSingleton<MusicManager>
    {
        DECLARE_OBJECT_CHILD_RTTI(MusicManager,IRTTIObject,4073580154);
    public:

        MusicManager();
        virtual ~MusicManager();

        void                                init();
        void                                update( f32 _deltaTime );
        void                                musicInstanceContainerUpdate( f32 _deltaTime );

        u32                                 createMusicInstance( Music * _music, const StringID _busName );
        u32                                 createMusicInstanceSimple( MusicSimple * _music, const StringID _busName );

        bbool                               canCreateMusicInstanceSimple( const u32 _nbChannels );

        void                                destroyMusicInstance( const u32 _metronomeType );
        void                                destroyMusicInstance( const StringID _ambianceName );
        void                                notifyEnd(const u32 _metronomeType, const MusicPart* _musicPart);
        void                                notifyDuration(const u32 _metronomeType, const MusicPart* _musicPart, f32 duration);

        void                                play( const u32 _metronomeType, const StringID _node, const f32 _fadeTime = 0.0f, const f32 _volume = 1.0f, const u32 _playOnNext = U32_INVALID, const bbool _stopAndPlay = bfalse );
        void                                play( const StringID _ambianceName, const f32 _fadeTime = 0.0f, const f32 _volume = 1.0f, const bbool _loop = btrue );
        void                                stop( const u32 _metronomeType, const f32 _fadeTime = 0.0f );
        void                                stop( const StringID _ambianceName, const f32 _fadeTime = 0.0f );
        void                                stopAll( f32 _fadeTime = 0.0f );

        bbool                               playSyncWithMetronome( const u32 _handle, const u32 _metronomeType, const u32 _playOnNextParam, const f32 _volume );
#if defined(ITF_SUPPORT_RAKI)
        bbool                               playSyncWithMetronome( raki::VoiceBase * _voice, const u32 _metronomeType, const u32 _playOnNextParam, const f32 _volume );
#else
        bbool                               playSyncWithMetronome(SoundInstance* _voice, const u32 _metronomeType, const u32 _playOnNextParam, const f32 _volume);

#endif

        void                                muteAll(bbool _mute);
        void                                muteAllBut(bbool _mute, u32 _metronomeType);
        bbool                               isMuted();
        bbool                               isMuted( const u32 _metronomeType );

        void                                setLinearVolume( const u32 _metronomeType, const f32 _linearVolume );
        void                                setLinearVolume( const StringID _ambianceName, const f32 _linearVolume );
        f32                                 getLinearVolume( const u32 _metronomeType );

        // pause and resume apply to all music instances 
        void                                pause(bbool _force = bfalse);
        void                                resume();
        ITF_INLINE bbool                    isPaused() const { return m_paused; }

        virtual void    onEvent( Event* _event );

        virtual void addMusicFromConfigFile(const Path& _configFile );
        void addMusic(const StringID _name, const Path& _musicPath, const StringID _busName, const StringID& _wwwiseid);
        void removeMusic( const MusicPartSet_Template * _musicPartSetTemplate, const MusicTree_Template * _musicTreeTemplate );
        void removeMusic( const StringID _name );
        bbool isMusicPlaying(const StringID _name) const;

        u32                                 getHandle(const u32 _metronomeType);
        u32                                 getHandle(const StringID _ambianceName);

        // Inputs
        void                                setInput(StringID _inputName, f32 _value );
        void                                setInput(StringID _inputName, u32 _value );
#ifdef ITF_SUPPORT_RAKI
        raki::StreamingThread *             getStreamingThread();
#endif
        void                                enableStreamingThread();
        void                                disableStreamingThread();


        const StringID                      debugGetMusicName(const u32 _metronomeType);

        const u32                           getNbStreamingInstances();

        bbool                               isPhysicallyLoaded() const;

    protected:

        void addMusic(const MusicManager_Template * _musicManagerTemplate);
       
#ifdef ITF_SUPPORT_RAKI
        ITFStreamingThread*             m_streamingThread = NULL;
#endif

        MusicInstanceContainer*         m_musicInstanceContainer;
        bbool m_paused;

#ifdef MUSICMANAGER_MUSICTEST
        typedef ITF_MAP<u32,Music*>             MusicContainer;
        MusicContainer                          m_musicArray;

        struct Ambiance
        {
            Ambiance()
                : m_music(NULL)
                , m_handle(U32_INVALID)
            {}

            MusicSimple *       m_music;
            u32                 m_handle;
        };

        typedef ITF_MAP<StringID,Ambiance>      AmbianceContainer;
        AmbianceContainer                       m_ambianceArray;

        TemplateClientHandler       m_templateClientHandler;

#endif // MUSICMANAGER_MUSICTEST
    };

#define MUSICMANAGER MusicManager::getptr()


}

#endif // _ITF_MUSICMANAGER_H_

