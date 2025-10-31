#ifndef _ITF_RAY_MUSICSCORESNAPCOMPONENT_H_
#define _ITF_RAY_MUSICSCORESNAPCOMPONENT_H_

namespace ITF
{
    class CurveComponent;

    class Ray_MusicScoreSnapComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_MusicScoreSnapComponent, ActorComponent,1990836487)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

        // ********************************************************************

     public:

        Ray_MusicScoreSnapComponent();
        virtual ~Ray_MusicScoreSnapComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const   { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);

                void        registerMusicScore(Actor* _actorMusicScore, bbool _disableActor = btrue);
                void        unregisterMusicScore(Actor* _actorMusicScore, bbool _enableActor = btrue);
                void        updateOpening(f32 _curDist, bbool _forced);
                void        updateClosing(f32 _curDist);
                f32         getDistOnCurve() { return m_distOnCurve; }
                u32         getNote() { return m_note; }
                bbool       getReverse() const { return m_reverse; }

                ActorRef    getParent() { return m_parent; }
                void        setInfoSnap(f32 _distOnCurve, u32 _selectedNote, f32 _intervalNote, bbool _badSnap, bbool _reverse);

    private:
           

    private:

        ITF_INLINE const class Ray_MusicScoreSnapComponent_Template*  getTemplate() const;

        ActorRef    m_parent;
        f32         m_distOnCurve;
        f32         m_intervalNote;
        bbool       m_isDeploy;
        u32         m_note;
        bbool       m_badSnap;
        bbool       m_reverse;

    };


    //*****************************************************************************
    // Template
    //*****************************************************************************

    class Ray_MusicScoreSnapComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_MusicScoreSnapComponent_Template,TemplateActorComponent,1953999640);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_MusicScoreSnapComponent);

    public:

        Ray_MusicScoreSnapComponent_Template();
        ~Ray_MusicScoreSnapComponent_Template();


    private:



    };


    const Ray_MusicScoreSnapComponent_Template*  Ray_MusicScoreSnapComponent::getTemplate() const {return static_cast<const Ray_MusicScoreSnapComponent_Template*>(m_template);}
};

#endif //_ITF_RAY_MUSICSCORESNAPCOMPONENT_H_