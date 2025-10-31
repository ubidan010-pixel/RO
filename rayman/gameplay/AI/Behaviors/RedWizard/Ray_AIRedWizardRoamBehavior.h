#ifndef _ITF_RAY_AIREDWIZARDROAMBEHAVIOR_H_
#define _ITF_RAY_AIREDWIZARDROAMBEHAVIOR_H_

#ifndef _ITF_RAY_AIGROUNDROAMBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ground/Ray_AIGroundRoamBehavior.h"
#endif //_ITF_RAY_AIGROUNDROAMBEHAVIOR_H_

namespace ITF
{
    class Ray_AIRedWizardRoamBehavior : public Ray_AIGroundRoamBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIRedWizardRoamBehavior, Ray_AIGroundRoamBehavior,1931990579)
        DECLARE_VALIDATE_BEHAVIOR()

    public:

        Ray_AIRedWizardRoamBehavior();
        virtual ~Ray_AIRedWizardRoamBehavior();

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onActivate();
        virtual void        onActionFinished();

    protected:

        virtual bbool       updateMove( f32 _dt );

    private:

        ITF_INLINE const class Ray_AIRedWizardRoamBehavior_Template * getTemplate() const;

        void                postLoad();
        void                clear();

        bbool               checkGreet();
        void                startGreet();
        void                onActionFinishedGreet();

        AIAction*           m_greet;                // silly pause animation when meeting a friendly
        f32                 m_greetTimer;
    };

    class Ray_AIRedWizardRoamBehavior_Template : public Ray_AIGroundRoamBehavior_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIRedWizardRoamBehavior_Template,Ray_AIGroundRoamBehavior_Template,3383994418);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AIRedWizardRoamBehavior);

    public:

        Ray_AIRedWizardRoamBehavior_Template();
        ~Ray_AIRedWizardRoamBehavior_Template();

        const AIAction_Template*        getGreet() const { return m_greet; }
        f32                             getGreetRange() const { return m_greetRange; }
        f32                             getGreetCooldown() const { return m_greetCooldown; }

    private:

        AIAction_Template*  m_greet;                // silly pause animation when meeting a friendly
        f32                 m_greetRange;
        f32                 m_greetCooldown;        // don't greet too often
    };

    ITF_INLINE const class Ray_AIRedWizardRoamBehavior_Template * Ray_AIRedWizardRoamBehavior::getTemplate() const
    {
        return static_cast<const class Ray_AIRedWizardRoamBehavior_Template *>(m_template);
    }
}

#endif //_ITF_RAY_AIREDWIZARDROAMBEHAVIOR_H_