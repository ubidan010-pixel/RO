#ifndef _ITF_RAY_HUNTERHOLEAICOMPONENT_H_
#define _ITF_RAY_HUNTERHOLEAICOMPONENT_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

namespace ITF
{
    class Ray_AIHunterAttackBehavior;

    class Ray_HunterHoleAIComponent : public AIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_HunterHoleAIComponent, AIComponent,3494158620)
        DECLARE_SERIALIZE();

    public:

        Ray_HunterHoleAIComponent();
        ~Ray_HunterHoleAIComponent();

        virtual void                onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                Update( f32 _deltaTime );
        virtual void                onBehaviorFinished();
        virtual void                onEvent( Event* _event);
        virtual void                updateAnimInput();
#ifdef ITF_SUPPORT_EDITOR
        virtual void                drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const;
#endif // ITF_SUPPORT_EDITOR

    private:
        ITF_INLINE const class Ray_HunterHoleAIComponent_Template * getTemplate() const;

        AIBehavior*                     m_idleBehavior;
        Ray_AIHunterAttackBehavior*     m_attackBehavior;

        void                        onCheckpointLoaded();


        ObjectRef                   findEnemy(bbool & _closeRange) const;

    };
    //---------------------------------------------------------------------------------------------------------------------------------------

    class Ray_HunterHoleAIComponent_Template : public AIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_HunterHoleAIComponent_Template, AIComponent_Template,1634055986);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_HunterHoleAIComponent);

    public:

        Ray_HunterHoleAIComponent_Template();
        ~Ray_HunterHoleAIComponent_Template() {}

        ITF_INLINE f32               getFindEnemyRadius()  const   {return m_findEnemyRadius;}
        ITF_INLINE const AABB&       getDetectionRange()   const   {return m_detectionRange;}
        ITF_INLINE bbool             getUseRadius()        const   {return m_useRadius;}

        ITF_INLINE const class TemplateAIBehavior* getIdleBehavior() const { return m_idleBehavior; }
        ITF_INLINE const class Ray_AIHunterAttackBehavior_Template* getAttackBehavior() const { return m_attackBehavior; }
    private:

        f32                         m_findEnemyRadius;
        AABB                        m_detectionRange;
        bbool                       m_useRadius;

        class TemplateAIBehavior*                     m_idleBehavior;
        class Ray_AIHunterAttackBehavior_Template*     m_attackBehavior;
    };

    ITF_INLINE const class Ray_HunterHoleAIComponent_Template * Ray_HunterHoleAIComponent::getTemplate() const
    {
        return static_cast<const class Ray_HunterHoleAIComponent_Template *>(m_template);
    }
}

#endif //_ITF_RAY_HUNTERHOLEAICOMPONENT_H_

