#ifndef _ITF_RAY_WMCONTENTUNLOCKCOMPONENT_H_
#define _ITF_RAY_WMCONTENTUNLOCKCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _LOCALISATION_ID_H_
#include "core/LocalisationId.h"
#endif // _LOCALISATION_ID_H_

#ifndef _ITF_INPUTLISTENER_H_
#include "engine/zinput/ZInputListener.h"
#endif //_ITF_INPUTLISTENER_H_

namespace ITF {

//------------------------------------------------------------------------------
class Ray_WMContentUnlockComponent : public ActorComponent, IInputListener
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_WMContentUnlockComponent, ActorComponent,654430470)
    DECLARE_SERIALIZE()
    DECLARE_VALIDATE_COMPONENT()

public:
    Ray_WMContentUnlockComponent();
    virtual ~Ray_WMContentUnlockComponent();

    virtual bbool                               needsUpdate() const { return btrue; }
    virtual bbool                               needsDraw() const { return bfalse; }
    virtual bbool                               needsDraw2D() const { return bfalse; }

    virtual void                                onActorLoaded(Pickable::HotReloadType _hotReload);
    virtual void                                onActorClearComponents();
    virtual void                                onBecomeActive();
    virtual void                                onBecomeInactive();
    virtual void                                Update( f32 _dt );
    virtual void                                onEvent( class Event* _event );

    virtual void                                Receive( u32 deviceID, f32 axis, const StringID& action );

    bbool                                       isFinished() const { return m_isFinished && !m_hiding; }

#ifdef ITF_SUPPORT_EDITOR
    virtual void                                onEditorCreated( class Actor* _original );
    virtual void                                onEditorMove( bbool _modifyInitialPos );
#endif

private:

    enum EStage
    {
        STAGE_FAIRY = 0,
        STAGE_CHEST,
        STAGE_COSTUME,
        STAGE_BOSS,
        STAGE_DEADLAND,
        MAX_STAGES,
        STAGE_NONE,
    };

    struct StageSentence
    {
        LocalisationId      m_sentenceId;
        LocalisationId      m_varId;
    };

    typedef SafeArray <StageSentence> StageSentencesList;

    ITF_INLINE const class Ray_WMContentUnlockComponent_Template* getTemplate() const;

    void                                        prepareSentences();
    void                                        setNextStage();
    void                                        updatePosition();
    void                                        updateIcon();

    class InGameTextComponent*                  m_inGameTextComponent;
    class AnimatedComponent*                    m_animatedComponent;

    ActorRef                                    m_iconRef;
    Vec2d                                       m_relativePos;
    u32                                         m_electoonBefore;
    u32                                         m_electoonAfter;
    u32                                         m_teethBefore;
    u32                                         m_teethAfter;
    u32                                         m_currentStage;
    StageSentencesList                          m_stageSentences[MAX_STAGES];
    u32                                         m_sentenceIndex;
    bbool                                       m_isFinished;
    bbool                                       m_playNext;
    bbool                                       m_hiding;
};

//------------------------------------------------------------------------------
class Ray_WMContentUnlockComponent_Template : public TemplateActorComponent
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_WMContentUnlockComponent_Template, TemplateActorComponent,1150594739)
    DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_WMContentUnlockComponent);
    DECLARE_SERIALIZE();

public:
    Ray_WMContentUnlockComponent_Template();
    virtual ~Ray_WMContentUnlockComponent_Template();

    const LocalisationId&                       getFairyLocId() const { return m_fairyLocId; }
    const LocalisationId&                       getChestLocId() const { return m_chestLocId; }
    const LocalisationId&                       getCostumeLocId() const { return m_costumeLocId; }
    const LocalisationId&                       getDeadLandLocId() const { return m_deadLandLocId; }
    const LocalisationId&                       getBossJungleLocId() const { return m_bossJungleLocId; }
    const LocalisationId&                       getBossMusicLocId() const { return m_bossMusicLocId; }
    const LocalisationId&                       getBossFoodLocId() const { return m_bossFoodLocId; }
    const LocalisationId&                       getBossOceanLocId() const { return m_bossOceanLocId; }
    const Vec2d&                                getAnimSize() const { return m_animSize; }
    const Path&                                 getIconPath() const { return m_iconPath; }
    const StringID&                             getIconPoint() const { return m_iconPoint; }
    const Vec2d&                                getIconAnimSize() const { return m_iconAnimSize; }

private:

    LocalisationId                              m_fairyLocId;
    LocalisationId                              m_chestLocId;
    LocalisationId                              m_costumeLocId;
    LocalisationId                              m_deadLandLocId;
    LocalisationId                              m_bossJungleLocId;
    LocalisationId                              m_bossMusicLocId;
    LocalisationId                              m_bossFoodLocId;
    LocalisationId                              m_bossOceanLocId;
    Vec2d                                       m_animSize;
    Path                                        m_iconPath;
    StringID                                    m_iconPoint;
    Vec2d                                       m_iconAnimSize;
};

ITF_INLINE const Ray_WMContentUnlockComponent_Template* Ray_WMContentUnlockComponent::getTemplate() const { return static_cast<const Ray_WMContentUnlockComponent_Template*>(m_template); }

} // namespace ITF

#endif //_ITF_RAY_WMCONTENTUNLOCKCOMPONENT_H_
