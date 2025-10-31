#ifndef _ITF_RAY_BUBBLEDREAMERAICOMPONENT_H_
#define _ITF_RAY_BUBBLEDREAMERAICOMPONENT_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif

namespace ITF
{




    class Ray_BubbleDreamerAIComponent : public ActorComponent, public IUpdateAnimInputCriteria
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BubbleDreamerAIComponent, ActorComponent,694843223)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_BubbleDreamerAIComponent();
        virtual ~Ray_BubbleDreamerAIComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void Update(f32 _dt);
        virtual void onEvent(Event * _event);
        virtual void onActorLoaded(Pickable::HotReloadType _tupe);
        virtual void onResourceReady();
        virtual void updateAnimInput();

        virtual void onActorClearComponents();
    private:
        ITF_INLINE const class Ray_BubbleDreamerAIComponent_Template* getTemplate() const;
        void playLine(ActorRef _user,u32 _index);
        class Ray_TalkingBubbleComponent*   getBubbleComponent() const;
        //Data
    private:


        LinkComponent * m_linkComponent;


        ActorRef m_currentUser;
        ActorRef m_lastUser;
        f32 m_timer;
        u32 m_lineIndex;
        ActorRef m_bubbleRef;
        SpawneeGenerator m_bubbleSpawner;
        u32 m_bubbleBoneIndex;
        class AnimatedComponent * m_animComponent;
        //key: player, value : min index, size 
        typedef ITF_MAP<StringID,std::pair<u32, u32> > LineMap ;
        LineMap m_lines;

        u32 m_outroLinesIndex;
        u32 m_outroLinesSize;

        bbool m_caged;

        ActorRefList m_currentUsers;
    };



    class Ray_BubbleDreamerAIComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BubbleDreamerAIComponent_Template,TemplateActorComponent,1952946068);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_BubbleDreamerAIComponent);

    public:

        Ray_BubbleDreamerAIComponent_Template();
        virtual ~Ray_BubbleDreamerAIComponent_Template();

        struct Dialog
        {
            DECLARE_SERIALIZE();
            StringID m_player;
            SafeArray<LocalisationId> m_lines;
        };


        ITF_INLINE f32 getUserTime() const { return m_userTime; }
        ITF_INLINE const ITF_VECTOR<Dialog> & getDialogs() const { return m_dialogs; }
        ITF_INLINE const SafeArray<LocalisationId> & getOutroLines() const { return m_outroLines; }
        ITF_INLINE const StringID&  getBubbleBone() const { return m_bubbleBone; }
        ITF_INLINE const Path&      getBubblePath() const { return m_bubbleAct; }
        ITF_INLINE ITF::StringID getCageStandAnim() const { return m_cageStandAnim; }
        ITF_INLINE ITF::StringID getCagePullAnim() const { return m_cagePullAnim; }
        ITF_INLINE ITF::StringID getFreeStandAnim() const { return m_freeStandAnim; }
        ITF_INLINE ITF::StringID getFreePullAnim() const { return m_freePullAnim; }
        ITF_INLINE ITF::bbool getCaged() const { return m_caged; }
        ITF_INLINE f32 getBubbleZOffset() const { return m_bubbleZOffset; }
    private:
        ITF_VECTOR<Dialog> m_dialogs;
        SafeArray<LocalisationId> m_outroLines;
        //Data
        f32 m_userTime;
        StringID                                m_bubbleBone;
        Path                                    m_bubbleAct;
        StringID                                m_cageStandAnim;
        StringID                                m_cagePullAnim;
        StringID                                m_freeStandAnim;
        StringID                                m_freePullAnim;
        bbool                                   m_caged;
        f32                                     m_bubbleZOffset; 
    };


    ITF_INLINE const class Ray_BubbleDreamerAIComponent_Template* Ray_BubbleDreamerAIComponent::getTemplate() const
    {
        return static_cast<const class Ray_BubbleDreamerAIComponent_Template*>(m_template);
    }

}

#endif //_ITF_RAY_BUBBLEDREAMERAICOMPONENT_H_
