#include "precompiled_engine.h"

#ifndef _ITF_PLAYANIM_EVT_H_
#include "engine/sequenceplayer/seq_events/PlayAnim_evt.h"
#endif //_ITF_PLAYANIM_EVT_H_

#ifndef _ITF_LUAHANDLER_H_
#include "core/script/LUAHandler.h"
#endif //_ITF_LUAHANDLER_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_ANIMATEDCOMPONENT_H_
#include "engine/actors/components/animatedcomponent.h"
#endif //_ITF_ANIMATEDCOMPONENT_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_ANIMLIGHTCOMPONENT_H_
#include "engine/actors/components/AnimLightComponent.h"
#endif //_ITF_ANIMLIGHTCOMPONENT_H_

#ifndef _ITF_ANIMATION_H_
#include "engine/animation/Animation.h"
#endif //_ITF_ANIMATION_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_SUBANIMFRAMEINFO_H_
#include "engine/animation/SubAnimFrameInfo.h"
#endif //_ITF_SUBANIMFRAMEINFO_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef SERIALIZEROBJECTBINARY_H
#include "core/serializer/ZSerializerObjectBinary.h"
#endif // SERIALIZEROBJECTBINARY_H

#ifndef _ITF_SUBANIMLIGHTSET_H_
#include    "engine/animation/SubAnimSet.h"
#endif //_ITF_SUBANIMLIGHTSET_H_

#ifndef _ITF_COLOR_H_
#include "core/Color.h"
#endif //_ITF_COLOR_H_


namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(PlayAnim_evt)

    PlayAnim_evt::PlayAnim_evt()
    : m_forceAnimDone(bfalse)
    , m_animationDuration(U32_INVALID)
    {
    }

    SequenceEvent * PlayAnim_evt::create(SequencePlayerComponent* _owner, SequenceEvent_Template *_template)
    {
        PlayAnim_evt *pNew  = newAlloc(mId_Gameplay, PlayAnim_evt);
        pNew->m_ownerSequence   = _owner;
        pNew->m_template        = _template;
        return pNew;
    }

    PlayAnim_evt::~PlayAnim_evt()
    {
        if(!m_ownerSequence)
            return;

        if  (getActive())
        {
            onBecomeInactive();
        }
    }

    void    PlayAnim_evt::init(const Vec3d& _pos)
    {
    }


    bbool PlayAnim_evt::linkBindedActor( bbool _forceLink /*= bfalse*/ )
    {
        bbool prevForce = m_forceAnimDone;
        setForceAnim( bfalse );
        bbool result = Super::linkBindedActor(_forceLink);        
        setForceAnim( prevForce );

        return result;
    }
    
    void    PlayAnim_evt::resetSubAnimsToPlay()
    {
        if(!m_ownerSequence)
            return;

        Actor* actor = getBindedActor();
        if (actor)
        {
            AnimLightComponent* animComp = actor->GetComponent<AnimLightComponent>();            
            if (animComp && animComp->isLoaded() && getTemplate()->getAnimType() != PlayAnim_evtTemplate::type_not_defined)       
            {
                animComp->resetSubAnimsToPlay();
            }
        }
    }


    void PlayAnim_evt::commitSubAnimsToPlay(ActorRef _actorRef, u32 _state)
    {
        Actor* actor = _actorRef.getActor();
        if (actor)
        {
            AnimLightComponent* animComp = actor->GetComponent<AnimLightComponent>();            
            if (animComp && animComp->isLoaded())       
            {
                animComp->setBankState(_state);
                animComp->commitSubAnimsToPlay();
            }
        }
    }


    void    PlayAnim_evt::forceCurrentFrame(   i32 _frame  )
    {
        if(!m_active) 
        {
            onBecomeActive();
        }
        
        const PlayAnim_evtTemplate * evtTemplate = getTemplate();

        ITF_ASSERT(_frame >= (evtTemplate->getStartFrame() + evtTemplate->getOffset()) && _frame <= (evtTemplate->getStopFrame() + evtTemplate->getOffset()));

        Actor* actor = getBindedActor();
        if (!actor)
            return;

#ifdef ITF_SUPPORT_EDITOR
        // if we are editing ...
        if (m_ownerSequence->GetActor()->getOverrideTemplate() )
            actor->updatePhysicalReady();
#endif
        
        if (!actor->isPhysicalReady())
            return;

        i32 keyFrame = (_frame - evtTemplate->getStartFrame());
        i32 keyFrameInAnim = (i32)(keyFrame * evtTemplate->getPlayRate() + 0.5f);
        if(m_animationDuration == U32_INVALID)
            m_animationDuration = getAnimationDuration(getAnimationIndex());

        if(m_animationDuration != U32_INVALID)
        {
            if (m_animationDuration == 0)
                keyFrameInAnim = 0;
            else
            {
                if (evtTemplate->getCycle())
                    keyFrameInAnim = keyFrameInAnim % m_animationDuration;
                else
                    keyFrameInAnim = Clamp(keyFrameInAnim, 0, (i32)m_animationDuration);
            }
        }

        AnimLightComponent* animComp = actor->GetComponent<AnimLightComponent>();            
        if (animComp && animComp->isLoaded())
        {
            i32 indexAnim = setAnimation(keyFrame);

            //////////////////////////////////////////////////////////////////////////
            // Set and update animation
            //////////////////////////////////////////////////////////////////////////
            if (indexAnim != -1)
            {
                setForceAnim(btrue);

                // loop
                if(evtTemplate->getLoop()) 
                {
                    animComp->setPlayRate(evtTemplate->getPlayRate());
                    animComp->setAnimLoop(btrue);
                }
                else
                {
                    animComp->setPlayRate(0.f);                            
                    animComp->setAnimLoop(bfalse);                    
                }                     

                // set animation
                animComp->setFrameForSubAnim(indexAnim, frameToTime((f32)(keyFrameInAnim), ANIMATION_FRAMERATE));
                m_ownerSequence->addActorPlayAnimControlled(actor->getRef());
            }
            else
            {
                //ITF_ASSERT_MSG(0, "anim not found");
            }

            // flip
            bbool flip = getFlip(keyFrame, actor);
            if (flip != actor->isFlipped())
                actor->setFlipped(flip);

            f32 alpha = animComp->getAlpha();

            // alpha of animation
            if(evtTemplate->getAlphaSpline().GetNumPoints() > 0)
            {
                Vec3d vec;
                evtTemplate->getAlphaSpline().GetInterpolatedAtTime((f32)keyFrame, vec);                
                alpha = vec.m_x;
            }

            // color of animation
            if(evtTemplate->getColorSpline().GetNumPoints() > 0)
            {
                Vec3d vec;
                evtTemplate->getColorSpline().GetInterpolatedAtTime((f32)keyFrame, vec);
                Color color(alpha, vec.m_x, vec.m_y, vec.m_z);
                animComp->setColor(color);
            } 
            else
            {
                Color color = animComp->getColor();
                color.m_a = alpha;
                animComp->setColor(color);
            }
        }
    }

    u32 PlayAnim_evt::getAnimationDuration(i32 _index)
    {
        if(_index == -1)
            return U32_INVALID;

        Actor* actor = NULL;
        actor = getBindedActor();
        if (!actor)
            return U32_INVALID;

        AnimLightComponent* animComp = actor->GetComponent<AnimLightComponent>();            
        if (!animComp || !animComp->isLoaded())
            return U32_INVALID;


        SubAnimSet* subAnimSet = animComp->getSubAnimSet();
        if(subAnimSet)
        {
            i32 count = subAnimSet->getSubAnimCount();
            if(_index < count)
            {
                SubAnim * sunAnim = subAnimSet->getSubAnim(_index);
                return (u32)timeToFrame(sunAnim->getDuration(), ANIMATION_FRAMERATE);
            }
        }

        return U32_INVALID;
    }

    i32 PlayAnim_evt::setAnimation(i32 _frameAnim)
    {
        Actor*  actor       = getBindedActor();
        i32     indexAnim   = -1;

        if (!actor)
            return -1;

        AnimLightComponent* animComp = actor->GetComponent<AnimLightComponent>();            
        if (!animComp || !animComp->isLoaded())
            return -1;

        const PlayAnim_evtTemplate *    evtTemplate = getTemplate();
        const String8 &                 animPath    = evtTemplate->getAnim();
        PlayAnim_evtTemplate::typeAnim  type        = evtTemplate->getAnimType();

        // weight of animation
        f32 weight = 1.0f;
        if(evtTemplate->getWeightSpline().GetNumPoints() > 0)
        {
            Vec3d vec;
            evtTemplate->getWeightSpline().GetInterpolatedAtTime((f32)_frameAnim, vec);
            weight = vec.m_x;
        }

        // use patches
        bbool usePatches = getUsePatches(_frameAnim, animComp);

        //////////////////////////////////////////////////////////////////////////
        //ANIMATION
        // => if the animation extension is "anm" else is a sub animation
        //////////////////////////////////////////////////////////////////////////

        if (type == PlayAnim_evtTemplate::type_anim)
        {
            String extension = FilePath::getExtension(animPath);

            if( extension.equals("anm", true) && !animPath.isEmpty())
            {
                indexAnim = animComp->addSubAnimToPlay(getTemplate()->getAnimName(), weight, usePatches); 
            } 
            else
            {
                type = PlayAnim_evtTemplate::type_sub_anim;
            }
        }


        //////////////////////////////////////////////////////////////////////////
        // SUB ANIM
        //////////////////////////////////////////////////////////////////////////
        if (type == PlayAnim_evtTemplate::type_sub_anim)
        {
            if(!animPath.isEmpty())
            {
                indexAnim = animComp->addSubAnimToPlay(animPath, weight, usePatches);
            }
        }

        ITF_ASSERT_MSG(indexAnim == -1 || type == evtTemplate->getAnimType(), "Bad animation type, Please correct data");
        return indexAnim;
    }


    i32 PlayAnim_evt::getAnimationIndex()
    {
        Actor*  actor       = getBindedActor();
        i32     indexAnim   = -1;

        if (!actor)
            return -1;

        AnimLightComponent* animComp = actor->GetComponent<AnimLightComponent>();            
        if (!animComp || !animComp->isLoaded())
            return -1;

        const PlayAnim_evtTemplate *    evtTemplate = getTemplate();
        const String8 &                 animPath    = evtTemplate->getAnim();
        PlayAnim_evtTemplate::typeAnim  type        = evtTemplate->getAnimType();

        if ( animPath.isEmpty())
            return -1;

        //////////////////////////////////////////////////////////////////////////
        //ANIMATION
        // => if the animation extension is "anm" else is a sub animation
        //////////////////////////////////////////////////////////////////////////
        if (type == PlayAnim_evtTemplate::type_anim)
        {
            String extension = FilePath::getExtension(animPath);

            if( extension.equals("anm", btrue))
            {
                indexAnim =  animComp->getSubAnimSet()->getSubAnimIndex(animPath);
                if(indexAnim == -1)
                {
                    indexAnim = animComp->getSubAnimSet()->getSubAnimIndex(evtTemplate->getAnimName());
                }
                return indexAnim;
            } 
            type = PlayAnim_evtTemplate::type_sub_anim;
        }

        //////////////////////////////////////////////////////////////////////////
        // SUB ANIM
        //////////////////////////////////////////////////////////////////////////
        if (type == PlayAnim_evtTemplate::type_sub_anim)
        {
            return animComp->getSubAnimSet()->getSubAnimIndex(animPath);
        }

        return -1;
    }

    void    PlayAnim_evt::onBecomeActive()
    {
        Super::onBecomeActive();

        if(!m_ownerSequence)
            return;

        Actor* actor = NULL;
        actor = getBindedActor();

        if (actor)
        {
            i32 index = getAnimationIndex();
            bbool forceControl = getTemplate()->getDisableComponentsActor() && index != -1;

            m_ownerSequence->controlActor(actor->getRef(), forceControl);        
            setForceAnim(forceControl);
        }
    }



    void    PlayAnim_evt::onBecomeInactive()
    {
        Super::onBecomeInactive();

        if(!m_ownerSequence)
            return;

        Actor* actor = NULL;
        actor = getBindedActor();

        if (actor)
        {
            i32 index = getAnimationIndex();
            bbool forceControl = getTemplate()->getDisableComponentsActor() && index != -1;

            m_ownerSequence->releaseActor(actor->getRef(), forceControl);
            setForceAnim(bfalse);
        }
    }

    void PlayAnim_evt::setForceAnim( bbool _force )
    {
        if (_force == m_forceAnimDone)
            return;

        Actor * actor = getBindedActor();
        if (!actor)
            return;

        AnimLightComponent* animComp = actor->GetComponent<AnimLightComponent>();
        if (animComp && animComp->isLoaded())
        {
            animComp->setForceDirectUpdate(_force);
            m_forceAnimDone = _force;
            if (!_force)
                animComp->setPlayRate(1.0f);
        }
    }

    bbool PlayAnim_evt::getFlip(i32 _frame, Actor* _actor)
    {
        bbool               flip        = _actor->isFlipped();
        const BoolEventList&flipList    = getTemplate()->getFlipBoolList();

        u32 count = flipList.size();
        for(u32 i = 0; i < count; i++)
        {
            if(flipList.getAt(i).m_frame <= _frame)
                flip = flipList.getAt(i).m_value;
            else
                break;
        }
        return flip;
    }

    bbool PlayAnim_evt::getUsePatches(i32 _frame, AnimLightComponent* animComp)
    {
        bbool               usePatches      = true;
        const BoolEventList&usePatchesList  = getTemplate()->getUsePatchesBoolList();

        u32 count = usePatchesList.size();
        for(u32 i = 0; i < count; i++)
        {
            if(usePatchesList.getAt(i).m_frame <= _frame)
                usePatches = usePatchesList.getAt(i).m_value;
            else
                break;
        }
        return usePatches;
    }


    f32 PlayAnim_evt::getStopFrameAnim()
    {
        Actor* actor = getBindedActor();
        if (actor)
        {
            AnimLightComponent* animComp = actor->GetComponent<AnimLightComponent>();            
            if (animComp && animComp->isLoaded())       
            {
                u32 numAnims = animComp->getNumAnimsPlaying();
                ITF_ASSERT(numAnims == 1);
                if(numAnims > 0)
                {
                    return animComp->getCurSubAnim(0)->getDuration();
                }
            }
        }
        return -1;
    }

    /////////////////////////////////////////////////////
    /// TEMPLATE
    /////////////////////////////////////////////////////



    IMPLEMENT_OBJECT_RTTI(PlayAnim_evtTemplate)

    BEGIN_SERIALIZATION_CHILD(PlayAnim_evtTemplate)
    
        SERIALIZE_ENUM_BEGIN("TypeAnim",m_typeAnim);
        SERIALIZE_ENUM_VAR(type_anim);
        SERIALIZE_ENUM_VAR(type_action);
        SERIALIZE_ENUM_VAR(type_sub_anim);
        SERIALIZE_ENUM_END();

        SERIALIZE_MEMBER("Anim",m_anim);
        SERIALIZE_MEMBER("Loop",m_loop);
        SERIALIZE_MEMBER("Cycle",m_cycle);
        SERIALIZE_MEMBER("PlayRate",m_playRate);
        SERIALIZE_MEMBER("BeginMarker",m_beginMarker);
        SERIALIZE_MEMBER("EndMarker",m_endMarker);

        SERIALIZE_OBJECT("Flip",m_flip);
        SERIALIZE_OBJECT("Weight",m_weight);
        SERIALIZE_OBJECT("usePatches",m_usePatches);
        SERIALIZE_OBJECT("Color",m_color);
        SERIALIZE_OBJECT("Alpha",m_alpha);
    END_SERIALIZATION()

    PlayAnim_evtTemplate::PlayAnim_evtTemplate()
    : m_loop(bfalse)
    , m_cycle(btrue)
    , m_anim("")
    , m_typeAnim(type_anim)
    , m_beginMarker(-1)
    , m_endMarker(-1)
    , m_playRate(1.f)
    {

    }

    String PlayAnim_evtTemplate::getAnimName() const
    {
        String name = FilePath::getFilenameWithoutExtension(m_anim);
        if (m_beginMarker <= 0 && m_endMarker <= 0)
            return name;

        String fullName;
        fullName.setTextFormat("%s_%i_%i", StringToUTF8(name).get(), m_beginMarker, m_endMarker);
        return fullName;
    }

#ifdef ITF_SUPPORT_BLOBEVENT
    SequenceEvent_Template*   PlayAnim_evtTemplate::createFromBlob( Blob* _blob, SequencePlayerComponent_Template* _owner )
    {
        PlayAnim_evtTemplate* pNew = newAlloc(mId_GameplayTemplate, PlayAnim_evtTemplate);
        pNew->setOwnerSequence(_owner);

        pNew->fillFromBlob(_blob);

        return pNew;
    }

    void PlayAnim_evtTemplate::serializeToBlob (Blob* _blob)
    {
        serializeCommonToBlob(_blob);
        serializeFriendlyToBlob(_blob);

        // type
        _blob->pushInt32((int)m_typeAnim);

        // anim
        _blob->pushString(m_anim);

        // loop
        _blob->pushBool(m_loop);

        // cycle
        _blob->pushBool(m_cycle);

        // alpha
        _blob->pushFloat32(m_playRate);

        // sub anim creation
        _blob->pushInt32(m_beginMarker);
        _blob->pushInt32(m_endMarker);

        // flip
        m_flip.serializeToBlob(_blob);

        // use patches
        m_usePatches.serializeToBlob(_blob);


        // weight
        m_weight.serializeToBlob(_blob);

        // color
        m_color.serializeToBlob(_blob);

        // alpha
        m_alpha.serializeToBlob(_blob);

    }

    void PlayAnim_evtTemplate::fillFromBlob(Blob* _blob)
    {
        fillCommonFromBlob(_blob);
        fillFriendlyFromBlob(_blob);

        // type
        i32 type;
        type = _blob->extractInt32();
        m_typeAnim = (typeAnim)type;

        // anim
        String anim_str;
        _blob->extractString(anim_str);
        m_anim = anim_str.cStr();

        // loop
        m_loop = _blob->extractBool();

        // cycle
        m_cycle = _blob->extractBool();

        // playrate
        m_playRate = _blob->extractFloat32();

        // sub anim creation
        m_beginMarker = _blob->extractInt32();
        m_endMarker = _blob->extractInt32();

        // flip
        m_flip.fillFromBlob(_blob);

        // use patches
        m_usePatches.fillFromBlob(_blob);

        // weight
        m_weight.fillFromBlob(_blob);

        // color
        m_color.fillFromBlob(_blob);

        // alpha
        m_alpha.fillFromBlob(_blob);
    }
#endif //ITF_SUPPORT_BLOBEVENT



} // namespace ITF


