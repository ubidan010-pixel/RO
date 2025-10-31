#ifndef _ITF_RAY_PLATFORMCHAINAICOMPONENT_H_
#define _ITF_RAY_PLATFORMCHAINAICOMPONENT_H_

namespace ITF
{
    class LinkComponent;
    class TweenComponent;

    class Ray_PlatformChainAIComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_PlatformChainAIComponent, ActorComponent,1486053090)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_PlatformChainAIComponent();
        virtual ~Ray_PlatformChainAIComponent();

        virtual bbool       needsUpdate() const { return bfalse; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded( Pickable::HotReloadType _hotReload );
        virtual void        onActorClearComponents();
        virtual void        onSceneActive();
        virtual void        onCheckpointLoaded();
        virtual void        onEvent( Event* _event );

    private:
        ITF_INLINE const class Ray_PlatformChainAIComponent_Template* getTemplate() const;

        LinkComponent* m_linkComponent;
        TweenComponent* m_tweenComponent;

        void sendDataToChildren() const;
        void receiveDataFromParent( EventSetTarget* eventInfo );
        Ray_PlatformChainAIComponent* getParent( const u32 _index ) const;

        void appear();
        void disappear();
        void makeChildrenAppear();
        void forceParentsToDisappear();
        void forceParentsToDisappear( u32& _depth );
        ActorRefList m_parents;
        bbool m_appeared;
    };


    //---------------------------------------------------------------------------------------------------

    class Ray_PlatformChainAIComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_PlatformChainAIComponent_Template, TemplateActorComponent,1666631592);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_PlatformChainAIComponent);

    public:

        Ray_PlatformChainAIComponent_Template()
            : m_rollbackDepth(1)
        {
        }
        virtual ~Ray_PlatformChainAIComponent_Template() {}

        ITF_INLINE u32 getRollbackDepth() const { return m_rollbackDepth; }

    private:

        u32 m_rollbackDepth;
    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const Ray_PlatformChainAIComponent_Template* Ray_PlatformChainAIComponent::getTemplate() const
    {
        return static_cast<const Ray_PlatformChainAIComponent_Template*>(m_template);
    }
};

#endif //_ITF_RAY_PLATFORMCHAINAICOMPONENT_H_