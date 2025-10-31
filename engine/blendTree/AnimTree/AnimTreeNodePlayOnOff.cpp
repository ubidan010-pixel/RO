
#include "precompiled_engine.h"

#ifndef _ITF_ANIMTREENODEPLAYONOFF_H_
#include "engine/blendTree/animTree/AnimTreeNodePlayOnOff.h"
#endif //_ITF_ANIMTREENODEPLAYONOFF_H_

namespace ITF
{

//======================================================================================================
//======================================================================================================

IMPLEMENT_OBJECT_RTTI(AnimTreeNodePlayOnOff)

AnimTreeNodePlayOnOff::AnimTreeNodePlayOnOff()
: m_animOn(NULL)
, m_animOff(NULL)
, m_state(STATEONOFF_OFF)
{
}

void AnimTreeNodePlayOnOff::clear()
{
    for (CriteriaArray::const_iterator it = m_criteriaOn.begin(); it != m_criteriaOn.end(); ++it)
    {
        delete (*it);
    }

    SF_DEL(m_animOn);
    SF_DEL(m_animOff);
}

void AnimTreeNodePlayOnOff::init( BlendTreeInitDataBase* _initData, const BlendTreeNodeTemplate <AnimTreeResult>* _template )
{
    Super::init(_initData,_template);

    const CriteriaDescArray& criteriaTemplate = getTemplate()->getCriteriaDesc();

    for ( u32 i = 0; i < criteriaTemplate.size(); i++ )
    {
        const CriteriaDesc& desc = criteriaTemplate[i];
        Criteria* crit = Criteria::createCriteria(desc,_initData->m_inputs);

        if ( crit )
        {
            m_criteriaOn.push_back(crit);
        }
    }

    if ( getTemplate()->getAnimOn() )
    {
        m_animOn = getTemplate()->getAnimOn()->createNode();
        m_animOn->init(_initData,getTemplate()->getAnimOn());
    }

    if ( getTemplate()->getAnimOff() )
    {
        m_animOff = getTemplate()->getAnimOff()->createNode();
        m_animOff->init(_initData,getTemplate()->getAnimOff());
    }

    m_transOff.init(_initData,&getTemplate()->getTransOff());
    m_transOn.init(_initData,&getTemplate()->getTransOn());
}

void AnimTreeNodePlayOnOff::onLoadResource( BlendTreeLoadResourceBase* _resData )
{
    Super::onLoadResource(_resData);

    if ( m_animOn )
    {
        m_animOn->onLoadResource(_resData);
    }

    if ( m_animOff )
    {
        m_animOff->onLoadResource(_resData);
    }

    m_transOff.onLoadResource(_resData);
    m_transOn.onLoadResource(_resData);
}

void AnimTreeNodePlayOnOff::getPlayingNodes( const InputContainer & _inputs, NodeList& _playingNodes )
{
    EStateOnOff targetState = getTargetState();

    if ( targetState != m_state )
    {
        if ( targetState == STATEONOFF_ON )
        {
            switch(m_state)
            {
            case STATEONOFF_OFF:
            case STATEONOFF_TOON:
                m_transOn.getPlayingNodes(_inputs,_playingNodes);
                break;
            case STATEONOFF_TOOFF:
                m_transOff.getPlayingNodes(_inputs,_playingNodes);
                break;
            case STATEONOFF_ON:
                {
                    // excluded as targetState != m_state
                    ITF_ASSERT(false);
                }
                break;
            }
        }
        else
        {
            switch(m_state)
            {
            case STATEONOFF_TOOFF:
            case STATEONOFF_ON:
                m_transOff.getPlayingNodes(_inputs,_playingNodes);
                break;
            case STATEONOFF_TOON:
                m_transOn.getPlayingNodes(_inputs,_playingNodes);
                break;
            case STATEONOFF_OFF:
                break;
            }
        }
    }
    else
    {
        switch(m_state)
        {
        case STATEONOFF_ON:
            if ( m_animOn )
            {
                m_animOn->getPlayingNodes(_inputs,_playingNodes);
            }
            break;
        case STATEONOFF_OFF:
            if ( m_animOff )
            {
                m_animOff->getPlayingNodes(_inputs,_playingNodes);
            }
            break;
        case STATEONOFF_TOON:
            m_transOn.getPlayingNodes(_inputs,_playingNodes);
            break;
        case STATEONOFF_TOOFF:
            m_transOff.getPlayingNodes(_inputs,_playingNodes);
            break;
        }
    }

    Super::getPlayingNodes(_inputs,_playingNodes);
}

void AnimTreeNodePlayOnOff::updateResult( f32 _deltaTime, const InputContainer & _inputs, BlendTreeResultBase* _result )
{
    Super::updateResult(_deltaTime,_inputs,_result);

    setActiveAnim(_inputs);

    AnimTreeResult localResult;

    updateLocalResult(_deltaTime,_inputs,localResult);

    _result->updateBlend(&localResult,btrue,btrue,1.f);
}

void AnimTreeNodePlayOnOff::setActiveAnim( const InputContainer& _inputs )
{
    EStateOnOff targetState = getTargetState();

    if ( targetState != m_state )
    {
        if ( targetState == STATEONOFF_ON )
        {
            switch(m_state)
            {
            case STATEONOFF_OFF:
                m_transOn.setReverse(bfalse);
                m_transOn.onBecomeActive(_inputs);
                m_state = STATEONOFF_TOON;
                break;
            case STATEONOFF_TOOFF:
                if ( !m_transOff.getIsReversed() )
                {
                    m_transOff.setReverse(btrue);
                }
                break;
            case STATEONOFF_TOON:
                if ( m_transOn.getIsReversed() )
                {
                    m_transOn.setReverse(bfalse);
                }
                break;
            case STATEONOFF_ON:
                {
                    // excluded as targetState != m_state
                    ITF_ASSERT(false);
                }
                break;
            }
        }
        else
        {
            switch(m_state)
            {
            case STATEONOFF_ON:
                m_transOff.setReverse(bfalse);
                m_transOff.onBecomeActive(_inputs);
                m_state = STATEONOFF_TOOFF;
                break;
            case STATEONOFF_TOOFF:
                if ( m_transOff.getIsReversed() )
                {
                    m_transOff.setReverse(bfalse);
                }
                break;
            case STATEONOFF_TOON:
                if ( !m_transOn.getIsReversed() )
                {
                    m_transOn.setReverse(btrue);
                }
                break;
            case STATEONOFF_OFF:
                break;
            }
        }
    }
}

void AnimTreeNodePlayOnOff::updateLocalResult( f32 _deltaTime, const InputContainer & _inputs, AnimTreeResult& _localResult )
{
    switch(m_state)
    {
    case STATEONOFF_ON:
        if ( m_animOn )
        {
            m_animOn->updateResult(_deltaTime,_inputs,&_localResult);
        }
        break;
    case STATEONOFF_OFF:
        if ( m_animOff )
        {
            m_animOff->updateResult(_deltaTime,_inputs,&_localResult);
        }
        break;
    case STATEONOFF_TOON:
        m_transOn.updateResult(_deltaTime,_inputs,&_localResult);

        if ( _localResult.m_animFinished )
        {
            if ( m_transOn.getIsReversed() )
            {
                m_state = STATEONOFF_OFF;

                if ( m_animOff )
                {
                    m_animOff->onBecomeActive(_inputs);
                }
            }
            else
            {
                m_state = STATEONOFF_ON;

                if ( m_animOn )
                {
                    m_animOn->onBecomeActive(_inputs);
                }
            }
        }
        break;
    case STATEONOFF_TOOFF:
        m_transOff.updateResult(_deltaTime,_inputs,&_localResult);

        if ( _localResult.m_animFinished )
        {
            if ( m_transOff.getIsReversed() )
            {
                m_state = STATEONOFF_ON;

                if ( m_animOn )
                {
                    m_animOn->onBecomeActive(_inputs);
                }
            }
            else
            {
                m_state = STATEONOFF_OFF;

                if ( m_animOff )
                {
                    m_animOff->onBecomeActive(_inputs);
                }
            }
        }
        break;
    }
}

void AnimTreeNodePlayOnOff::onBecomeActive( const InputContainer& _inputs )
{
    Super::onBecomeActive(_inputs);

    m_state = getTargetState();

    if ( m_state == STATEONOFF_OFF )
    {
        if ( m_animOff )
        {
            m_animOff->onBecomeActive(_inputs);
        }
    }
    else
    {
        if ( m_animOn )
        {
            m_animOn->onBecomeActive(_inputs);
        }
    }
}

AnimTreeNodePlayOnOff::EStateOnOff AnimTreeNodePlayOnOff::getTargetState() const
{
    if ( Criteria::matchCriteria(m_criteriaOn) )
    {
        return STATEONOFF_ON;
    }
    else
    {
        return STATEONOFF_OFF;
    }
}

//======================================================================================================
//======================================================================================================

IMPLEMENT_OBJECT_RTTI(AnimTreeNodePlayOnOff_Template)

BEGIN_SERIALIZATION_CHILD(AnimTreeNodePlayOnOff_Template)

    SERIALIZE_OBJECT_WITH_FACTORY("nodeOn", m_animOn, NULL);
    SERIALIZE_OBJECT_WITH_FACTORY("nodeOff", m_animOff, NULL);
    SERIALIZE_OBJECT("transOn", m_transOn);
    SERIALIZE_OBJECT("transOff", m_transOff);
    SERIALIZE_CONTAINER_OBJECT("criteriasOn",m_criteriaOnDesc);

END_SERIALIZATION()

AnimTreeNodePlayOnOff_Template::AnimTreeNodePlayOnOff_Template()
: m_animOn(NULL)
, m_animOff(NULL)
{
}

AnimTreeNodePlayOnOff_Template::~AnimTreeNodePlayOnOff_Template()
{
    SF_DEL(m_animOn);
    SF_DEL(m_animOff);
}

}



