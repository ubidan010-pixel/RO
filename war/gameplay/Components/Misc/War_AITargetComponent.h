#ifndef _ITF_WAR_AITARGETCOMPONENT_H_
#define _ITF_WAR_AITARGETCOMPONENT_H_

namespace ITF
{
    class War_AITargetComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(War_AITargetComponent, ActorComponent,3124125847)

    public:
        DECLARE_SERIALIZE();

        War_AITargetComponent();
        virtual ~War_AITargetComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded();
        virtual void        Update(f32 _deltaTime);

                //bool        canRegisterAttacker() { return m_attackers.size() < m_maxAttackerCount; }
                //void        registerAttacker(Actor* _attacker);
                //void        unregisterAttacker(Actor* _attacker);

                bbool       canGetAttacked();
                void        requestAttack(Actor* _attacker);
                void        attackPerformed(Actor* _attacker);

    private:

        u32                 m_maxAttackerCount;

        typedef ITF_VECTOR<Actor*> TargetList;
        //TargetList          m_attackers;
        TargetList          m_pendingAttacks;
    };
};

#endif //_ITF_WAR_AITARGETCOMPONENT_H_