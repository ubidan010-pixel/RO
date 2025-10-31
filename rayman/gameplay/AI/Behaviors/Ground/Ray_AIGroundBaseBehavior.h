#ifndef _ITF_RAY_AIGROUNDBASEBEHAVIOR_H_
#define _ITF_RAY_AIGROUNDBASEBEHAVIOR_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

namespace ITF
{
    class Ray_AIGroundBaseBehavior : public AIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(Ray_AIGroundBaseBehavior,AIBehavior,2449527905);
        DECLARE_VALIDATE_BEHAVIOR()

    public:

        Ray_AIGroundBaseBehavior();
        virtual ~Ray_AIGroundBaseBehavior();

        virtual void                        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);

    protected:

        StickToPolylinePhysComponent*       m_physComponent;

    };
    class Ray_AIGroundBaseBehavior_Template : public TemplateAIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(Ray_AIGroundBaseBehavior_Template,TemplateAIBehavior,2056374281);

    public:

        Ray_AIGroundBaseBehavior_Template(){}
        ~Ray_AIGroundBaseBehavior_Template(){}


    private:
    };

}

#endif //_ITF_RAY_AIGROUNDBASEBEHAVIOR_H_