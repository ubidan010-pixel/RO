#ifndef __RAY_AIJANODROAMINGBASEBEHAVIOR_H__
#define __RAY_AIJANODROAMINGBASEBEHAVIOR_H__

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_RAY_AIBOUNCEUTILITY_H_
#include "rayman/gameplay/ai/Behaviors/Ray_AIBounceUtility.h"
#endif

namespace ITF
{
    class StickToPolylinePhysComponent;
    class AIPlayAnimAction;

    ///////////////////////////////////////////////////////////////////////////////////////////
    class Ray_AIJanodRoamingBaseBehavior : public AIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIJanodRoamingBaseBehavior, AIBehavior,112329065);
        DECLARE_SERIALIZE()
        

    public:
        virtual void                        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                        update( f32 _delta  );
        virtual void                        onActivate();
        virtual void                        onDeactivate();
        virtual void                        onEvent(Event * _event);

        virtual void                        attackEnnemies();
        virtual void                        sendStim(ObjectRef _victimRef);
        bbool                               wasCrushedRecently();

        ITF_INLINE const class Ray_AIJanodRoamingBaseBehavior_Template * getTemplate() const;
    };


    class Ray_AIJanodRoamingBaseBehavior_Template : public TemplateAIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIJanodRoamingBaseBehavior_Template,TemplateAIBehavior,2536804209);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AIJanodRoamingBaseBehavior);

    public:

        Ray_AIJanodRoamingBaseBehavior_Template() :
            m_hitLevel(1)
          , m_bristlingTime(2.f)

        {}
        ~Ray_AIJanodRoamingBaseBehavior_Template() {}

        u32                 getHitLevel() const { return m_hitLevel; }
        f32                 getBristlingTime() const { return m_bristlingTime; }

    private:
        u32                 m_hitLevel;
        f32                 m_bristlingTime;
    };

    ITF_INLINE const class Ray_AIJanodRoamingBaseBehavior_Template * Ray_AIJanodRoamingBaseBehavior::getTemplate() const
    {
        return static_cast<const class Ray_AIJanodRoamingBaseBehavior_Template *>(m_template);
    }

}


#endif