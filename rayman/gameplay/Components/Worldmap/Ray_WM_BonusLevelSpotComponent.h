#ifndef _ITF_RAYWMBONUSLEVELSPOTCOMPONENT_H_
#define _ITF_RAYWMBONUSLEVELSPOTCOMPONENT_H_

#ifndef _ITF_RAYWMLEVELSPOTCOMPONENT_H_
#include "rayman/gameplay/Components/WorldMap/Ray_WM_LevelSpotComponent.h"
#endif //_ITF_RAYWMLEVELSPOTCOMPONENT_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif

namespace ITF
{
    class Ray_WM_BonusLevelSpotComponent : public Ray_WM_LevelSpotComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_WM_BonusLevelSpotComponent, Ray_WM_LevelSpotComponent,3994185942)
        DECLARE_SERIALIZE()

    public:

        Ray_WM_BonusLevelSpotComponent();
        virtual ~Ray_WM_BonusLevelSpotComponent(){}

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onBecomeActive();
        virtual void        onBecomeInactive();
        virtual void        onSceneActive();
        virtual void        Update( f32 _dt );

        virtual void        onAction();
        virtual void        onConnection();

        virtual void        onEnter(bbool _changeMusic = bfalse);
        virtual void        onExit();

        virtual void        onActorClearComponents();

    protected:


    private:
        ITF_INLINE const class Ray_WM_BonusLevelSpotComponent_Template* getTemplate() const;
        class Ray_TalkingBubbleComponent*   getBubbleComponent() const;
        class Ray_WM_PanelComponent*        getPanelComponent() const;
        Actor * getBlocker();
        void activateBlocker(bbool _activate);
        ITF_INLINE ITF::Vec2d getOffset() const { return m_offset; }
        ITF_INLINE f32 getBubbleZOffset() const { return m_bubbleZOffset; }

        void pauseCamMod(bbool _pause);

        class LinkComponent * m_linkComponent;

        bbool               m_firstUpdate;
        ActorRef            m_bubbleRef;
        ActorRef            m_panelRef;
        SpawneeGenerator    m_bubbleSpawner;
        SpawneeGenerator    m_panelSpawnee;


        u32 m_blockedLineIndexStart;
        u32 m_openLineIndexStart;

        MarsagliaSeeder m_seeder;
        Vec2d          m_offset;
        f32             m_bubbleZOffset;
        bbool           m_blockerActive;
        SafeArray<LocalisationId> m_blockedLines;
        SafeArray<LocalisationId> m_openedLines;
        u32 m_currentLineIndex;
    };


    //---------------------------------------------------------------------------------------------------

    class Ray_WM_BonusLevelSpotComponent_Template : public Ray_WM_LevelSpotComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_WM_BonusLevelSpotComponent_Template, Ray_WM_LevelSpotComponent_Template,995170804);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_WM_BonusLevelSpotComponent);

    public:

        Ray_WM_BonusLevelSpotComponent_Template();
        virtual ~Ray_WM_BonusLevelSpotComponent_Template() {}
        ITF_INLINE const Path&      getBubblePath() const { return m_bubbleAct; }
        ITF_INLINE const Path&      getPanelPath() const { return m_panelAct; }
        ITF_INLINE const StringID&  getPanelBone() const { return m_panelBone; }
        ITF_INLINE const Vec3d&     getPanelOffset() const { return m_panelOffset; }
        ITF_INLINE const SafeArray<LocalisationId> & getBlockedLines() const { return m_blockedLines; }
        ITF_INLINE const SafeArray<LocalisationId> & getOpenedLines() const { return m_openedLines; }
    private:
        Path                        m_bubbleAct;
        Path                        m_panelAct;
        StringID                    m_panelBone;
        Vec3d                       m_panelOffset;
        SafeArray<LocalisationId>   m_blockedLines;
        SafeArray<LocalisationId>   m_openedLines;
    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const Ray_WM_BonusLevelSpotComponent_Template* Ray_WM_BonusLevelSpotComponent::getTemplate() const
    {
        return static_cast<const Ray_WM_BonusLevelSpotComponent_Template*>(m_template);
    }
};

#endif //_ITF_RAYWMBONUSLEVELSPOTCOMPONENT_H_
