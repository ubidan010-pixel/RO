#ifndef _ITF_RAY_CONDITIONALTRIGGERCOMPONENT_H_
#define _ITF_RAY_CONDITIONALTRIGGERCOMPONENT_H_

#ifndef _ITF_TRIGGERCOMPONENT_H_
#include "gameplay/Components/Trigger/TriggerComponent.h"
#endif //_ITF_TRIGGERCOMPONENT_H_

namespace ITF
{
    class Ray_ConditionalTriggerComponent : public TriggerComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ConditionalTriggerComponent, TriggerComponent,3398893856)
        DECLARE_SERIALIZE()

    public:

        Ray_ConditionalTriggerComponent();
        virtual ~Ray_ConditionalTriggerComponent();
        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }
        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        Update( f32 _deltaTime );
        virtual void        onEvent( Event* _event );
        virtual void        onBecomeActive();
        virtual void        onActorRemovedFromScene(Scene* _pScene, bbool _forDeletion);
#ifdef ITF_SUPPORT_EDITOR
        virtual void        drawEdit( class ActorDrawEditInterface* _drawInterface, u32 _flags ) const;
#endif // ITF_SUPPORT_EDITOR

    protected:

        enum ConditionType
        {
            Invalid = 0,
            IsDead,
            IsNotDead,
            TriggerEvent,
            IsEnable,
            IsDisable,
            isOpen,
            isNotOpen,

            ENUM_FORCE_SIZE_32(ConditionType)
        };

        struct ParentData
        {
            ParentData() : m_condition( Invalid ), m_conditionValidated( bfalse ){}
            ObjectRef       m_parent;
            ConditionType   m_condition;
            bbool           m_conditionValidated;
        };

        ConditionType           getLinkTagConditionType( const StringID & _tag, i32 _value );
        void                    getLinkTagFromConditionType( ConditionType _conditionType, StringID & _res_tag, i32 &_res_value ) const;
        void                    checkAllConditions( bbool _forceCheck = bfalse );

        
        ObjectRefList           m_receivedTriggerEventActors;
        SafeArray<ParentData>   m_parentDataList;
    
    private:
        ITF_INLINE const class Ray_ConditionalTriggerComponent_Template* getTemplate() const;
    };


    //---------------------------------------------------------------------------------------------------

    class Ray_ConditionalTriggerComponent_Template : public TriggerComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ConditionalTriggerComponent_Template, TriggerComponent_Template,3900043245);
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_ConditionalTriggerComponent);
        DECLARE_SERIALIZE();

    public:

        Ray_ConditionalTriggerComponent_Template();
        virtual ~Ray_ConditionalTriggerComponent_Template() {}

    private:
    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const Ray_ConditionalTriggerComponent_Template* Ray_ConditionalTriggerComponent::getTemplate() const
    {
        return static_cast<const Ray_ConditionalTriggerComponent_Template*>(m_template);
    }
};

#endif //_ITF_RAY_CONDITIONALTRIGGERCOMPONENT_H_