#ifndef _ITF_RAY_FIGHTMANAGER_H_
#define _ITF_RAY_FIGHTMANAGER_H_

namespace ITF
{
    #define RAY_FIGHTMANAGER Ray_FightManager::get()

    class Ray_FightManager
    {
        static Ray_FightManager*    s_instance;
    public:

        static void                 create()    { s_instance = newAlloc(mId_Gameplay, Ray_FightManager()); }
        static void                 destroy()   { SF_DEL(s_instance); }
        static Ray_FightManager*    get()       { return s_instance; }  //ITF_ASSERT_MSG(s_instance, "Ray_FightManager not ready, be sure to call Ray_FightManager::create() !");


        Ray_FightManager();
        ~Ray_FightManager();

        void            init();
        void            update( f32 _dt );

        void            requestAttack( ActorRef _attacker, ActorRef _victim, ObjectRef _swimmingPolyline = ITF_INVALID_OBJREF );
        u32             getLastFrameRequestedAttackersCount( ActorRef _victim );
        u32             getLastFrameRequestedAttackersCount( ActorRef _victim, ObjectRef _swimmingPolyline );
        bbool           removeFromLastFrameRequestedAttackers( ActorRef _attacker, ActorRef _victim ); // changing target

        // public for qsort only
        struct AttackerData
        {
            AttackerData()
                : m_attackCount(0)
            {}

            ActorRef    m_attacker;
            u32         m_attackCount;
            ObjectRef   m_swimmingPolyline; // for piranhas only
            //u32         m_priority;
            // ATM attacks are prioritized by lowest attackCount;
            // other rules could be used... by distance? violence?
            // who decides, attacker or manager?

            ITF_INLINE bool operator==(const AttackerData& _other) const
            {
                return (m_attacker == _other.m_attacker);
            }
        };
        typedef SafeArray<AttackerData> AttackerList;

    private:

        struct VictimData
        {
            VictimData()
                : m_timer(s_minTimeBetweenAttacks)
            {}

            ActorRef        m_victim;
            AttackerList    m_requestedAttacks;
            AttackerList    m_previousAttackers; // the actors that already done an attack
            
            AttackerList    m_lastFrameRequestedAttacks;
            f32             m_timer;

            void            update(f32 _dt);
        };
        typedef ITF_VECTOR<VictimData> VictimList;

        VictimList      m_pendingAttacks;

        static f32      s_minTimeBetweenAttacks;
    };
}

#endif //_ITF_RAY_FIGHTMANAGER_H_

