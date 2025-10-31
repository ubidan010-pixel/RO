#ifndef _ITF_INSTANCEMANAGER_H_
#define _ITF_INSTANCEMANAGER_H_




namespace raki
{
    class Format;
    class VoiceBase;
}


namespace ITF
{
    class SoundInstance;

    class InstanceManager
    {
    public:

        InstanceManager( u32 _nbInstances );

        ~InstanceManager();
#ifdef ITF_SUPPORT_RAKI
        u32 createSource( const raki::Format * _format, const StringID & _busName );
#else

        u32 createSource(const StringID& _busName, SoundInstance* inst);
#endif

        void update();
#ifdef ITF_SUPPORT_RAKI
        ITF_INLINE raki::VoiceBase* getInstance(const u32 _handle);
#else
        ITF_INLINE SoundInstance* getInstance(const u32 _handle);
#endif

        void getPlayingVoices(SafeArray<u32> & _playingVoices);
        void getPauseSensitivePlayingVoices(SafeArray<u32> & _playingVoices);

        SoundInstance* getInstanceFromInstanceIndex(const u32 _instanceIndex);


    private:

        const u32 getHandleFromInstanceIndex( const u32 _instanceIndex );

        bbool isHandleRegistered( const u32 _handle ) const;

        bbool isInstanceIndexRegistered( const u32 _instanceIndex ) const;

        u32 registerInstanceIndexAndReturnHandle( const u32 _instanceIndex, const bbool _pauseSensitive, SoundInstance* _inst = nullptr);

        ITF_INLINE const u32 getInstanceIndexFromHandle( const u32 _handle );

        ITF_INLINE const u32 getNbUsedInstances();

        struct HandleInstanceStruct
        {
            HandleInstanceStruct( u32 _handle, u32 _instanceIndex, const bbool _pauseSensitive )
                : m_handle( _handle )
                , m_instanceIndex( _instanceIndex )
                , m_pauseSensitive( _pauseSensitive )
            {}
            u32 m_handle;
            u32 m_instanceIndex;
            bbool m_pauseSensitive;
            SoundInstance* m_Instance = nullptr;
        };

#ifdef ITF_SUPPORT_RAKI
        raki::VoiceBase **          m_bufferSoundArray =NULL;
#endif
        u32                                 m_nbInstances;

        typedef ITF_VECTOR<HandleInstanceStruct> InstanceVector;
        InstanceVector                      m_usedInstanceArray;

        ITF_LIST<u32>                       m_freeInstanceArray;

        u32                                 m_nextHandle;
    };


    const u32 InstanceManager::getInstanceIndexFromHandle( const u32 _handle )
    {
        for ( InstanceVector::iterator it = m_usedInstanceArray.begin() ; it != m_usedInstanceArray.end() ; ++it )
        {
            if ( it->m_handle == _handle )
                return it->m_instanceIndex;
        }

        return (u32) -1;
    }


    const u32 InstanceManager::getNbUsedInstances()
    {
        return m_usedInstanceArray.size();
    }

#ifdef ITF_SUPPORT_RAKI
    raki::VoiceBase * InstanceManager::getInstance( const u32 _handle )
    {
        const u32 instanceIndex = getInstanceIndexFromHandle( _handle );


        if ( instanceIndex < m_nbInstances )
            return m_bufferSoundArray[ instanceIndex ];


        return NULL;
    }
#else

    SoundInstance* InstanceManager::getInstance(const u32 _handle)
    {
        const u32 instanceIndex = getInstanceIndexFromHandle(_handle);

        

        if (instanceIndex < m_nbInstances)
            return getInstanceFromInstanceIndex(instanceIndex);


        return NULL;
    }
#endif

}

#endif //_ITF_INSTANCEMANAGER_H_
