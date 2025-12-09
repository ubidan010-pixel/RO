#ifndef _ITF_MUSICINSTANCECONTAINER_H_
#define _ITF_MUSICINSTANCECONTAINER_H_





namespace ITF
{

    class Music;
    class MusicSimple;
    class MusicInstance;
    class MusicInstanceSimple;
    class MusicPart;

    class MusicInstanceContainer
    {
    public:

        MusicInstanceContainer();
        ~MusicInstanceContainer();

        void                        update( f32 _dt );

        u32                                 createMusicInstance( Music * _music, const StringID _busName );
        u32                                 createMusicInstanceSimple( MusicSimple * _music, const StringID _busName );

        bbool                               canCreateMusicInstanceSimple( const u32 _nbChannels );

        void                                destroyMusicInstance( u32 _handle );
        void                                notifyEnd(u32 _handle, const MusicPart* _musicPart);
        void                                notifyDuration(u32 _handle, const MusicPart* _musicPart, f32 duration);
        void                                updateFromAudioCallback(u32 _handle, const MusicPart* _musicPart, f32 _timePositionSec, f32 _soundDurationSec);

        void                                play( const u32 _handle, const StringID _node, f32 _fadeTime = 0.0f, f32 _volume = .0f, const bbool _loop = btrue, const bbool _stopAndPlay = bfalse );
        void                                stop( const u32 _handle, f32 _fadeTime = 0.0f );
        void                                stopAll( f32 _fadeTime = 0.0f );
        void                                muteAll(bbool _mute);
        void                                mute(bbool _mute, u32 _handle);
        bbool                               isMuted();
        bbool                               isMuted(const u32 _handle);

        void                                setLinearVolume( const u32 _handle, const f32 _linearVolume );
        f32                                 getLinearVolume( const u32 _handle );
        // Inputs
        void                                setInput(StringID _inputName, u32 _value );
        void                                setInput(StringID _inputName, f32 _value );

        // pause and resume apply to all music instances 
        void                                pause(bbool _force = bfalse);
        void                                resume();

        u32                                 getHandle(const u32 _musicType);

        const StringID                      debugGetMusicName(const u32 _handle);

        const u32                           getNbStreamingInstances();

        ITF_INLINE MusicInstance *          getMusicInstance( u32 _handle );

    private:

        ITF_INLINE void                     removeHandleFromPausedInstanceList(const u32 _handle);

        struct HandleInstanceStruct
        {
            HandleInstanceStruct( u32 _handle, MusicInstance * _musicInstance )
                : m_handle( _handle ), m_musicInstance( _musicInstance )
            {}
            u32 m_handle;
            MusicInstance * m_musicInstance;
        };

        typedef ITF_VECTOR<HandleInstanceStruct> MusicInstanceStructVector;
        MusicInstanceStructVector           m_musicInstanceArray;

        u32                                 m_nextHandle;

        ITF_VECTOR<MusicInstance*>          m_freeMusicInstanceArray;
        ITF_VECTOR<MusicInstanceSimple*>    m_freeStereoAmbianceInstanceArray;
        ITF_VECTOR<MusicInstanceSimple*>    m_freeMonoVoiceInstanceArray;

        SafeArray<u32>                      m_pausedInstanceList;   //used to store and resume paused instances
    };


    MusicInstance * MusicInstanceContainer::getMusicInstance( u32 _handle )
    {
        for ( u32 u = 0 ; u < m_musicInstanceArray.size() ; ++u )
        {
            if ( m_musicInstanceArray[ u ].m_handle == _handle )
                return m_musicInstanceArray[ u ].m_musicInstance;
        }

        return NULL;
    }

    ITF_INLINE void MusicInstanceContainer::removeHandleFromPausedInstanceList(const u32 _handle)
    {

        for (u32 index = 0; index < m_pausedInstanceList.size(); ++index)
        {
            const u32 instanceIndex = m_pausedInstanceList[index];
            if(instanceIndex<m_musicInstanceArray.size() && _handle == m_musicInstanceArray[instanceIndex].m_handle)
            {
                m_pausedInstanceList.eraseNoOrder(index);
                return;
            }
                
        }

    }


}

#endif //_ITF_MUSICINSTANCECONTAINER_H_
