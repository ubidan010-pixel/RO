#ifndef _ITF_RAYWMGAMESTATSCOMPONENT_H_
#define _ITF_RAYWMGAMESTATSCOMPONENT_H_

#ifndef _ITF_UITEXTBOX_H_
#include "gameplay/components/UI/UITextBox.h"
#endif //_ITF_UITEXTBOX_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif //_ITF_SPAWNER_H_

namespace ITF
{
    enum ScoreType
    {
        SCORE_TYPE_CAGE = 0,
        SCORE_TYPE_LUMS,
        SCORE_TYPE_STONE,
        SCORE_TYPE_COUNT,
    };

    class Ray_WM_GameStatsComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_WM_GameStatsComponent, ActorComponent,1541137795);
        DECLARE_SERIALIZE();

    public:

        Ray_WM_GameStatsComponent();
        virtual ~Ray_WM_GameStatsComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return btrue; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onBecomeActive();
        virtual void        onEvent( Event* _event);
        virtual void        Update( f32 _dt );
        virtual void        Draw2D();

    private:
        ITF_INLINE const class Ray_WM_GameStatsComponent_Template* getTemplate() const;

        struct TextField
        {
            TextField() : m_offset2D(Vec2d::Zero), m_textActor(ObjectRef::InvalidRef) {}

            Vec2d                   m_offset2D;
            ActorRef                m_textActor;
            TextInfo                m_textInfo;
        };

        ITF_VECTOR<TextField>           m_textFields;

        SpawneeGenerator                m_spawneeGen;
    };


    //---------------------------------------------------------------------------------------------------

    class Ray_WM_GameStatsComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_WM_GameStatsComponent_Template, TemplateActorComponent,2451436856);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_WM_GameStatsComponent);

    public:

        Ray_WM_GameStatsComponent_Template()
            : m_initialFontHeight(10.0)
        {}
        virtual ~Ray_WM_GameStatsComponent_Template() {}

        ITF_INLINE const f32                    getInitialFontHeight() const { return m_initialFontHeight; }
        ITF_INLINE const Path&                  getScoreTextActorPath() const { return m_scoreTextActor; }
        ITF_INLINE const Color&                 getTextColor() const { return m_textColor; }
        ITF_INLINE const ITF_VECTOR<Vec2d>&     getOffsets2D() const { return m_scoreOffsets2D; }

    private:

        Path                    m_scoreTextActor;
        f32                     m_initialFontHeight;
        Color                   m_textColor;
        ITF_VECTOR<Vec2d>       m_scoreOffsets2D;
    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const Ray_WM_GameStatsComponent_Template* Ray_WM_GameStatsComponent::getTemplate() const
    {
        return static_cast<const Ray_WM_GameStatsComponent_Template*>(m_template);
    }
};

#endif //_ITF_RAYWMGAMESTATSCOMPONENT_H_