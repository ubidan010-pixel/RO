#ifndef _ITF_CINEMATICMANAGER_H_
#define _ITF_CINEMATICMANAGER_H_

#ifndef _ITF_GAMEPLAYTYPES_H_
#include "gameplay/GameplayTypes.h"
#endif //_ITF_GAMEPLAYTYPES_H_

#define MAX_SEQUENCE_CHANNELS 4


namespace ITF
{
    struct SequencePlayers
    {
        SequencePlayers()
            : m_isActive(bfalse)
            , m_angle(0.f)
            , m_isDead(bfalse)
        { }

        bbool       m_isActive;

        // we do not allow changing scale
        Vec3d       m_pos;
        f32         m_angle;
        bbool       m_isDead;
    };


    #define CINEMATICMANAGER CinematicManager::get()

    typedef FixedArray<u32, MAX_SEQUENCE_CHANNELS> PlayersChannels;

    class CinematicManager : public IEventListener
    {
        static CinematicManager*      s_instance;

    public:

        static void                 create()    { s_instance = newAlloc(mId_Singleton, CinematicManager()); }
        static void                 destroy()   { SF_DEL(s_instance); }
        static CinematicManager*    get()       { return s_instance; } 
        

        CinematicManager();
        virtual ~CinematicManager();

        ITF_INLINE bbool getLockPlayers() const { return m_lockPlayersCount > 0; }
        ITF_INLINE bbool getIsCameraLocked() const { return m_cameraSubjectsCount > 0; }
        virtual    void  onEvent       (Event * _event);
                   void  clear();

                   void  sendEventSequenceActorActivate(bbool _activate);
                   void  sendEventSequenceLosckInputs(bbool _activate, bbool _revive);

                   void resetPlayersChannels();
                   u32  getPlayerChannel(u32 _player)               { return m_playerToChannelIndex[_player];}
                   void setPlayerChannel(u32 _player, u32 _channel) { m_playerToChannelIndex[_player] = _channel; m_needRecomputeChannels = btrue; }

        void                                           controlCamera();
        bbool                                          releaseCamera(bbool _teleport);

        // channel
        void                                           disactiveChannel    (const StringID & _channel);
        bbool                                          isActiveChannel     (const StringID & _channel);
        void                                           recomputeChannels() { m_needRecomputeChannels =  btrue; }
        void                                           computeInactiveChannels();
        const ITF_VECTOR<StringID> &                   getInactiveChannels();
        const ITF_INLINE StringID &                    getPlayerChannelID(u32 i)
        {
            if (!m_playerChannels.size()) 
                computeInactiveChannels();  
            return m_playerChannels[i];
        }
        
        const ITF_INLINE ITF_VECTOR<SequencePlayers> & getPlayersInfoList() { return m_playersInfo; }
        u32                                            getPlayersInfoCount() { return m_playersInfo.size(); }
        SequencePlayers &                              getPlayersInfoAtIndex(u32 _index) { return m_playersInfo[_index]; }
        void                                           disableSequencePlayers();
        void                                           enableSequencePlayers(bbool _reactivateAlive, bbool _reactivateActors = btrue);
        


    private:
        u32                         m_lockPlayersCount;
        PlayersChannels             m_playerToChannelIndex;
        
        ITF_VECTOR<StringID>        m_playerChannels;
        ITF_VECTOR<StringID>        m_inactiveChannels;
        bbool                       m_needRecomputeChannels;

        ITF_VECTOR<SequencePlayers> m_playersInfo;
        u32                         m_playersInfoCount;

        ActorRefList                m_cameraSubjects;
        i32                         m_cameraSubjectsCount;
    };

};

#endif //_ITF_CINEMATICMANAGER_H_
