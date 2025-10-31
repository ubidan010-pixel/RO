#ifndef _ITF_RAY_CAGEAICOMPONENT_H_
#define _ITF_RAY_CAGEAICOMPONENT_H_

#ifndef _ITF_RAY_FIXEDAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_FixedAIComponent.h"
#endif //_ITF_RAY_FIXEDAICOMPONENT_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

namespace ITF
{

    class Ray_CageAIComponent : public Ray_FixedAIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_CageAIComponent, Ray_FixedAIComponent,521364948);
        DECLARE_SERIALIZE()

    public:
        Ray_CageAIComponent() : m_cageIndex(-1), m_wasBrokenInSession(bfalse)
        {
        }

        ~Ray_CageAIComponent()
        {
        }

        i32 getCageIndex() const {return m_cageIndex;}
        bbool wasBrokenInSession() const {return m_wasBrokenInSession;}
        void setBrokenInSession(bbool _broken) {m_wasBrokenInSession = _broken;}

    protected:
        i32 m_cageIndex;
        bbool m_wasBrokenInSession;
    };

    class Ray_CageAIComponent_Template : public Ray_FixedAIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_CageAIComponent_Template,Ray_FixedAIComponent_Template,2067105519);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_CageAIComponent);

    public:

        Ray_CageAIComponent_Template() : Ray_FixedAIComponent_Template() {}
        ~Ray_CageAIComponent_Template() {}
    private:
    };

}

#endif //_ITF_RAY_CAGEAICOMPONENT_H_