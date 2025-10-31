
#include "precompiled_engine.h"

#ifndef _ITF_MUSICTREENODERANDOM_H_ 
#include "engine/blendTree/MusicTree/MusicTreeNodeRandom.h"
#endif //_ITF_MUSICTREENODERANDOM_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(MusicTreeNodeRandom)
IMPLEMENT_OBJECT_RTTI(MusicTreeNodeRandom_Template)

BEGIN_SERIALIZATION_CHILD(MusicTreeNodeRandom_Template)
    SERIALIZE_MEMBER("looping",m_looping);
    SERIALIZE_MEMBER("startingLeaf",m_startingLeaf);
END_SERIALIZATION()


void MusicTreeNodeRandom::getPlayingNodes( const InputContainer & _inputs, NodeList& _playingNodes )
{
    if ( m_activeLeaf != U32_INVALID )
    {
        BlendTreeNode <MusicTreeResult>* node = this->m_leafs[m_activeLeaf];
        node->getPlayingNodes( _inputs, _playingNodes );
    }

    Super::getPlayingNodes( _inputs, _playingNodes );
}

void MusicTreeNodeRandom::updateResult( f32 _deltaTime, const InputContainer& _inputs, BlendTreeResultBase* _result )
{
    if ( m_updateHasBeenCalledOnce )
    {
        u32 next = U32_INVALID;

        if (m_indexesLeft.size() > 0)
        {
            u32 nextIndex = Seeder::GetUnseededU32(0,m_indexesLeft.size());
            next = m_indexesLeft[nextIndex];

            m_indexesLeft.removeAt(nextIndex);
            if ( m_indexesLeft.size() == 0 && getTemplate()->getLooping())
                fillArray();
        }

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

void MusicTreeNodeRandom::setActiveLeaf( const InputContainer& _inputs, u32 leafIndex )
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

void MusicTreeNodeRandom::onBecomeActive( const InputContainer& _inputs )
{
    // reinit m_updateHasBeenCalledOnce
    m_updateHasBeenCalledOnce = false;

    u32 startingLeaf = getTemplate()->getStartingLeaf();
    ITF_ASSERT_MSG(startingLeaf == U32_INVALID || (startingLeaf != U32_INVALID && m_leafs.size() > 1), "You can't use this node with an intro as there is only 1 leaf. Need at least 2 leafs !");

    fillArray();

    if ( startingLeaf == U32_INVALID )
    {
        u32 indexInArray= Seeder::GetUnseededU32(0,m_indexesLeft.size());
        u32 index       = m_indexesLeft[indexInArray];

        m_indexesLeft.removeAt(indexInArray);
        if ( m_indexesLeft.size() == 0 && getTemplate()->getLooping())
            fillArray();

        // JP : calling SetActiveLeaf() will call onBecomeActive() on the selected node which is useless in this
        // case as it will be done a second time during updateResult() and it may cause random to not feel so "random"
        //setActiveLeaf(_inputs,index);
        m_activeLeaf    = index;
    }
    else
    {
        // JP : calling SetActiveLeaf() will call onBecomeActive() on the selected node which is useless in this
        // case as it will be done a second time during updateResult() and it may cause random to not feel so "random"
        //setActiveLeaf(_inputs, startingLeaf);
        m_activeLeaf    = startingLeaf;
    }

    Super::onBecomeActive(_inputs);
}

void MusicTreeNodeRandom::onTransitionPlay()
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

void MusicTreeNodeRandom::fillArray()
{
    m_indexesLeft.clear();
    u32 size = m_leafs.size();

    if ( getTemplate()->getStartingLeaf() != U32_INVALID )
    {
        for ( u32 i = 1 ; i < size ; ++i )
        {
            // Avoid playing again the same previous leaf if there are more than
            // 1 leaf available
            if ( size == 1
                || (size > 1 && m_activeLeaf != i) )
            m_indexesLeft.push_back(i);
        }
    }
    else
    {
        for ( u32 i = 0 ; i < size ; ++i )
        {
            // Avoid playing again the same previous leaf if there are more than
            // 1 leaf available
            if ( size == 1
                || (size > 1 && m_activeLeaf != i) )
            m_indexesLeft.push_back(i);
        }
    }
}


}


