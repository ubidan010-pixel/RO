
#ifndef _ITF_MUSICTREERESULT_H_
#define _ITF_MUSICTREERESULT_H_


#ifndef _ITF_BLENDTREERESULT_H_
#include "engine/blendTree/BlendTreeResult.h"
#endif //_ITF_BLENDTREERESULT_H_

namespace ITF
{

//===========================================================================================================
//===========================================================================================================

struct MusicTreeResultLeaf
{
    MusicTreeResultLeaf() : m_part(NULL){}

    class MusicPart *       m_part;
    StringID m_partName;
};

//===========================================================================================================
//===========================================================================================================

struct MusicTreeResult : public BlendTreeResult <MusicTreeResult>
{
    typedef BlendTreeResult <MusicTreeResult> Super;

    MusicTreeResult()
        : m_isfinished(bfalse)
    {}

    virtual void        clear() { Super::clear(); }
    virtual void        updateBlend( BlendTreeResultBase* _blendResult, bbool _main, bbool _mostSignificant, f32 _weight )
    {
        MusicTreeResult* castResult = static_cast<MusicTreeResult*>(_blendResult);
        m_musicLeaf = castResult->m_musicLeaf;
        m_isfinished = castResult->m_isfinished;
    }

    MusicTreeResultLeaf                 m_musicLeaf;
    bbool                               m_isfinished;
};

//===========================================================================================================
//===========================================================================================================

struct MusicTreeLoadResource : public BlendTreeLoadResourceBase
{
    class MusicPart*   m_musicPart;
};

//===========================================================================================================
//===========================================================================================================

struct MusicTreeInitData : public BlendTreeInitData <MusicTreeResult>
{
};

//===========================================================================================================
//===========================================================================================================

#define DECLARE_MUSICTREENODE_TEMPLATE(blendTreeNodeClass) \
public: \
    virtual class BlendTreeNode<MusicTreeResult>* createNode() const { return newAlloc(mId_System, blendTreeNodeClass()); } \

}

#endif // _ITF_MUSICTREERESULT_H_

