#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_MUSICSCORESNAPCOMPONENT_H_
#include "Rayman/gameplay/Components/AI/Ray_MusicScoreSnapComponent.h"
#endif //_ITF_RAY_MUSICSCORESNAPCOMPONENT_H_

#ifndef _ITF_RAY_GAMEPLAYEVENTS_H_
#include "rayman/gameplay/Ray_GameplayEvents.h"
#endif //_ITF_RAY_GAMEPLAYEVENTS_H_

#ifndef _ITF_RAY_MUSICSCOREAICOMPONENT_H_
#include "Rayman/gameplay/Components/AI/Ray_MusicScoreAIComponent.h"
#endif //_ITF_RAY_MUSICSCOREAICOMPONENT_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_MusicScoreSnapComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_MusicScoreSnapComponent)
        BEGIN_CONDITION_BLOCK(ESerializeGroup_Data)
            SERIALIZE_MEMBER("distOnCurve",  m_distOnCurve);
            SERIALIZE_MEMBER("intervalNote", m_intervalNote);
            SERIALIZE_MEMBER("note", m_note);
        END_CONDITION_BLOCK()
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_MusicScoreSnapComponent)
        VALIDATE_COMPONENT_PARAM("note", m_note<5, "Valid notes: 0, 1, 2, 3, 4");
    END_VALIDATE_COMPONENT()

    //*****************************************************************************

    Ray_MusicScoreSnapComponent::Ray_MusicScoreSnapComponent()
    : m_distOnCurve(0.0f)
    , m_intervalNote(1.0f)
    , m_isDeploy(bfalse)
    , m_note(0)
    , m_badSnap(bfalse)
    , m_reverse(bfalse)
    {
        // none
    }

    //*****************************************************************************

    Ray_MusicScoreSnapComponent::~Ray_MusicScoreSnapComponent()
    {
        // none
    }

    //*****************************************************************************

    void Ray_MusicScoreSnapComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);
    }

    //****************************************************************************
  
    void Ray_MusicScoreSnapComponent::registerMusicScore(Actor* _actorMusicScore, bbool _disableActor /*= btrue*/)
    {
        m_parent = _actorMusicScore->getRef();
        if (_disableActor)
        {
            m_actor->disable();
        }
    }

    //*****************************************************************************

    void Ray_MusicScoreSnapComponent::unregisterMusicScore(Actor* _actorMusicScore, bbool _enableActor /*= btrue*/)
    {   
        m_parent.invalidate();
        if (_enableActor)
        {
            m_actor->enable();
        }
    }

    //*****************************************************************************

    void Ray_MusicScoreSnapComponent::updateOpening( f32 _curDist, bbool _forced )
    {
       if (_curDist > m_distOnCurve && !m_isDeploy || _forced)
       {
           m_isDeploy = btrue;
           
           // enable and pass snap info
           m_actor->enable();
           Ray_MusicScoreOpenEvent open(
               5,
               m_badSnap ? U32_INVALID : m_note,
               m_distOnCurve,
               m_intervalNote,
               _forced
               );
           m_actor->onEvent(&open);
       }
    }

    //*****************************************************************************

    void Ray_MusicScoreSnapComponent::updateClosing( f32 _curDist )
    {
        if (_curDist < m_distOnCurve && m_isDeploy)
        {
            m_isDeploy = bfalse;

            // unsnap, actors should disable themselves
            Ray_MusicScoreCloseEvent close;
            m_actor->onEvent(&close);
        }
    }

    //*****************************************************************************

    void Ray_MusicScoreSnapComponent::setInfoSnap( f32 _distOnCurve, u32 _selectedNote, f32 _intervalNote, bbool _badSnap, bbool _reverse )
    {
        m_distOnCurve =_distOnCurve;
        m_note =_selectedNote;
        m_intervalNote =_intervalNote;
        m_badSnap =_badSnap;
        m_reverse = _reverse;
    }
    //*****************************************************************************
    // Template
    //*****************************************************************************

    IMPLEMENT_OBJECT_RTTI(Ray_MusicScoreSnapComponent_Template)
    BEGIN_SERIALIZATION(Ray_MusicScoreSnapComponent_Template)
    END_SERIALIZATION()

    //*****************************************************************************

    Ray_MusicScoreSnapComponent_Template::Ray_MusicScoreSnapComponent_Template()
    {
        // none
    }

    //*****************************************************************************

    Ray_MusicScoreSnapComponent_Template::~Ray_MusicScoreSnapComponent_Template()
    {
        // none
    }

    //*****************************************************************************


};
