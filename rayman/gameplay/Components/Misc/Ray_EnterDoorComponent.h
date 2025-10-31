#ifndef _ITF_RAY_ENTERDOORCOMPONENT_H_
#define _ITF_RAY_ENTERDOORCOMPONENT_H_

#ifndef _ITF_TRIGGERCOMPONENT_H_
#include "gameplay/Components/Trigger/TriggerComponent.h"
#endif //_ITF_TRIGGERCOMPONENT_H_

namespace ITF
{
    class Ray_EnterDoorComponent : public TriggerComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_EnterDoorComponent, TriggerComponent,34510304);

    public:
        DECLARE_SERIALIZE()

        Ray_EnterDoorComponent();
        virtual ~Ray_EnterDoorComponent();

        virtual void                onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                Update( f32 _dt );
        virtual void                onEvent( Event * _event);
#ifdef ITF_SUPPORT_EDITOR
        virtual void                drawEdit( ActorDrawEditInterface* drawInterface, u32 _flags  ) const;
#endif // ITF_SUPPORT_EDITOR
        void                        setIsTriggerDisabled(bbool _disabled) {m_isTriggerDisabled = _disabled;}

    private: 
        ITF_INLINE const class Ray_EnterDoorComponent_Template * getTemplate() const;

        void                        triggerDoor( EventTrigger* _trigger );

        void                        setPoints();
        Vec2d                       getEntryPoint() const;
        Vec2d                       getExitPoint() const;

        Vec2d                       m_entryPointPos;
        Vec2d                       m_exitPointPos;
        bbool                       m_isTriggerDisabled;
    };

    //---------------------------------------------------------------------------------------------------

    class Ray_EnterDoorComponent_Template : public TriggerComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_EnterDoorComponent_Template,TriggerComponent_Template,2048296090);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_EnterDoorComponent);

    public:

        Ray_EnterDoorComponent_Template() : m_noEntry(bfalse) {}
        ~Ray_EnterDoorComponent_Template() {}

        const StringID&             getEntryPoint() const { return m_entryPointID; }
        const StringID&             getExitPoint() const { return m_exitPointID; }
        bbool                       getNoEntry() const { return m_noEntry; }

    private:

        StringID                    m_entryPointID;
        StringID                    m_exitPointID;
        bbool                       m_noEntry;
    };
    ITF_INLINE const class Ray_EnterDoorComponent_Template * Ray_EnterDoorComponent::getTemplate() const
    {
        return static_cast<const class Ray_EnterDoorComponent_Template *>(m_template);
    }

}

#endif //_ITF_RAY_ENTERDOORCOMPONENT_H_
