#ifndef _ITF_RAY_AILUMS2COMPONENT_H_
#define _ITF_RAY_AILUMS2COMPONENT_H_

#ifndef _ITF_RAY_FIXEDAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_FixedAIComponent.h"
#endif //_ITF_RAY_FIXEDAICOMPONENT_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

namespace ITF
{

    class Ray_AILums2Component : public Ray_FixedAIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AILums2Component, Ray_FixedAIComponent,82903916)
        DECLARE_SERIALIZE()

    public:
        Ray_AILums2Component() : m_isTaken(bfalse)
        {}

        ~Ray_AILums2Component() {}

        bbool isTaken() const {return m_isTaken;}
        void setIsTaken(bbool _isTaken) {m_isTaken = _isTaken;}

    protected:
        bbool m_isTaken;
    };

    class Ray_AILums2Component_Template : public Ray_FixedAIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AILums2Component_Template,Ray_FixedAIComponent_Template,3392457114);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_AILums2Component);

    public:

        Ray_AILums2Component_Template() : Ray_FixedAIComponent_Template() {}
        ~Ray_AILums2Component_Template() {}
    private:
    };

}

#endif //_ITF_RAY_AILUMS2COMPONENT_H_