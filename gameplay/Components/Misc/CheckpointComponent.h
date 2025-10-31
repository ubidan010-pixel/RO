#ifndef _ITF_CHECKPOINTCOMPONENT_H_
#define _ITF_CHECKPOINTCOMPONENT_H_

namespace ITF
{
    class CheckpointComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(CheckpointComponent,ActorComponent,1429523170);

    public:
        DECLARE_SERIALIZE()

        CheckpointComponent();
        virtual ~CheckpointComponent();

        virtual bbool   needsUpdate() const { return bfalse; }
        virtual bbool   needsDraw() const { return bfalse; }
        virtual bbool   needsDraw2D() const { return bfalse; }
        virtual void    onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void    onEvent( Event * _event);

        virtual void    onSceneActive();
        virtual void    onSceneInactive();

        u32             getIndex() const { return  m_index; }
        bbool           getActive() const { return m_active;}
        
        virtual Vec2d   getPlayerSpawnPos( u32 _playerIndex ) const;
        virtual bbool   getPlayerSpawnFlipped( u32 _playerIndex ) const { return m_actor->isFlipped(); }
#ifdef ITF_SUPPORT_EDITOR
        virtual void    drawEdit( class ActorDrawEditInterface* /*drawInterface*/, u32 _flags ) const;
#endif // ITF_SUPPORT_EDITOR

        bbool           canSpawnPlayer() const;
        bbool           getJoinAlive() const { return m_joinAlive; }

    private:
        ITF_INLINE const class CheckpointComponent_Template* getTemplate() const;
        
        void            clear();
        virtual void    checkpointReached( ObjectRef _player );
        void            registerToManager();
        void            unregisterFromManager();

        u32                 m_index;
        bbool               m_registered;
        bbool               m_active;
        bbool               m_joinAlive;    // should extra players that join at this checkpoint start alive or dead?
    };

    //---------------------------------------------------------------------------------------------------

    class CheckpointComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(CheckpointComponent_Template,TemplateActorComponent,260764834);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(CheckpointComponent);

    public:

        CheckpointComponent_Template()
            : m_spawnPlayer(true)
        {}

        ~CheckpointComponent_Template() {}

        bbool               getSpawnPlayer() const { return m_spawnPlayer; }

    private:
        bbool               m_spawnPlayer;
    };

    ITF_INLINE const CheckpointComponent_Template* CheckpointComponent::getTemplate() const
    {
        return static_cast<const CheckpointComponent_Template*>(m_template);
    }
}

#endif // _ITF_CHECKPOINTCOMPONENT_H_
