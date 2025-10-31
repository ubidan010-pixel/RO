#ifndef _ITF_RAY_LIVINGSTONEHATAICOMPONENT_H_
#define _ITF_RAY_LIVINGSTONEHATAICOMPONENT_H_

#ifndef _ITF_RAY_AICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AIComponent.h"
#endif //_ITF_RAY_AICOMPONENT_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

namespace ITF
{
    class Ray_LivingstoneHatAIComponent : public Ray_AIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_LivingstoneHatAIComponent, Ray_AIComponent,3806885714)
        DECLARE_SERIALIZE();

    public:
        DECLARE_VALIDATE_COMPONENT()

        Ray_LivingstoneHatAIComponent();
        virtual ~Ray_LivingstoneHatAIComponent();

        virtual void                onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                Update( f32 _deltaTime );

        virtual void                onBehaviorFinished();

    protected:

    private:
        ITF_INLINE const class Ray_LivingstoneHatAIComponent_Template * getTemplate() const;

        // BEHAVIORS
        AIBehavior*                     m_roamBehavior;
        class Ray_AIWaterBaseBehavior*  m_floatingBehavior;
        StickToPolylinePhysComponent*   m_physComponent;

        void                startRoam(bbool _restart = bfalse);
        void                startFloating( ObjectRef _poly );

        void                updateRoam( f32 _dt );
        void                updateSwim( f32 _dt );

    };


//---------------------------------------------------------------------------------------------------------------------------------------

    class Ray_LivingstoneHatAIComponent_Template : public Ray_AIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_LivingstoneHatAIComponent_Template, Ray_AIComponent_Template,2261019325);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_LivingstoneHatAIComponent);

    public:

        Ray_LivingstoneHatAIComponent_Template();
        ~Ray_LivingstoneHatAIComponent_Template();


        ITF_INLINE const class TemplateAIBehavior* getRoamBehavior() const { return m_roamBehavior; }
        ITF_INLINE const class Ray_AIWaterBaseBehavior_Template* getFloatingBehavior() const { return m_floatingBehavior; }
    private:
        class TemplateAIBehavior*                       m_roamBehavior;
        class Ray_AIWaterBaseBehavior_Template*         m_floatingBehavior;
    };

    ITF_INLINE const class Ray_LivingstoneHatAIComponent_Template * Ray_LivingstoneHatAIComponent::getTemplate() const
    {
        return static_cast<const class Ray_LivingstoneHatAIComponent_Template *>(m_template);
    }


}

#endif //_ITF_RAY_LIVINGSTONEHATAICOMPONENT_H_
