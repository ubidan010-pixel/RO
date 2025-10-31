
#include "precompiled_engine.h"

#ifndef _ITF_MUSICTREENODESEQUENCE_H_ 
#include "engine/blendTree/MusicTree/MusicTreeNodeSequence.h"
#endif //_ITF_MUSICTREENODESEQUENCE_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(MusicTreeNodeSequence)
IMPLEMENT_OBJECT_RTTI(MusicTreeNodeSequence_Template)

BEGIN_SERIALIZATION_CHILD(MusicTreeNodeSequence_Template)
    SERIALIZE_MEMBER("startingLeaf",m_startingLeaf);
    SERIALIZE_MEMBER("looping",m_looping);
    SERIALIZE_MEMBER("useIntro",m_useIntro);
END_SERIALIZATION()


void MusicTreeNodeSequence::getPlayingNodes( const InputContainer & _inputs, NodeList& _playingNodes )
{
    if ( m_activeLeaf != U32_INVALID )
    {
        BlendTreeNode <MusicTreeResult>* node = this->m_leafs[m_activeLeaf];
        node->getPlayingNodes( _inputs, _playingNodes );
    }

    Super::getPlayingNodes( _inputs, _playingNodes );
}

void MusicTreeNodeSequence::updateResult( f32 _deltaTime, const InputContainer& _inputs, BlendTreeResultBase* _result )
{
    if ( m_updateHasBeenCalledOnce )
    {
        u32 next = m_activeLeaf + 1;

        if (getTemplate()->getLooping())
        {
            next = next % m_leafs.size();

            // Assuming here that there is at least 2 leafs inside this node !
            // cf ASSERT inside OnBecomeActive() below
            if ( getTemplate()->getUseIntro() && next == 0)
                next++;
        }
        else if (next >= m_leafs.size())
            next = U32_INVALID;

        setActiveLeaf( _inputs, next );
    }
    else
    {
        setActiveLeaf( _inputs, m_activeLeaf );

        m_updateHasBeenCalledOnce = true;
    }

    MusicTreeResult* musicTreeResult = static_cast<MusicTreeResult*>(_result);

    if ( m_activeLeaf != U32_INVALID )
    {
        BlendTreeNode<MusicTreeResult>* childNode = this->m_leafs[m_activeLeaf];
        MusicTreeResult localResult;
        childNode->updateResult(_deltaTime,_inputs,&localResult);

        musicTreeResult->updateBlend(&localResult,bfalse,bfalse,0.0f);
    }

    if ( m_activeLeaf != U32_INVALID )
        musicTreeResult->m_isfinished = bfalse;
    else
        musicTreeResult->m_isfinished = btrue;

    Super::updateResult(_deltaTime,_inputs,_result);
}

void MusicTreeNodeSequence::setActiveLeaf( const InputContainer& _inputs, u32 leafIndex )
{
    //ITF_ASSERT( leafIndex < m_leafs.size() );

    if ( leafIndex < m_leafs.size() )
    {
        m_activeLeaf = leafIndex;

        BlendTreeNode <MusicTreeResult>* node = this->m_leafs[leafIndex];

        if ( node )
        {
            node->onBecomeActive(_inputs);
        }
    }
    else
    {
        m_activeLeaf = U32_INVALID;
    }
}

void MusicTreeNodeSequence::onBecomeActive( const InputContainer& _inputs )
{
    // reinit m_updateHasBeenCalledOnce
    m_updateHasBeenCalledOnce = false;

    u32 startingLeaf = getTemplate()->getStartingLeaf();

    ITF_ASSERT_MSG(!getTemplate()->getUseIntro() || (getTemplate()->getUseIntro() && m_leafs.size() > 1), "You can't use this node with an intro as there is only 1 leaf. Need at least 2 leafs !");

    if ( startingLeaf == U32_INVALID )
    {
        setActiveLeaf(_inputs, 0);
    }
    else
    {
        setActiveLeaf(_inputs, startingLeaf);
    }
}

void MusicTreeNodeSequence::onTransitionPlay()
{
    if ( m_activeLeaf != U32_INVALID && m_activeLeaf < this->m_leafs.size() )
    {
        BlendTreeNode <MusicTreeResult>* childNode = this->m_leafs[m_activeLeaf];

        if ( childNode )
        {
            childNode->onTransitionPlay();
        }
    }

    Super::onTransitionPlay();
}


}


