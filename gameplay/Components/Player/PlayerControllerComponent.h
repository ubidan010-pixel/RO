#ifndef _ITF_PLAYERCONTROLLERCOMPONENT_H_
#define _ITF_PLAYERCONTROLLERCOMPONENT_H_

#ifndef _ITF_PLAYERCONTROLLERSTATE_H_
#include "gameplay/Components/Player/PlayerControllerState.h"
#endif //_ITF_PLAYERCONTROLLERSTATE_H_

#ifndef _ITF_PHYSSHAPES_H_
#include "engine/physics/PhysShapes.h"
#endif //_ITF_PHYSSHAPES_H_

#ifndef _ITF_PHYSTYPES_H_
#include "engine/physics/PhysTypes.h"
#endif //_ITF_PHYSTYPES_H_

#ifndef _ITF_GAMEPLAYTYPES_H_
#include "gameplay/GameplayTypes.h"
#endif //_ITF_GAMEPLAYTYPES_H_

#ifndef _ITF_INPUTCRITERIAS_H_
#include "engine/blendTree/InputCriterias.h"
#endif //_ITF_INPUTCRITERIAS_H_

namespace ITF
{
    class PhysPhantom;
    class EventCollide;
    class EventPadRumbleStart;
    class EventPadRumbleStop;
    class SoundComponent;
    class FxBankComponent;

    class PlayerControllerComponent : public ActorComponent, IUpdateAnimInputCriteria, IUpdateSoundInputCriteria, IUpdateFxInputCriteria
    {
        DECLARE_OBJECT_CHILD_RTTI(PlayerControllerComponent, ActorComponent,1225958368);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        PlayerControllerComponent();
        virtual ~PlayerControllerComponent();

        virtual bbool               needsUpdate() const { return btrue; }
        virtual bbool               needsDraw() const { return bfalse; }
        virtual bbool               needsDraw2D() const { return bfalse; }

        virtual void                onBecomeActive();
        virtual void                onBecomeInactive();
        virtual void                onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                onCheckpointLoaded();
        virtual void                onActorClearComponents();

        virtual void                Update( f32 _deltaTime );
        virtual void                updateAnimInput() {}
        virtual void                updateSoundInput() {}
        virtual void                updateFxInput() {}
        virtual void                onEvent( Event* _event);
        virtual void                changeState( PlayerControllerState* _newState);

        virtual void                onDepthChanged( f32 _oldZ, f32 _newZ );

        void                        updateCurrentState( f32 _deltaTime );

        ITF_INLINE u32              getPlayerIndex() const { return m_playerIndex; }

        virtual bbool               isDead() const { return bfalse; }
        virtual bbool               isDetectable( const ActorRef _detector ) const { return btrue; }
        ITF_INLINE PhysPhantom*     getPhantom() const { return m_phantom; }

        ITF_INLINE Faction          getFaction() const { return m_curentFaction;}
        void                        setFaction(Faction _newFaction);


#ifdef ITF_SUPPORT_EDITOR
        virtual void                drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags  ) const;
        virtual void                onEditorMove(bbool _modifyInitialPos = btrue);
#endif // ITF_SUPPORT_EDITOR

    protected:
        
        virtual bbool               internalOnEvent( class Event* _event );

        const class PlayerControllerComponent_Template* getTemplate() const;

        void                        createPhantom();
        void                        updatePhantom();
        void                        setCurrentPhantomShape( const PhysShape* _shape );

        PhysShape*                  getCurrentPhantomShape() const { return m_currentPhantomShape; }

        virtual void                registerDatas();
        virtual void                unregisterDatas();
        void                        enablePhantom( bbool _val );

        virtual void                processTeleport( EventTeleport* _eventTeleport );
        virtual void                processPlayerIndexChange( class EventPlayerIndexChange* _playerIndex );

        // check if we collide with the other shape
        void                        processCollision(EventCollide* _collide);
        
        void                        processPadRumbleStart(EventPadRumbleStart* _padRumbleStart);
        void                        processPadRumbleStop(EventPadRumbleStop* _padRumbleStop);

        StickToPolylinePhysComponent*   m_characterPhys;
        AnimatedComponent*              m_animComponent;
        FXControllerComponent*          m_fxController;
        SoundComponent*                 m_soundComponent;
        FxBankComponent*                m_fxBankComponent;

        // Phantom data for receiving stims from the world
        PhysPhantom*                m_phantom;                          // Phantom for receiving stims from the game
        PhysShape*                  m_currentPhantomShape;              // The current shape used on the phantom

        SafeArray<PlayerControllerState*>   m_stateList;
        PlayerControllerState*              m_currentState;
        PlayerControllerState*              m_prevState;                // Temporary variables set during changeState()
        PlayerControllerState*              m_nextState;                // Temporary variables set during changeState()

        u32                         m_playerIndex;                      // The player index (also joystick index)
        bbool                       m_objectsRegistered;
        bbool                       m_phantomEnabled;                   // If the phantom is enabled

        Faction                     m_curentFaction;


    };

    //---------------------------------------------------------------------------------------------------

    class PlayerControllerComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(PlayerControllerComponent_Template,TemplateActorComponent,107300929);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(PlayerControllerComponent);

    public:

        PlayerControllerComponent_Template();
        ~PlayerControllerComponent_Template() { SF_DEL(m_phantomShape); }

        PhysShape*                  getPhantomShape() const { return m_phantomShape; }
        Faction                     getFaction() const { return m_faction; }
        Faction                     getDeadSoulFaction() const { return m_deadSoulFaction; }

    private:

        PhysShape*                  m_phantomShape;                     // The shape used by the phantom
        Faction                     m_faction;
        Faction                     m_deadSoulFaction;

    };

    ITF_INLINE const PlayerControllerComponent_Template* PlayerControllerComponent::getTemplate() const
    {
        return static_cast<const PlayerControllerComponent_Template*>(m_template);
    }

} // namespace ITF

#endif //_ITF_PLAYERCONTROLLERCOMPONENT_H_
