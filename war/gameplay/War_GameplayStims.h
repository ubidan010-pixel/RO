#ifndef _ITF_WAR_GAMEPLAYSTIMS_H_
#define _ITF_WAR_GAMEPLAYSTIMS_H_

#ifndef _ITF_GAMEPLAYSTIMS_H_
#include "gameplay/GameplayStims.h"
#endif //_ITF_GAMEPLAYSTIMS_H_

#ifndef _ITF_WAR_GAMEPLAYTYPES_H_
#include "war/gameplay/War_GameplayTypes.h"
#endif //_ITF_WAR_GAMEPLAYTYPES_H_

namespace ITF
{
    class War_HitStim : public HitStim
    {
        DECLARE_OBJECT_CHILD_RTTI(War_HitStim, HitStim,3270006373)

    public:

        War_HitStim()
            : Super()
            , m_receivedHitType(WAR_RECEIVEDHITTYPE_NORMALHIT)
        {}
        virtual ~War_HitStim() {}

        War_EReceivedHitType        getReceivedHitType() const { return m_receivedHitType; }
        void                        setReceivedHitType( War_EReceivedHitType _type ) { m_receivedHitType = _type; }

    private:

        War_EReceivedHitType        m_receivedHitType;
    };
}

#endif // _ITF_WAR_GAMEPLAYSTIMS_H_