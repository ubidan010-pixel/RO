#ifndef _ITF_RAY_AIBTORDERCOMPONENT_H_
#define _ITF_RAY_AIBTORDERCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_RAY_AIBTEVENTS_H_
#include "rayman/gameplay/ai/BTs/Ray_AIBTEvents.h"
#endif //_ITF_RAY_AIBTEVENTS_H_

namespace ITF
{
    class Ray_AIBTOrderComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIBTOrderComponent, ActorComponent,3178466555)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_AIBTOrderComponent();
        virtual ~Ray_AIBTOrderComponent();

        virtual bbool                   needsUpdate() const { return btrue; }
        virtual bbool                   needsDraw() const { return bfalse; }
        virtual bbool                   needsDraw2D() const { return bfalse; }

        virtual void                    onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                    Update( f32 _dt );
        virtual void                    onEvent( Event* _event );

    protected:

        const class Ray_AIBTOrderComponent_Template* getTemplate() const;

        void                            setDisabled( bbool _val );
        void                            sendOrders();

        class DetectorComponent*        m_detector;

        Ray_EventAIOrderBT              m_order;
        Ray_EventAIBTSetDetectionArea   m_detectionArea;

        ActorRefList                    m_actorsInside;
        bbool                           m_disabled;
    };

    //////////////////////////////////////////////////////////////////////////
    class Ray_AIBTOrderComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIBTOrderComponent_Template,TemplateActorComponent,1269576061);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_AIBTOrderComponent);

    public:

        Ray_AIBTOrderComponent_Template();
        virtual ~Ray_AIBTOrderComponent_Template();

        BTAIOrderType                   getType() const { return m_type; }
        const StringID&                 getDetectionArea() const { return m_detectionArea; }
        bbool                           getRemoveOnExit() const { return m_removeOnExit; }

    private:

        BTAIOrderType                   m_type;
        StringID                        m_detectionArea;
        bbool                           m_removeOnExit;
    };

    ITF_INLINE const Ray_AIBTOrderComponent_Template* Ray_AIBTOrderComponent::getTemplate() const
    {
        return static_cast<const Ray_AIBTOrderComponent_Template*>(m_template);
    }
}

#endif //_ITF_RAY_AIBTORDERCOMPONENT_H_
