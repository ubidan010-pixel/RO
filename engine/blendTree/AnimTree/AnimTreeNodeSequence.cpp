
#include "precompiled_engine.h"

#ifndef _ITF_ANIMTREENODESEQUENCE_H_ 
#include "engine/blendTree/AnimTree/AnimTreeNodeSequence.h"
#endif //_ITF_ANIMTREENODESEQUENCE_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(AnimTreeNodeSequence)
IMPLEMENT_OBJECT_RTTI(AnimTreeNodeSequence_Template)

BEGIN_SERIALIZATION_CHILD(AnimTreeNodeSequence_Template)
SERIALIZE_MEMBER("loopCount",m_loopCount);
SERIALIZE_MEMBER("randomizeLoopCount",m_randomizeLoopCount);
SERIALIZE_MEMBER("randomLoopCountMax",m_randomLoopCountMax);
SERIALIZE_MEMBER("randomLoopCountMin",m_randomLoopCountMin);
END_SERIALIZATION()


void AnimTreeNodeSequence::getPlayingNodes( const InputContainer & _inputs, NodeList& _playingNodes )
{
    if ( m_activeLeaf != U32_INVALID )
    {
        BlendTreeNode <AnimTreeResult>* node = this->m_leafs[m_activeLeaf];
        node->getPlayingNodes( _inputs, _playingNodes );
    }

    Super::getPlayingNodes( _inputs, _playingNodes );
}

void AnimTreeNodeSequence::updateResult( f32 _deltaTime, const InputContainer& _inputs, BlendTreeResultBase* _result )
{
    Super::updateResult(_deltaTime,_inputs,_result);

    if ( !m_leafs.size() )
        return;


    if ( m_activeLeaf <  this->m_leafs.size() )
    {

        this->m_leafs[m_activeLeaf]->updateResult(_deltaTime,_inputs,_result);
        AnimTreeResult* result = static_cast<AnimTreeResult*>(_result);
        if ( result->m_animFinished )
        {
            u32 nextLeaf = m_activeLeaf + 1;
            if (nextLeaf >= this->m_leafs.size())
            {
                m_numTimesLooped++;
                //infinite looping
                if (m_loopCount == -1)
                {
                    nextLeaf = 0;
                    setActiveLeaf(_inputs,nextLeaf);
                    result->m_animFinished = bfalse;
                }
                // no looping
                else if (m_loopCount == 0)
                {
                    //do nothing
                }
                else
                {
                    //finite number of loops
                    if (m_numTimesLooped <= m_loopCount)
                    {
                        nextLeaf = 0;
                        setActiveLeaf(_inputs,nextLeaf);
                        result->m_animFinished = bfalse;
                    }
                    //otherwise do nothing we're finished
                }
            }
            else
            {
                setActiveLeaf(_inputs,nextLeaf);
                result->m_animFinished = bfalse;
            }
        }
    }
}

void AnimTreeNodeSequence::setActiveLeaf( const InputContainer& _inputs, u32 leafIndex )
{
    ITF_ASSERT( leafIndex < m_leafs.size() );

    if ( leafIndex < m_leafs.size() )
    {
        m_activeLeaf = leafIndex;

        BlendTreeNode <AnimTreeResult>* node = this->m_leafs[leafIndex];

        if ( node )
        {
            node->onBecomeActive(_inputs);
        }
    }
}

void AnimTreeNodeSequence::onBecomeActive( const InputContainer& _inputs )
{
    setActiveLeaf(_inputs, 0);
    if (getTemplate()->getRandomizeLoopCount())
    {
        m_loopCount = (i32)Seeder::GetUnseededU32(getTemplate()->getRandomLoopCountMin(),getTemplate()->getRandomLoopCountMax()+1);
    }
    else
        m_loopCount = getTemplate()->getLoopCount();
    m_numTimesLooped = 0;
}

void AnimTreeNodeSequence::onTransitionPlay()
{
    if ( m_activeLeaf != U32_INVALID && m_activeLeaf < this->m_leafs.size() )
    {
        BlendTreeNode <AnimTreeResult>* childNode = this->m_leafs[m_activeLeaf];

        if ( childNode )
        {
            childNode->onTransitionPlay();
        }
    }

    Super::onTransitionPlay();
}


}


