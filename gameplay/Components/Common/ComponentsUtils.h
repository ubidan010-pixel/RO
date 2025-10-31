#ifndef _ITF_COMPONENTSUTILS_H_
#define _ITF_COMPONENTSUTILS_H_

namespace ITF
{
    //******************************************
    //          ACTOR SPAWN BANK
    //******************************************

    // A set contain a list of actor spawn data
    class ActorSpawnBank_Template
    {
        DECLARE_SERIALIZE();
    public:
        
        class ActorSpawnData
        {
        public:
            ActorSpawnData() : m_recycle(bfalse), m_useScale( bfalse), m_flip(bfalse), m_onSpawnEvent(NULL), m_userData( 0 ) {}
            ~ActorSpawnData() { SF_DEL( m_onSpawnEvent ); }
            DECLARE_SERIALIZE();
            StringID    m_id; 
            Path        m_actorPath;
            bbool       m_recycle;
            bbool       m_useScale;
            bbool       m_flip;
            Event*      m_onSpawnEvent;
            i32         m_userData;
        };

        const ActorSpawnData *                          getActorSpawnDataFromId( const StringID & _id )const;
        const Path &                                    getActorPathFromId( const StringID & _id )const;
        ITF_INLINE const ITF_VECTOR<ActorSpawnData> &   getBankList() const     { return m_actorSpawnDataList; }
        ITF_INLINE const bbool                          isValid() const         { return ( m_actorSpawnDataList.size() > 0 ); }

    private:
        
        ITF_VECTOR<ActorSpawnData>   m_actorSpawnDataList;
    };

    
};

#endif //_ITF_COMPONENTSUTILS_H_