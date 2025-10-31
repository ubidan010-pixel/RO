#include "precompiled_engine.h"

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_ANIMTREENODEPLAYANIM_H_
#include "engine/blendTree/animTree/AnimTreeNodePlayAnim.h"
#endif //_ITF_ANIMTREENODEPLAYANIM_H_

#ifndef _ITF_SUBANIMLIGHTSET_H_
#include "engine/animation/SubAnimSet.h"
#endif //_ITF_SUBANIMLIGHTSET_H_

namespace ITF
{

//======================================================================================================
//======================================================================================================

IMPLEMENT_OBJECT_RTTI(AnimTreeNodePlayAnim)

void AnimTreeNodePlayAnim::onLoadResource( BlendTreeLoadResourceBase* _resData )
{
    Super::onLoadResource(_resData);

    if(getTemplate()->getAnimationName().isValid())
    {
        AnimTreeLoadResource* animTreeInit = static_cast<AnimTreeLoadResource*>(_resData);
        i32 subAnimIndex = animTreeInit->m_subAnimSet->getSubAnimIndex(getTemplate()->getAnimationName());

        if ( subAnimIndex != -1 )
        {
            m_anim.setSubAnimSet(animTreeInit->m_subAnimSet);
            m_anim.setSubAnimIndex(subAnimIndex);

            if ( getTemplate()->getProceduralCursor().isValid() )
            {
                m_anim.setIsProcedural(btrue);
            }
        }
        else
        {
            ITF_FATAL_ERROR("Animation with friendly %s not defined",getTemplate()->getAnimationName().getDebugString());
        }
    }
}

void AnimTreeNodePlayAnim::updateResult( f32 _deltaTime, const InputContainer& _inputs, BlendTreeResultBase* _result )
{
    Super::updateResult(_deltaTime,_inputs,_result);

    setCursorPos(_deltaTime,_inputs);
    setPlayRate(_deltaTime,_inputs);

    static AnimTreeResultLeaf result;

    result.m_subAnim = NULL;
    result.m_events.clear();

    if ( !m_firstUpdate )
    {
        if ( _deltaTime )
        {
            m_anim.updateAnimTime(_deltaTime,getTemplate()->getUseEvents() ? &result.m_events : NULL);
        }
        else
        {
            m_anim.updateAnimTime(0.f,NULL);
        }
    }
    else
    {
        m_firstUpdate = bfalse;
    }

    result.m_usePatches = getTemplate()->getUsePatches();
    result.m_useEvents = getTemplate()->getUseEvents();
    result.m_weight = 1.f;
    result.m_subAnim = &m_anim;

    AnimTreeResult* animTreeResult = static_cast<AnimTreeResult*>(_result);
    animTreeResult->m_anims.push_back(result);
    animTreeResult->m_animLooped = m_anim.getIsLooped();
    if (animTreeResult->m_animLooped)
        animTreeResult->m_animFinished = bfalse;
    else
        animTreeResult->m_animFinished = m_anim.isSubAnimFinished();
    animTreeResult->m_animPos = m_anim.calculateT();
}

void AnimTreeNodePlayAnim::setCursorPos( f32 _deltaTime, const InputContainer& _inputs )
{
    if ( getTemplate()->getProceduralCursor().isValid() )
    {
        ITF_ASSERT(m_anim.getIsProcedural());
        f32 cursor = getTemplate()->getProceduralCursor().getValue(m_anim.calculateT(),_deltaTime,_inputs);
        
        if ( m_anim.getIsLooped() )
        {
            cursor = f32_Modulo(cursor,1.f);
        }
        else
        {
            cursor = Clamp(cursor,0.f,1.f);
        }

        m_anim.setCursor(cursor);
    }
}

void AnimTreeNodePlayAnim::setPlayRate( f32 _deltaTime, const InputContainer& _inputs )
{
    if ( getTemplate()->getProceduralPlayRate().isValid() )
    {
        f32 playRate = getTemplate()->getProceduralPlayRate().getValue(m_anim.getPlayRate(),_deltaTime,_inputs);
        m_anim.setPlayRate(playRate);
    }
}

void AnimTreeNodePlayAnim::onBecomeActive( const InputContainer& _inputs )
{
    Super::onBecomeActive(_inputs);

    m_firstUpdate = !getTemplate()->getProceduralCursor().isValid();

    m_anim.resetCurTime();
}

void AnimTreeNodePlayAnim::setReverse( bbool _reverse )
{
    m_anim.setIsReversed(_reverse);
}

bbool AnimTreeNodePlayAnim::getIsReversed() const
{
    return m_anim.getIsReversed();
}

bbool AnimTreeNodePlayAnim::validate() const
{
    if(!getTemplate()->getAnimationName().isValid())
    {
        return bfalse;
    }

    if ( m_anim.getSubAnim() == NULL )
    {
        return bfalse;
    }

    return btrue;
}

//======================================================================================================
//======================================================================================================

IMPLEMENT_OBJECT_RTTI(AnimTreeNodePlayAnim_Template)

BEGIN_SERIALIZATION_CHILD(AnimTreeNodePlayAnim_Template)

    SERIALIZE_MEMBER("animationName",m_animationName);
    SERIALIZE_MEMBER("usePatches",m_usePatches);
    SERIALIZE_MEMBER("useEvents",m_useEvents);
    SERIALIZE_OBJECT("proceduralInput",m_proceduralCursor);
    SERIALIZE_OBJECT("proceduralPlayRate",m_proceduralPlayRate);

END_SERIALIZATION()

}



