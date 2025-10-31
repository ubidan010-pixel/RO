
#ifndef _ITF_BLENDTREERESULT_H_
#define _ITF_BLENDTREERESULT_H_

#ifndef _ITF_INPUTCRITERIAS_H_
#include "engine/blendTree/InputCriterias.h"
#endif //_ITF_INPUTCRITERIAS_H_

namespace ITF
{

template <class RESULTTYPE>
class BlendTreeNode;

template <class RESULTTYPE>
class BlendTreeTemplate;

//===========================================================================================================
//===========================================================================================================

struct BlendTreeResultBase
{
    virtual void        updateBlend( BlendTreeResultBase* _blendResult, bbool _main, bbool _mostSignificant, f32 _weight ) = 0;
};

//===========================================================================================================
//===========================================================================================================

template <class RESULTTYPE>
struct BlendTreeResult : public BlendTreeResultBase
{
    BlendTreeResult()  {}

    virtual void                                 clear() { m_playingNodes.clear();  }
    virtual void                                 updateBlend( BlendTreeResultBase* _blendResult, bbool _main, bbool _mostSignificant, f32 _weight )
    {
        BlendTreeResult <RESULTTYPE>* otherResult = static_cast<BlendTreeResult <RESULTTYPE>* >(_blendResult);

        u32 nodeNamesSize = otherResult->m_playingNodes.size();
        m_playingNodes.reserve(nodeNamesSize);

        for ( u32 nameIndex  = 0; nameIndex < nodeNamesSize; ++nameIndex )
        {
            m_playingNodes.push_back(otherResult->m_playingNodes[nameIndex]);
        }
    }

    SafeArray < BlendTreeNode <RESULTTYPE> * /*, 8, MemoryId::mId_Temporary*/ >   m_playingNodes;
};

//===========================================================================================================
//===========================================================================================================

struct BlendTreeInitDataBase
{
    InputContainer*                                 m_inputs;
};

//===========================================================================================================
//===========================================================================================================

template <class RESULTTYPE>
struct BlendTreeInitData : public BlendTreeInitDataBase
{
    ITF_MAP<StringID,BlendTreeNode <RESULTTYPE>* >* m_namedNodes;
};

//===========================================================================================================
//===========================================================================================================

struct BlendTreeLoadResourceBase
{

};

//===========================================================================================================
//===========================================================================================================

}

#endif // _ITF_BLENDTREERESULT_H_

