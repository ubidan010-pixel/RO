
#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAYANIMNODESWING_H_
#include "rayman/gameplay/AnimNodes/Ray_AnimNodeSwing.h"
#endif //_ITF_RAYANIMNODESWING_H_

namespace ITF
{

//======================================================================================================
//======================================================================================================

IMPLEMENT_OBJECT_RTTI(AnimNodeSwing)

AnimNodeSwing::AnimNodeSwing()
{

}

void AnimNodeSwing::init( BlendTreeInitDataBase* _initData, const BlendTreeNodeTemplate <AnimTreeResult>* _template )
{
    Super::init(_initData,_template);

    ITF_ASSERT(this->m_leafs.size() == _template->getCapacity());
}

void AnimNodeSwing::updateResult( f32 _deltaTime, const InputContainer& _inputs, BlendTreeResultBase* _result )
{
    updateActiveChild(_inputs,bfalse);

    this->processBlend(_deltaTime,_inputs,_result);

    AnimTreeResult* result = static_cast<AnimTreeResult*>(_result);

    result->m_playingNodes.push_back(this);
}

u32  AnimNodeSwing::getActiveCriteria( const InputContainer& _inputs ) const
{
    u32 activeCriteria = 0;

    for ( ChildDataContainer::const_iterator it = m_childData.begin(); it != m_childData.end(); ++it, activeCriteria++ )
    {
        const BlendLeaf& leafCriterias = *it;
        bbool match = btrue;

        for (ITF_VECTOR<Criteria*>::const_iterator it = leafCriterias.m_criteria.begin(); it != leafCriterias.m_criteria.end(); ++it)
        {
            if (!(*it)->isEqual())
            {
                match = bfalse;
                break;
            }
        }

        if ( match )
        {
            break;
        }
    }

    return activeCriteria;
}

void AnimNodeSwing::updateActiveChild( const InputContainer& _inputs, bbool _force )
{
    u32 activeCriteria = getActiveCriteria(_inputs);

    if ( _force )
    {
        switch(activeCriteria)
        {
            case BRANCHCRITERIA_NONE:
                setActiveLeaf(_inputs,IDLE_DOWN,_force);
                break;
            case BRANCHCRITERIA_FORWARD:
                setActiveLeaf(_inputs,DOWN_TO_UP,_force);
                break;
            case BRANCHCRITERIA_BACKWARD:
                setActiveLeaf(_inputs,UTURN_DOWN,_force);
                break;
        }
    }
    else
    {
        bbool force;

        u32 playChild = calculateNewActiveChild(_inputs,force);

        if ( playChild != m_activeLeaf )
        {
            setActiveLeaf(_inputs,playChild,force);
        }
    }
}
u32  AnimNodeSwing::calculateNewActiveChild( const InputContainer& _inputs, bbool& _force ) const
{
    u32 activeCriteria = getActiveCriteria(_inputs);
    u32 playChild = U32_INVALID;
    
    _force = bfalse;

    switch(activeCriteria)
    {
    case BRANCHCRITERIA_NONE:
        switch(m_activeLeaf)
        {
        case IDLE_DOWN:
            // Do nothing
            break;
        case IDLE_UP:
            {
                // Need to put legs down
                playChild = UP_TO_DOWN;
                _force = btrue;
            }
            break;
        case DOWN_TO_UP:
            {
                // Stop this, put legs down
                AnimTreeResult localResult;

                this->m_leafs[DOWN_TO_UP]->updateResult(0.f,_inputs,&localResult);

                if ( localResult.m_animPos < 0.5f )
                {
                    // The animation is on the first half, so just blend down
                    playChild = IDLE_DOWN;
                }
                else
                {
                    // If it played too much, then play the transition to legs down
                    playChild = UP_TO_DOWN;
                }

                _force = bfalse;
            }
            break;
        case UP_TO_DOWN:
        case UTURN_DOWN:
            {
                // Keep with this until it is finished, then switch to IDLE_DOWN
                AnimTreeResult localResult;

                this->m_leafs[m_activeLeaf]->updateResult(0.f,_inputs,&localResult);

                if ( localResult.m_animFinished )
                {
                    playChild = IDLE_DOWN;
                    _force = btrue;
                }
            }
            break;
        case UTURN_UP:
            {
                // Wait until this is done, then lower the legs
                AnimTreeResult localResult;

                this->m_leafs[UTURN_UP]->updateResult(0.f,_inputs,&localResult);

                if ( localResult.m_animFinished )
                {
                    playChild = UP_TO_DOWN;
                    _force = btrue;
                }
            }
            break;
        }
        break;
    case BRANCHCRITERIA_FORWARD:
        switch(m_activeLeaf)
        {
        case IDLE_DOWN:
            {
                // Start swinging forward, play down to up
                playChild = DOWN_TO_UP;
                _force = bfalse;
            }
            break;
        case IDLE_UP:
            // We are fine
            break;
        case DOWN_TO_UP:
        case UTURN_UP:
            {
                // Wait until this finishes, then switch to IDLE_UP
                AnimTreeResult localResult;

                this->m_leafs[m_activeLeaf]->updateResult(0.f,_inputs,&localResult);

                if ( localResult.m_animFinished )
                {
                    playChild = IDLE_UP;
                    _force = btrue;
                }
            }
            break;
        case UP_TO_DOWN:
            {
                // Stop this and raise the legs again
                AnimTreeResult localResult;

                this->m_leafs[UP_TO_DOWN]->updateResult(0.f,_inputs,&localResult);

                if ( localResult.m_animPos < 0.5f )
                {
                    // The animation is on the first half, so just blend up
                    playChild = IDLE_UP;
                    _force = bfalse;
                }
                else
                {
                    // If it played too much, then play the transition to legs down
                    playChild = DOWN_TO_UP;
                    _force = bfalse;
                }
            }
            break;
        case UTURN_DOWN:
            {
                // Wait until this is done, then raise the legs
                AnimTreeResult localResult;

                this->m_leafs[UTURN_DOWN]->updateResult(0.f,_inputs,&localResult);

                if ( localResult.m_animFinished )
                {
                    playChild = DOWN_TO_UP;
                    _force = btrue;
                }
            }
            break;
        }
        break;
    case BRANCHCRITERIA_BACKWARD:
        switch(m_activeLeaf)
        {
        case IDLE_DOWN:
            {
                // Switch to UTURN_DOWN
                playChild = UTURN_DOWN;
                _force = btrue;
            }
            break;
        case IDLE_UP:
            {
                // Switch to UTURN_UP
                playChild = UTURN_UP;
                _force = btrue;
            }
            break;
        case DOWN_TO_UP:
            {
                // Play UTURN_UP when it's finished
                AnimTreeResult localResult;

                this->m_leafs[DOWN_TO_UP]->updateResult(0.f,_inputs,&localResult);

                if ( localResult.m_animPos < 0.2f )
                {
                    playChild = UTURN_DOWN;
                    _force = btrue;
                }
                else if ( localResult.m_animPos > 0.8f )
                {
                    playChild = UTURN_UP;
                    _force = btrue;
                }
            }
            break;
        case UP_TO_DOWN:
            {
                // Play UTURN_DOWN when it's finished
                AnimTreeResult localResult;

                this->m_leafs[UP_TO_DOWN]->updateResult(0.f,_inputs,&localResult);

                if ( localResult.m_animPos < 0.2f )
                {
                    playChild = UTURN_UP;
                    _force = btrue;
                }
                else if ( localResult.m_animPos > 0.8 )
                {
                    playChild = UTURN_DOWN;
                    _force = btrue;
                }
            }
            break;
        case UTURN_DOWN:
            {
                // Wait until this is done, then decide
                AnimTreeResult localResult;

                this->m_leafs[UTURN_DOWN]->updateResult(0.f,_inputs,&localResult);

                if ( localResult.m_animFinished )
                {
                    playChild = IDLE_DOWN;
                    _force = btrue;
                }
            }
            break;
        case UTURN_UP:
            {
                // Wait until it is finished, then decide
                AnimTreeResult localResult;

                this->m_leafs[UTURN_UP]->updateResult(0.f,_inputs,&localResult);

                if ( localResult.m_animFinished )
                {
                    playChild = IDLE_UP;
                    _force = btrue;
                }
            }
            break;
        }
        break;
    }

    if ( playChild == U32_INVALID )
    {
        playChild = m_activeLeaf;
    }

    return playChild;
}

void AnimNodeSwing::getPlayingNodes( const InputContainer & _inputs, NodeList& _playingNodes )
{
    bbool force;
    u32 playChild = calculateNewActiveChild(_inputs,force);

    if ( playChild != U32_INVALID )
    {
        m_leafs[playChild]->getPlayingNodes(_inputs,_playingNodes);
    }

    _playingNodes.push_back(this);
}

//======================================================================================================
//======================================================================================================

IMPLEMENT_OBJECT_RTTI(AnimNodeSwing_Template)

BEGIN_SERIALIZATION_CHILD(AnimNodeSwing_Template)

END_SERIALIZATION()

}



