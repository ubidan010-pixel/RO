#ifndef _ITF_RAYWMSPOTCOMPONENT_H_
#define _ITF_RAYWMSPOTCOMPONENT_H_

#ifndef _ITF_UITEXTBOX_H_
#include "gameplay/components/UI/UITextBox.h"
#endif //_ITF_UITEXTBOX_H_

#ifndef _ITF_INPUTCRITERIAS_H_
#include "engine/blendTree/InputCriterias.h"
#endif //_ITF_INPUTCRITERIAS_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif //_ITF_SPAWNER_H_

#ifndef _ITF_UITEXTBOXESCOMPONENT_H_
#include "gameplay/components/UI/UITextBoxesComponent.h"
#endif //_ITF_UITEXTBOXESCOMPONENT_H_

namespace ITF
{
    class AnimatedComponent;
    class Ray_WM_FootpathComponent;

    class Ray_WM_SpotComponent : public ActorComponent, public IUpdateAnimInputCriteria
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_WM_SpotComponent, ActorComponent,3442082762)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_WM_SpotComponent();
        virtual ~Ray_WM_SpotComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return btrue; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onSceneActive();
        virtual void        Update( f32 _dt );
        virtual void        onEvent(Event * _event);
        virtual void        Draw();

#ifdef ITF_SUPPORT_EDITOR
        virtual void        drawEdit( class ActorDrawEditInterface* drawInterface, u32 _flags ) const;
#endif

        virtual void        updateAnimInput();

        virtual void        onEnter( bbool _changeMusic = bfalse );
        virtual void        onExit();
        virtual void        onAction() {}
        virtual void        onConnection();

        ITF_INLINE void             setState(ESpot_State _state) { m_state = _state; }
        ITF_INLINE ESpot_State      getState() { return m_state; }
        ITF_INLINE const StringID&  getTag() const { return m_tag; }
        bbool            isAccessible();

        void                changeState( ESpot_State _newState );

        bbool               isMid() const;

        ITF_INLINE bbool    isStandSwim() const { return m_isStandSwim; }

    protected:
        void                saveState(ESpot_State _state);
        virtual void        writeInfo();

        StringID                    m_tag;
        u32                         m_activateCount;
        UITextBoxesComponent *      m_textBoxesComponent;

        AnimatedComponent*  m_animatedComponent;

    private:
        ITF_INLINE const class Ray_WM_SpotComponent_Template* getTemplate() const;

        ESpot_State         m_state;
        bbool               m_isConnected;
        bbool               m_isPlayerPresent;

        u32                 m_electoons;
        bbool               m_hasCup;
		bbool               m_isFrescoLevel;
		bbool               m_hasNoRelics;
		bbool               m_hasAllRelics;
        bbool               m_hasPrize;

        bbool               m_is3Slots;
        bbool               m_isNoSlot;

        bbool               m_isStandSwim;

        Ray_WM_FootpathComponent* m_footpathComponent;
    };


    //---------------------------------------------------------------------------------------------------

    class Ray_WM_SpotComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_WM_SpotComponent_Template, TemplateActorComponent,655616801);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_WM_SpotComponent);

    public:

        Ray_WM_SpotComponent_Template()
            : m_closedAnim()
            , m_newAnim()
            , m_cannotEnterAnim()
            , m_openAnim()
            , m_completedAnim()
            , m_isMid(bfalse)
        {}
        virtual ~Ray_WM_SpotComponent_Template() {}

        ITF_INLINE const StringID&  getClosedAnim() const { return m_closedAnim; }
        ITF_INLINE const StringID&  getNewAnim() const { return m_newAnim; }
        ITF_INLINE const StringID&  getCannotEnterAnim() const { return m_cannotEnterAnim; }
        ITF_INLINE const StringID&  getOpenAnim() const { return m_openAnim; }
        ITF_INLINE const StringID&  getCompletedAnim() const { return m_completedAnim; }

        ITF_INLINE const StringID&      getTextNameID() const { return m_textNameID; }

        ITF_INLINE bbool isMid() const { return m_isMid; }

    private:
        
        StringID        m_closedAnim;
        StringID        m_newAnim;
        StringID        m_cannotEnterAnim;
        StringID        m_openAnim;
        StringID        m_completedAnim;

        StringID        m_textNameID;

        bbool           m_isMid;
    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const Ray_WM_SpotComponent_Template* Ray_WM_SpotComponent::getTemplate() const
    {
        return static_cast<const Ray_WM_SpotComponent_Template*>(m_template);
    }
};

#endif //_ITF_RAYWMSPOTCOMPONENT_H_
