#ifndef _ITF_RAY_AIREDWIZARDDANCEBEHAVIOR_H_
#define _ITF_RAY_AIREDWIZARDDANCEBEHAVIOR_H_

#ifndef _ITF_RAY_AIGROUNDBASEMOVEMENTBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ground/Ray_AIGroundBaseMovementBehavior.h"
#endif //_ITF_RAY_AIGROUNDBASEMOVEMENTBEHAVIOR_H_

namespace ITF
{
    class Ray_AIRedWizardDanceBehavior : public Ray_AIGroundBaseMovementBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIRedWizardDanceBehavior, Ray_AIGroundBaseMovementBehavior,2549424634)
        DECLARE_SERIALIZE()

    public:

        Ray_AIRedWizardDanceBehavior();
        virtual ~Ray_AIRedWizardDanceBehavior();

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual bbool       isActivatedByEvent( Event* _event );
        virtual void        onActivate();
        virtual void        onDeactivate();
        virtual void        update( f32 _dt );
        virtual void        onEvent( Event* _event );

    private:

        ITF_INLINE const class Ray_AIRedWizardDanceBehavior_Template * getTemplate() const;

        bbool               m_waitingForSync;

        ObjectRef           m_target;       // the object we're currently dancing on
        StringID            m_targetBone;
        bbool               m_targetFlipped;
    };

    class Ray_AIRedWizardDanceBehavior_Template : public Ray_AIGroundBaseMovementBehavior_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIRedWizardDanceBehavior_Template,Ray_AIGroundBaseMovementBehavior_Template,1990234418);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AIRedWizardDanceBehavior);

    public:

        Ray_AIRedWizardDanceBehavior_Template();
        ~Ray_AIRedWizardDanceBehavior_Template(){}

        f32                 getSyncRatio() const { return m_syncRatio; }
        f32                 getSyncOffset() const { return m_syncOffset; }

    private:

        f32                 m_syncRatio;
        f32                 m_syncOffset;
    };

    ITF_INLINE const class Ray_AIRedWizardDanceBehavior_Template * Ray_AIRedWizardDanceBehavior::getTemplate() const
    {
        return static_cast<const class Ray_AIRedWizardDanceBehavior_Template *>(m_template);
    }
}

#endif //_ITF_RAY_AIREDWIZARDDANCEBEHAVIOR_H_